import threading
import serial
import serial.tools.list_ports
import matplotlib.pyplot as plt
import math    
import tkinter as tk
from tkinter import *
from tkinter.ttk import Combobox  
from tkinter.messagebox import showerror, showwarning, showinfo
x_lim = 10
y_lim = 10
step = 10
is_plot = True
x=[]
y=[]
for _ in range(360):
    x.append(0)
    y.append(0)
window = Tk()
def draw():
    global is_plot
    while is_plot:
        plt.figtext(0.5, -0.1, "figtext")
        plt.suptitle("Lidar 2D")
        plt.figtext(-11,3,"figtext")
        plt.xlabel("Ось X, см")
        plt.figtext(-11,-1,"figtext")
        plt.ylabel("Ось Y, см")
        plt.ylim(-y_lim,y_lim)
        plt.xlim(-x_lim,x_lim)
        plt.scatter(x,y,c='r',s=10)
        plt.pause(0.1)
        plt.cla()
    plt.close("all")
# def clicked_():
#     threading.Thread(target=clicked).start()
def clicked():
    global y_lim
    global x_lim
    global var_combobox
    # window.quit()
    try:
        uart = serial.Serial(port = var_combobox.get() ,baudrate = 115200)
        # print((str)(uart.name)+":"+(str)(uart.is_open))
        # window.destroy()
        threading.Thread(target=draw).start()
        if uart.is_open:
            st = f"$LDESP,0,0,0,0,180,{step},"
            # st = f"$LDESP,0,0,0,0,0,0,"
            uart.write(st.encode()) 
            while uart.is_open:
                data = uart.readline().decode().strip()
                index = str(data).find('\t') + 1
                angle = int(str(data)[index: str(data).find('\t',index)])
                index = str(data).find('\t', index) + 1
                dist = int(str(data)[index: len(str(data))])
                x[angle] = dist * math.cos(math.radians(angle))
                y[angle] = dist * math.sin(math.radians(angle))
                if (dist> y_lim) or (dist> x_lim):
                    y_lim = dist
                    x_lim = dist
                    plt.ylim(-y_lim,y_lim)
                    plt.xlim(-x_lim,x_lim)
                elif (dist < y_lim - 10) and (dist< x_lim-10):
                    flag_mash = True
                    for i in range(0,360):
                        if(y[i]> y[angle]) and (x[i]> x[angle]):
                            flag_mash = False
                    if flag_mash:
                        y_lim = dist
                        x_lim = dist
                        plt.ylim(-y_lim,y_lim)
                        plt.xlim(-x_lim,x_lim)
        is_plot = False
    except:
        showerror(title="Ошибка", message="Сообщение об ошибке")
        # start_form()
def update():
    global combo
    ports = serial.tools.list_ports.comports()
    ports2=[]
    for port, desc, hwid in sorted(ports):
        ports2.append(port)
    combo['values'] = ports2
def start_form():
    global var_combobox
    global combo
    window.title("Lidar 2D")
    window.geometry('250x30')
    var_combobox = tk.StringVar()
    btn = Button(window, text = "Start", command= clicked)
    btn.grid(column =1,row=0)
    btn = Button(window, text = "Обновить", command= update)
    btn.grid(column =3,row=0)
    combo = Combobox(window)  
    ports = serial.tools.list_ports.comports()
    ports2=[]
    for port, desc, hwid in sorted(ports):
        ports2.append(port)
    combo['values'] = ports2
    combo['textvariable'] = var_combobox
    combo.current(1)  # установите вариант по умолчанию  
    combo.grid(column=0, row=0)  
    window.mainloop()
start_form()
