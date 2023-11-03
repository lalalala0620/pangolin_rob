#Copyright Deng（灯哥） (ream_d@yeah.net)  Py-apple dog project
#Github:https://github.com/ToanTech/py-apple-quadruped-robot
#Licensed under the Apache License, Version 2.0 (the "License");
#you may not use this file except in compliance with the License.
#You may obtain a copy of the License at:http://www.apache.org/licenses/LICENSE-2.0

from math import asin,acos,atan,pi,sqrt
# from ServoCmd import *
# from ServoCmd import *
import time

init_1h=90

init_1s=90

init_2h=90

init_2s=90

init_3h=90

init_3s=90

init_4h=90

init_4s=90

l1=80

l2=69

l=142

b=92.8

w=108

speed=0.05

h=30

Kp_H=0.06

pit_Kp_G=0.04

pit_Kd_G=0.6

rol_Kp_G=0.04

rol_Kd_G=0.35

tran_mov_kp=0.1

CC_M=0

walk_h=50

walk_speed=0.02

ma_case=0

trot_cg_f=4

trot_cg_b=4

trot_cg_t=2

in_y=17


def swing_curve_generate(t,Tf,xt,zh,x0,z0,xv0):
  #输入参数：当前时间；支撑相占空比；x方向目标位置，z方向抬腿高度，摆动相抬腿前腿速度
  # X Generator
  if t>=0 and t<Tf/4:
    xf=(-4*xv0/Tf)*t*t+xv0*t+x0
    
  if t>=Tf/4 and t<(3*Tf)/4:
    xf=((-4*Tf*xv0-16*xt+16*x0)*t*t*t)/(Tf*Tf*Tf)+((7*Tf*xv0+24*xt-24*x0)*t*t)/(Tf*Tf)+((-15*Tf*xv0-36*xt+36*x0)*t)/(4*Tf)+(9*Tf*xv0+16*xt)/(16)
    
  if t>(3*Tf)/4:
    xf=xt

  # Z Generator
  if t>=0 and t<Tf/2:
    zf=(16*z0-16*zh)*t*t*t/(Tf*Tf*Tf)+(12*zh-12*z0)*t*t/(Tf*Tf)+z0
  
  if t>=Tf/2:
    zf=(4*z0-4*zh)*t*t/(Tf*Tf)-(4*z0-4*zh)*t/(Tf)+z0
      
  #Record touch down position
  x_past=xf
  t_past=t
  
  # # Avoid zf to go zero
  if zf<=0:
    zf=0
  #x,z position,x_axis stop point,t_stop point;depend on when the leg stop
  
  return xf,zf,x_past,t_past


def support_curve_generate(t,Tf,x_past,t_past,zf):
  # 当前时间；支撑相占空比；摆动相 x 最终位;t最终时间，支撑相 zf
  # Only X Generator
  average=x_past/(1-Tf)
  xf=x_past-average*(t-t_past)
  return xf,zf

def trot(t,x_target,z_target,r1,r4,r2,r3):
  global x1,x2,x3,x4,y1,y2,y3,y4
  Tf=0.5
  if t<Tf:
    phase_1_swing=swing_curve_generate(t,Tf,x_target,z_target,0,0,0)
    phase_1_support=support_curve_generate(0.5+t,Tf,x_target,0.5,0)
    #TROT
    x1=phase_1_swing[0]*r1;x2=phase_1_support[0]*r2;x3=phase_1_swing[0]*r3;x4=phase_1_support[0]*r4
    y1=phase_1_swing[1];y2=phase_1_support[1];y3=phase_1_swing[1];y4=phase_1_support[1]
    
  if t>=Tf:
    phase_2_swing=swing_curve_generate(t-0.5,Tf,x_target,z_target,0,0,0)
    phase_2_support=support_curve_generate(t,Tf,x_target,0.5,0)
    #TROT
    x1=phase_2_support[0]*r1;x2=phase_2_swing[0]*r2;x3=phase_2_support[0]*r3;x4=phase_2_swing[0]*r4
    y1=phase_2_support[1];y2=phase_2_swing[1];y3=phase_2_support[1];y4=phase_2_swing[1]
  return x1,x2,x3,x4,y1,y2,y3,y4

def ik(l1,l2,x1,x2,x3,x4,y1,y2,y3,y4):
    #=============串联腿=============
    #腿1
    x1=-x1
    shank1=pi-acos((x1*x1+y1*y1-l1*l1-l2*l2)/(-2*l1*l2))
    fai1=acos((l1*l1+x1*x1+y1*y1-l2*l2)/(2*l1*sqrt(x1*x1+y1*y1)))
    if x1>0:
        ham1=(abs(atan(y1/x1))-fai1)
    elif x1<0:
        ham1=(pi-abs(atan(y1/x1))-fai1)
    else:
        ham1=(pi-1.5707-fai1)
    shank1=180*shank1/pi
    ham1=180-180*ham1/pi
    # print(shank1, ham1)

    #腿2
    x2=-x2
    shank2=pi-acos((x2*x2+y2*y2-l1*l1-l2*l2)/(-2*l1*l2))

    fai2=acos((l1*l1+x2*x2+y2*y2-l2*l2)/(2*l1*sqrt(x2*x2+y2*y2)))
    if x2>0:
        ham2=abs(atan(y2/x2))-fai2
    elif x2<0:
        ham2=pi-abs(atan(y2/x2))-fai2
    else:
        ham2=pi-1.5707-fai2
    shank2=180-180*shank2/pi
    ham2=180*ham2/pi

    #腿3
    x3=-x3
    shank3=pi-acos((x3*x3+y3*y3-l1*l1-l2*l2)/(-2*l1*l2))
    fail3=acos((l1*l1+x3*x3+y3*y3-l2*l2)/(2*l1*sqrt(x3*x3+y3*y3)))
    if x3>0:
        ham3=abs(atan(y3/x3))-fail3
    elif x3<0:
        ham3=pi-abs(atan(y3/x3))-fail3
    else:
        ham3=pi-1.5707-fail3
    shank3=180-180*shank3/pi
    ham3=180*ham3/pi

    #腿4
    x4=-x4
    shank4=pi-acos((x4*x4+y4*y4-l1*l1-l2*l2)/(-2*l1*l2))
    fai4=acos((l1*l1+x4*x4+y4*y4-l2*l2)/(2*l1*sqrt(x4*x4+y4*y4)))
    if x4>0:
        ham4=abs(atan(y4/x4))-fai4
    elif x4<0:
        ham4=pi-abs(atan(y4/x4))-fai4
    else:
        ham4=pi-1.5707-fai4

    shank4=180*shank4/pi
    ham4=180-180*ham4/pi
    
    return ham1,ham2,ham3,ham4,shank1,shank2,shank3,shank4
    # return ham1, shank1, 

l1 = 76
l2 = 83
x1 = 50
x2 = 50
x3 = 50
x4 = 50
y1 = 50
y2 = 50
y3 = 50
y4 = 50

if __name__ == "__main__":

    # print(l1, l2, x1, y1)
    # ham1,ham2,ham3,ham4,shank1,shank2,shank3,shank4 = ik(l1,l2,x1,x2,x3,x4,y1,y2,y3,y4)
    # print(ham1, shank1)
    # print(int(ham1*2000/180+500), int(shank1*2000/180+500))
    # setServoPulse(3, int(ham1*2000/180+500), 2000)
    # setServoPulse(4, int(shank1*2000/180+500), 2000)

    # print(ham2, shank2)
    # print(int(ham2*2000/180+500), int(shank2*2000/180+500))
    # setServoPulse(1, int(ham2*2000/180+500), 2000)
    # setServoPulse(2, int(shank2*2000/180+500), 2000)

    # print(ham3, shank3)
    # print(int(ham3*2000/180+500), int(shank3*2000/180+500))
    # setServoPulse(5, int(ham3*2000/180+500), 2000)
    # setServoPulse(6, int(shank3*2000/180+500), 2000)

    # print(ham4, shank4)
    # print(int(ham4*2000/180+500), int(shank4*2000/180+500))
    # setServoPulse(7, int(ham4*2000/180+500), 2000)
    # setServoPulse(8, int(shank4*2000/180+500), 2000)

    # time.sleep(2)
    # allServoRelease()
    # print(ik)
    # t = 10
    # x_target = 10
    # z_target = 10
    # r1 = 10
    # r4 = 10
    # r2 = 10
    # r3 = 10
    # trot1 = trot(t,x_target,z_target,r1,r4,r2,r3)
    # print(trot1)
    import matplotlib.pyplot as plt
    import matplotlib.animation as animation
    import numpy as np


    speed = 0.05
    h=50
    gait_mode = 0
    t = 0
    L = 1
    R = 1
    x = []
    y = []
    i = 0
    # pigpio = pigpio.pi()
    PWMServo_IO_dict = {1:4, 2:18, 3:27, 4:10, 5:20, 6:19, 7:13, 8:6, 9:11, 10:5}
    while i < 200:

        spd = 1
        if gait_mode==0:
            act_tran_mov_kp=tran_mov_kp
            if t>=1:#一个完整的运动周期结束 trot
                t=0
            elif L==0 and R==0:
                t=0
            else:
                t=t+speed
            x1,x2,x3,x4,y1,y2,y3,y4=trot(t,spd*5,h,L,L,R,R)
        i += 1
        print(i)
        # y1 = 100-y1
        # y2 = 100-y2
        # y3 = 100-y3
        # y4 = 100-y4
        print(x1,y1)
        x.append(x1)
        y.append(y1)
        # print(l1, l2, x1, y1)
        # ham1,ham2,ham3,ham4,shank1,shank2,shank3,shank4 = ik(l1,l2,x1,x2,x3,x4,y1,y2,y3,y4)
        # print(ham1, shank1)
        # print(int(ham1*2000/180+500), int(shank1*2000/180+500))
        # setServoPulse(3, int(ham1*2000/180+500), speed*1000)
        # setServoPulse(4, int(shank1*2000/180+500), speed*1000)
        # setServoPulse(1, int(ham2*2000/180+500), speed*1000)
        # setServoPulse(2, int(shank2*2000/180+500), speed*1000)
        # setServoPulse(5, int(ham3*2000/180+500), speed*1000)
        # setServoPulse(6, int(shank3*2000/180+500), speed*1000)
        # setServoPulse(7, int(ham4*2000/180+500), speed*1000)
        # setServoPulse(8, int(shank4*2000/180+500), speed*1000)
        # pigpio.set_servo_pulsewidth(27, int(ham1*2000/180+500))
        # pigpio.set_servo_pulsewidth(10, int(shank1*2000/180+500))


        # time.sleep(speed)
        # time.sleep(2)
    # allServoRelease()
    fig, ax = plt.subplots()
    def plot(t):
        if t % 20 == 0:
            ax.clear()
        ax.set_xlim(min(x)-0.5,max(x)+0.5)
        ax.set_ylim(min(y)-0.5,max(y)+0.5)
        # ax.set(xlim=(0, 30), xticks=np.arange(1, 30), ylim=(0, 4), yticks=np.arange(1, 4))   
        ax.scatter(x[t], y[t])
    
    ti = [i for i in range(200)]
    ani = animation.FuncAnimation(fig, plot, frames=ti, interval=10)
    writer = animation.PillowWriter(fps=15, metadata=dict(artist='Me'), bitrate=1800)
    print("saving gif")
    ani.save('gait.gif', writer=writer)

    # k = np.linspace(0, 500, 500)
    # ax.plot(k, x, linewidth=2.0) 
    # plt.show()
        
    