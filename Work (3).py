
# import time
# from time import sleep
import threading
import serial
import serial.tools.list_ports
import matplotlib.pyplot as plt
import math    
from tkinter import *
from tkinter.ttk import Combobox
import tkinter as tk
import numpy as np
from matplotlib.widgets import Button as btt
# $LDESP,0,0,0,0,180,10,

#fig = plt.figure()
#plt.subplots_adjust(left = 0.3, bottom = 0.25)

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


#def start():
    #global running
    #running =True
#def stop():
   ## global running
    #running = False

def draw():
    global is_plot
    while is_plot:
        plt.figtext(0.5, -0.1, "figtext")
        plt.suptitle("Интеллектуальный сканирующий датчик")
        plt.figtext(-11,3,"figtext")
        plt.xlabel("Ось X, см")
        plt.figtext(-11,-1,"figtext")
        plt.ylabel("Ось Y, см")
        #fig = plt.figure()
        #plt.subplots_adjust(left = 0.3, bottom = 0.25)
        #fig, graph_axes = plt.subplots()
        #graph_axes.grid()
        #fig.subplots_adjust(left=0.07, right=0.95, top=0.95, bottom=0.2)
        
        plt.ylim(-y_lim,y_lim)
        plt.xlim(-x_lim,x_lim)
        plt.scatter(x,y,c='r',s=10)
        plt.pause(0.10)
        
        plt.cla()
        #axes_button_add = plt.axes([0.7,0.05,0.25,0.075])
        #button_add = btt(axes_button_add, 'Stop')
    plt.close("all")
   

def clicked_():
    threading.Thread(target=clicked).start()
def clicked():
    global y_lim
    global x_lim
    global combo
    global uart
    global th
    global is_plot
    global var_combobox
    is_plot = True
    # window.quit()
    #window.destroy()
    #syka = combo.get()
    # print("-------------------------------")
   # print(syka)
    # print("-------------------------------")
    try: 
        uart = serial.Serial(port = var_combobox.get() ,baudrate = 115200)
        # print((str)(uart.name)+":"+(str)(uart.is_open))
        th = threading.Thread(target=draw).start()
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
   # is_plot = False
    except:
        print("exept")
    
def update():
    global combo
    ports = serial.tools.list_ports.comports()
    ports2=[]
    for port, desc, hwid in sorted(ports):
        ports2.append(port)
    combo['values'] = ports2


def pause():
    st = "$LDESP,0,0,0,0,0,0,"
    uart.write(st.encode())
    uart.close()
def stop():
    global is_plot
    is_plot = False
    st = "$LDESP,0,0,0,0,0,0,"
    uart.write(st.encode())
    uart.close()
    
    
    

    
window.title("Lidar 2D")
window.geometry('400x250')
btn = Button(window, text = "Start", command= clicked_)
btn.grid(column =1,row=0)
btn_pause = Button(window, text = "Pause", command = pause)
btn_pause.grid(column=5, row = 0)
#fig.subplots_adjust(left=0.07, right=0.95, top=0.95, bottom=0.2)
#axes_button_add = plt.axes([0.7,0.05,0.25,0.075])
#button_add = btt(axes_button_add, 'Start')
#addPlot(graph_axes)
btn_stop = Button(window, text = "Stop", command = stop)
btn_stop.grid(column = 7, row = 0)
btn = Button(window, text = "Обновить", command= update)
btn.grid(column =3,row=0)
var_combobox = tk.StringVar()
combo = Combobox(window)  
ports = serial.tools.list_ports.comports()
ports2=[]
for port, desc, hwid in sorted(ports):
    ports2.append(port)
combo['values'] = ports2
combo['textvariable'] = var_combobox
#combo.current(1)  # установите вариант по умолчанию  
combo.grid(column=0, row=0)
menu = Menu(window)
#new_item = Menu(menu)
#new_item.add_command(label='Новый', command = clicked)
#new_item.add_separator()
#menu.add_cascade(label = 'Файл', menu = new_item)
#new_item.add_command(label='Изменить', command = clicked)
#new_item.add_separator()
window.config(menu=menu)
window.mainloop()
