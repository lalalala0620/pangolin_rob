from math import asin,acos,atan,pi,sqrt
import PuppyPI_Config

# Servo: 
# 
#  (o o)
# 1-----2
#   |||
#   |||
# 4-----3
#    *
#    *

class Puppy_Control():
    def __init__(self):
        self.upper_leg_length = PuppyPI_Config.upper_leg_length
        self.lower_leg_length = PuppyPI_Config.lower_leg_length
        self.leg_1_x = PuppyPI_Config.leg_1_x
        self.leg_2_x = PuppyPI_Config.leg_2_x
        self.leg_3_x = PuppyPI_Config.leg_3_x
        self.leg_4_x = PuppyPI_Config.leg_4_x
        self.leg_1_y = PuppyPI_Config.leg_1_y
        self.leg_2_y = PuppyPI_Config.leg_2_y
        self.leg_3_y = PuppyPI_Config.leg_3_y
        self.leg_4_y = PuppyPI_Config.leg_4_y

        self.dt = 0.05
        self.t = 0
        self.speed = 0.05

    def legInverseKinematics(self):
        
        #Leg1
        self.leg_1_x = -self.leg_1_x
        lower_leg_1 = pi-acos((self.leg_1_x**2+self.leg_1_y**2-self.upper_leg_length**2-self.lower_leg_length**2)/(-2*self.upper_leg_length**2))
        phi_1 = acos((self.upper_leg_length**2+self.leg_1_x**2+self.leg_1_y**2-self.lower_leg_length**2)/(2*self.upper_leg_length*sqrt(self.leg_1_x**2+self.leg_1_y**2)))
        
        if self.leg_1_x>0:
            upper_leg_1 = (abs(atan(self.leg_1_y/self.leg_1_x))-phi_1)
        elif self.leg_1_x < 0:
            upper_leg_1 = (pi-abs(atan(self.leg_1_y/self.leg_1_x))-phi_1)
        else:
            upper_leg_1 = (pi-1.5707-phi_1)

        lower_leg_1=180 * lower_leg_1 / pi
        upper_leg_1=180 - 180 * upper_leg_1 / pi

        #Leg2
        self.leg_2_x = -self.leg_2_x
        lower_leg_2 = pi-acos((self.leg_2_x**2+self.leg_2_y**2-self.upper_leg_length**2-self.lower_leg_length**2)/(-2*self.upper_leg_length**2))
        phi_2 = acos((self.upper_leg_length**2+self.leg_2_x**2+self.leg_2_y**2-self.lower_leg_length**2)/(2*self.upper_leg_length*sqrt(self.leg_2_x**2+self.leg_2_y**2)))
        
        if self.leg_2_x>0:
            upper_leg_2=abs(atan(self.leg_2_y/self.leg_2_x))-phi_2
        elif self.leg_2_x<0:
            upper_leg_2=pi-abs(atan(self.leg_2_y/self.leg_2_x))-phi_2
        else:
            upper_leg_2=pi-1.5707-phi_2

        lower_leg_2=180-180*lower_leg_2/pi
        upper_leg_2=180*upper_leg_2/pi

        #Leg3
        self.leg_3_x = -self.leg_3_x
        lower_leg_3 = pi-acos((self.leg_3_x**2+self.leg_3_y**2-self.upper_leg_length**2-self.lower_leg_length**2)/(-2*self.upper_leg_length**2))
        phi_3 = acos((self.upper_leg_length**2+self.leg_3_x**2+self.leg_3_y**2-self.lower_leg_length**2)/(2*self.upper_leg_length*sqrt(self.leg_3_x**2+self.leg_3_y**2)))
        
        if self.leg_3_x > 0:
            upper_leg_3 = abs(atan(self.leg_3_y/self.leg_3_x))-phi_3
        elif self.leg_3_x < 0:
            upper_leg_3 = pi-abs(atan(self.leg_3_y/self.leg_3_x))-phi_3
        else:
            upper_leg_3 = pi-1.5707-phi_3

        lower_leg_3 = 180-180*lower_leg_3/pi
        upper_leg_3 = 180*upper_leg_3/pi

        #Leg4
        self.leg_4_x = -self.leg_4_x
        lower_leg_4 = pi-acos((self.leg_4_x**2+self.leg_4_y**2-self.upper_leg_length**2-self.lower_leg_length**2)/(-2*self.upper_leg_length**2))
        phi_4 = acos((self.upper_leg_length**2+self.leg_4_x**2+self.leg_4_y**2-self.lower_leg_length**2)/(2*self.upper_leg_length*sqrt(self.leg_4_x**2+self.leg_4_y**2)))
        
        if self.leg_4_x > 0:
            upper_leg_4 = abs(atan(self.leg_4_y/self.leg_4_x))-phi_4
        elif self.leg_4_x < 0:
            upper_leg_4 = pi-abs(atan(self.leg_4_y/self.leg_4_x))-phi_4
        else:
            upper_leg_4 = pi-1.5707-phi_4

        lower_leg_4 = 180*lower_leg_4/pi
        upper_leg_4 = 180-180*upper_leg_4/pi
        
        return upper_leg_1, upper_leg_2, upper_leg_3, upper_leg_4, lower_leg_1, lower_leg_2, lower_leg_3, lower_leg_4
    

    def gaitTrot(self, x_target, z_target, r1, r4, r2, r3):
        Tf=0.5
        if self.t<Tf:
            phase_1_swing=self.swingCurveGenerate(self.t,Tf,x_target,z_target,0,0,0)
            phase_1_support=self.supportCurveGenerate(0.5+self.t,Tf,x_target,0.5,0)
            #TROT
            self.leg_1_x = phase_1_swing[0]*r1 ; self.leg_2_x = phase_1_support[0]*r2 ; self.leg_3_x = phase_1_swing[0]*r3 ; self.leg_4_x = phase_1_support[0]*r4
            self.leg_1_y = 80-phase_1_swing[1]    ; self.leg_2_y = 80-phase_1_support[1]    ; self.leg_3_y = 80-phase_1_swing[1]    ; self.leg_4_y = 80-phase_1_support[1]
            
        if self.t>=Tf:
            phase_2_swing=self.swingCurveGenerate(self.t-0.5,Tf,x_target,z_target,0,0,0)
            phase_2_support=self.supportCurveGenerate(self.t,Tf,x_target,0.5,0)
            #TROT
            self.leg_1_x = phase_2_support[0]*r1 ; self.leg_2_x = phase_2_swing[0]*r2 ; self.leg_3_x = phase_2_support[0]*r3 ; self.leg_4_x = phase_2_swing[0]*r4
            self.leg_1_y = 80-phase_2_support[1]    ; self.leg_2_y = 80-phase_2_swing[1]    ; self.leg_3_y = 80-phase_2_support[1]    ; self.leg_4_y = 80-phase_2_swing[1]

        return self.leg_1_x, self.leg_2_x, self.leg_3_x, self.leg_4_x, self.leg_1_y, self.leg_2_y, self.leg_3_y, self.leg_4_y
    
    def supportCurveGenerate(self,t,Tf,x_past,t_past,zf):
        # 当前时间；支撑相占空比；摆动相 x 最终位;t最终时间，支撑相 zf
        # Only X Generator
        average=x_past/(1-Tf)
        xf=x_past-average*(t-t_past)
        return xf,zf

    def swingCurveGenerate(self,t,Tf,xt,zh,x0,z0,xv0):
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
    
    def runTrot(self, left_leg_move, right_leg_move, leg_height):
        # act_tran_mov_kp=tran_mov_kp

        if self.t >= 1:#一个完整的运动周期结束 trot
            self.t = 0
        elif right_leg_move == 0 and right_leg_move == 0:
            self.t = 0
        else:
            self.t += self.dt

        trot = self.gaitTrot(self.speed*25, leg_height, left_leg_move, left_leg_move, right_leg_move, right_leg_move)
        print(trot)

        upper_leg_1, upper_leg_2, upper_leg_3, upper_leg_4, lower_leg_1, lower_leg_2, lower_leg_3, lower_leg_4 = self.legInverseKinematics()

        return upper_leg_1, upper_leg_2, upper_leg_3, upper_leg_4, lower_leg_1, lower_leg_2, lower_leg_3, lower_leg_4

    def setSpeed(self, speed):
        self.speed = speed

    def getSpeed(self):
        return self.speed