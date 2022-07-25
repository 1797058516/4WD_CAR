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
sensor.set_vflip(1)#翻转摄像头
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 200)
#sensor.set_auto_gain(False) # must be turned off for color tracking
#sensor.set_auto_whitebal(False) # must be turned off for color tracking
sensor.run(1)
#PID
def pid_timer(_error):
    if last_t == 0 or dt > 1000:
        dt = 0
        integrator = 0
    output+=error*kp
    last_error=error
    derivative = (error - self._last_error) / dt

    print("time up:",timer)
    print("param:",timer.callback_arg())
#MODE_PERIODIC--连续回调
#tim_pid = Timer(Timer.TIMER1, Timer.CHANNEL0, mode=Timer.MODE_PERIODIC, period=1000, unit=Timer.UNIT_MS, callback=pid_timer, arg=on_timer, start=False, priority=1, div=0)

kp = ki = kd = integrator = imax = 0
last_error = last_derivative = last_t = 0
dt=0.05  #50ms
output = 0

class PID:
    _kp = _ki = _kd = _integrator = _imax = 0
    _last_error = _last_derivative = _last_t = 0
    _RC = 1/(2 * pi * 20)
    def __init__(self, p=0, i=0, d=0, imax=0):
        self._kp = float(p)
        self._ki = float(i)
        self._kd = float(d)
        self._imax = abs(imax)
        self._last_derivative = float('nan')
    def get_pid(self, error, scaler):
        #tnow = millis()
        #tnow =
        dt = tnow - self._last_t
        output = 0
        if self._last_t == 0 or dt > 1000:
            dt = 0
            self.reset_I()
        self._last_t = tnow
        delta_time = float(dt) / float(1000)
        output += error * self._kp
        if abs(self._kd) > 0 and dt > 0:
            if isnan(self._last_derivative):
                derivative = 0
                self._last_derivative = 0
            else:
                derivative = (error - self._last_error) / delta_time
            derivative = self._last_derivative + \
                                     ((delta_time / (self._RC + delta_time)) * \
                                        (derivative - self._last_derivative))
            self._last_error = error
            self._last_derivative = derivative
            output += self._kd * derivative
        output *= scaler
        if abs(self._ki) > 0 and dt > 0:
            self._integrator += (error * self._ki) * scaler * delta_time
            if self._integrator < -self._imax: self._integrator = -self._imax
            elif self._integrator > self._imax: self._integrator = self._imax
            output += self._integrator
        return output
    def reset_I(self):
        self._integrator = 0
        self._last_derivative = float('nan')
#实例化PID
pan_pid = PID(p=0.07, i=0, imax=90) #脱机运行或者禁用图像传输，使用这个PID
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
    servoangle(1,0)
    uart1_rec()
    #img=sensor.snapshot().binary([thresholds], invert=False, zero=True).mean(1)
    #tim_pid.start()
    img=sensor.snapshot()
    blobs=img.find_blobs([rgb_thresholds[0]])
    if blobs :
        for b in blobs:
            tmp=img.draw_rectangle(b[0:4])##在图像上绘制一个矩形。
            tmp=img.draw_cross(b[5], b[6])##画十字交叉
            x=b.cx()
            print(b[5])
            uart_A.write('\1'+","+str(b[5])+","+str(b[6])+","+"\r\n")
    #find_target()

    #pan_output=pan_pid.get_pid(10,1)/2

