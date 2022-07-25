from machine import Timer,PWM
import utime
#PWM 通过定时器配置，接到 IO17 引脚
tim1 = Timer(Timer.TIMER0, Timer.CHANNEL0, mode=Timer.MODE_PWM)
tim2 = Timer(Timer.TIMER0, Timer.CHANNEL1, mode=Timer.MODE_PWM)
servo1 = PWM(tim1, freq=50, duty=7.5, pin=16)
servo2 = PWM(tim2, freq=50, duty=7.5, pin=17)

#舵机对应不同角度,num范围为-90--90
def servoangle(num,angle):
    freq=angle*(5/90)
    duty=7.5+freq
    if num==1:
        servo1.duty(duty)
    else:
        servo2.duty(duty)

while True:
    utime.sleep_ms(30)
    #servo.duty(10)
    servoangle(2,0)
