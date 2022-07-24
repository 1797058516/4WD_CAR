from machine import Timer,PWM
import utime
from fpioa_manager import fm
from machine import UART
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
#UART
fm.register(9, fm.fpioa.UART1_TX, force=True)
fm.register(10, fm.fpioa.UART1_RX, force=True)
uart_A = UART(UART.UART1, 115200, 8, 0, 1, timeout=1000, read_buf_len=100)
def uart1_rec():
    if uart_A.any():
        while uart_A.any():
            rec=uart_A.read()
            print("recv = ",rec)
            uart_A.write(rec)

while True:
    #utime.sleep_ms(30)
    #servoangle(2,0)
    uart1_rec()
