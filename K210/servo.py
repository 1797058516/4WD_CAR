from machine import Timer,PWM
import utime
from fpioa_manager import fm
from machine import UART
import sensor,image,lcd,time
#from pyb import millis
from math import pi,isnan
import time
#追踪颜色
#红色阈值[0],绿色阈值[1],蓝色阈值[2]
rgb_thresholds   =[(30, 100, 15, 127, 15, 127),
(0, 80, -70, -10, -0, 30),
(0, 30, 0, 64, -128, -20)]   #阈值调试可以在IDE上看哦
thresholds = (90, 100, -128, 127, -128, 127)
#摄像头
sensor.reset()
#sensor.set_vflip(1)#翻转摄像头
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 200)
#sensor.set_auto_gain(False) # must be turned off for color tracking
#sensor.set_auto_whitebal(False) # must be turned off for color tracking
sensor.run(1)
#PID
kp = 0.45
ki = 0
kd = 0
integrator = 0
last_error = 0
dt=0.05  #50ms
output = 0
err=0
target = 160
feedback =0
isBegin = True
def pid_timer(tim):
    #global kp,ki,kd,integrator,last_error,dt,output,target,feedback,isBegin
    global kp,kd,ki,err,last_error,isBegin,output,dt,integrator
    #if dt == 0 or dt_add > 1000:
        #dt_add = 0
        #integrator = 0
    #err = target -feedback
    print("err - last_error",err - last_error)
    if (isBegin):
        last_error = err;
        isBegin = False;
    integrator += ki*(err+last_error)/2
    output=kp*err + integrator*dt +kd*(err - last_error)/ dt
    last_error=err
    print("err:",err)
    print("integrator",integrator)
    #print("output",output)
    tim_pid.stop()
#MODE_PERIODIC--连续回调 1000*1ms=1s
tim_pid = Timer(Timer.TIMER1, Timer.CHANNEL0, mode=Timer.MODE_PERIODIC, period=100, unit=Timer.UNIT_MS, callback=pid_timer ,start=False, priority=1, div=0)

#PWM 通过定时器配置，接到 IO17 引脚
tim1 = Timer(Timer.TIMER0, Timer.CHANNEL0, mode=Timer.MODE_PWM)
tim2 = Timer(Timer.TIMER0, Timer.CHANNEL1, mode=Timer.MODE_PWM)
servo1 = PWM(tim1, freq=50, duty=7.5, pin=16)
servo2 = PWM(tim2, freq=50, duty=7.5, pin=17)

#UART
fm.register(9, fm.fpioa.UART1_TX, force=True)
fm.register(10, fm.fpioa.UART1_RX, force=True)
uart_A = UART(UART.UART1, 115200, 8, 0, 1, timeout=1000, read_buf_len=100)

#舵机对应不同角度,num范围为-90--90
def servoangle(num,angle):
    if angle>60:
        angle = 60
    elif angle<-60:
        angle = -60
    freq=angle*(5/90)
    duty=7.5+freq
    if num==1:
        servo1.duty(duty)
    else:
        servo2.duty(duty)

def uart1_rec():
    if uart_A.any():
        while uart_A.any():
            rec=uart_A.read()
            print("recv = ",rec)
            uart_A.write(rec)

#寻找最大色块函数定义
def find_max(blobs):
    max_size=0
    for blob in blobs:
        if blob[2]*blob[3] > max_size:
            max_blob=blob
            max_size = blob[2]*blob[3]
    return max_blob
#寻找目标色块
def find_target():
    blobs=img.find_blobs([rgb_thresholds[0]])
    if blobs :
        for b in blobs:
            tmp=img.draw_rectangle(b[0:4])##在图像上绘制一个矩形。
            tmp=img.draw_cross(b[5], b[6])##画十字交叉
while True:
    #utime.sleep_ms(30)
    #servoangle(1,-60)
    #uart1_rec()
    #img=sensor.snapshot().binary([thresholds], invert=False, zero=True).mean(1)

    #tim_pid.stop()
    img=sensor.snapshot()
    blobs=img.find_blobs([rgb_thresholds[0]],area_threshold=300)
    if blobs :
        tim_pid.start()
        for b in blobs:
            tmp=img.draw_rectangle(b[0:4])##在图像上绘制一个矩形。
            tmp=img.draw_cross(b[5], b[6])##画十字交叉
            feedback=b[5]
            err=target-feedback
            servoangle(1,output)
            #print(b[5])
            #uart_A.write('\1'+","+str(b[5])+","+str(b[6])+","+"\r\n")
            uart_A.write("160"+","+str(b[5])+"\r\n")
    #uart_A.write("160"+","+"120"+"\r\n")
    #find_target()

    #pan_output=pan_pid.get_pid(10,1)/2

