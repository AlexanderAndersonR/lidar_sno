
# import time
# from time import sleep
import threading
import serial
import matplotlib.pyplot as plt
import math    
# $LDESP,0,0,0,0,180,10,
x_lim = 10
y_lim = 10
step = 10
def draw():
    global is_plot
    while is_plot:
        plt.figure(1)
        plt.cla()
        plt.ylim(-y_lim,y_lim)
        plt.xlim(-x_lim,x_lim)
        plt.scatter(x,y,c='r',s=8)
        plt.pause(1)
    plt.close("all")
is_plot = True
x=[]
y=[]
for _ in range(360):
    x.append(0)
    y.append(0)
# port = input(", к которому подключен лидар:")
uart = serial.Serial(port = "COM22",baudrate = 115200)
print((str)(uart.name)+":"+(str)(uart.is_open))
threading.Thread(target=draw).start()
if uart.is_open:
    st = f"$LDESP,0,0,0,0,180,{step},"
    uart.write(st.encode()) 
    # 0 угол дальность
    # uart.write(b"$LDESP,0,0,0,0,180,10,")
    while uart.is_open:
        data = uart.readline().decode().strip()
        # str(data)[index: str(data).find(',', index)]
        # angle = str(data)[index: str(data).find(' ',)
        index = str(data).find('\t') + 1
        # print(index)
        angle = int(str(data)[index: str(data).find('\t',index)])
        index = str(data).find('\t', index) + 1
        dist = int(str(data)[index: len(str(data))])
        x[angle] = dist * math.cos(math.radians(angle))
        y[angle] = dist * math.sin(math.radians(angle))
        if (dist> y_lim) or (dist> x_lim):
            # print(x_lim)
            # print(y_lim)
            # y_lim = dist
            # x_lim = dist
            # is_plot = False
            # threading.Thread(target=draw).start()
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
        # print(x)
        # print(y)
        # print(angle)
        # print(dist)
        # data =str(data)
        # print(data)
is_plot = False
# Obj = ...
# if (Obj.connect()):
#     print(Obj.GetDeviceInfo())
#     gen = Obj.StartScanning()
#     t = time.time()
#     while (time.time()-t)<30:
#         print(next(gen))
#         time.sleep(0.5)
#     Obj.StopScanning()
#     Obj.Disconnect()
# else:
#     print("Ошибка подключения устройства")
#     print("23",chr(176),sep="")



# line = ser.readline().decode().strip()
