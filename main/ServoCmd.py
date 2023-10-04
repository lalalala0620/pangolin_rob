import sys
import threading
from PWMServoControl import *
from time import sleep
from multi_robot_datatype import BatteryState, OverViewState, JointStates, UWBState, Vector3, Twist
# import ActionGroupControl as AGC

servo = PWMServo()

servo.setThreshold(9,500,2500)
servo.setThreshold(10,500,2500)
servo.setThreshold(6,500,2500)
servo.setThreshold(8,500,2500)

def getServoPulse(id):
    return servo.servo_pwm_duty_now[id]

def getServoDeviation(id):
    return servo.getDeviation(id)

def setServoPulse(id, pulse, use_time):
    servo.setPulse(id, pulse, use_time)

def setServoDeviation(id ,dev):
    servo.setDeviation(id, dev)
    
def saveServoDeviation(id):
    servo.saveDeviation(id)

def unloadServo(id):
    servo.unload(id)

def updatePulse(id):
    servo.updatePulse(id)

def forward(servo_angle=[40, 40, -40, -40] , sleepTime=1.0, servo_rate=[1.0, 1.0]):
    deviation = [-100 ,0, 40, 120]
    setServoPulse(9, servo_rate[0]*servo_angle[0]*6.25+1500 + deviation[2], 80)
    setServoPulse(10, servo_rate[1]*servo_angle[1]*6.25+1500 + deviation[3], 80)
    setServoPulse(6, servo_rate[0]*servo_angle[2]*6.25+1500 + deviation[0], 80)
    setServoPulse(8, servo_rate[1]*servo_angle[3]*6.25+1500 + deviation[1], 80)
    sleep(sleepTime)

def move(linear, angular):
    linear_x, _, _ = linear.x, linear.y, linear.z
    _, _, angular_z = angular.x, angular.y, angular.z
        
    forward(servo_angle = [20, 0, 0, -20], sleepTime=0.08, servo_rate=[linear_x-angular_z, linear_x+angular_z])

    forward(servo_angle = [-38, 0, 0, 38], sleepTime=0.08, servo_rate=[linear_x-angular_z, linear_x+angular_z])

    forward(servo_angle = [-40, -40, 40, 40], sleepTime=0.08, servo_rate=[linear_x-angular_z, linear_x+angular_z])

    forward(servo_angle = [0, -20, 20, 0], sleepTime=0.08, servo_rate=[linear_x-angular_z, linear_x+angular_z])

    forward(servo_angle = [0, 38, -38, 0], sleepTime=0.08, servo_rate=[linear_x-angular_z, linear_x+angular_z])

    forward(servo_angle = [40, 40, -40, -40], sleepTime=0.08, servo_rate=[linear_x-angular_z, linear_x+angular_z])

def move_joystick(linear_x, angular_z):
    # linear_x, _, _ = linear.x, linear.y, linear.z
    # _, _, angular_z = angular.x, angular.y, angular.z
        
    forward(servo_angle = [20, 0, 0, -20], sleepTime=0.08, servo_rate=[linear_x-angular_z, linear_x+angular_z])

    forward(servo_angle = [-38, 0, 0, 38], sleepTime=0.08, servo_rate=[linear_x-angular_z, linear_x+angular_z])

    forward(servo_angle = [-40, -40, 40, 40], sleepTime=0.08, servo_rate=[linear_x-angular_z, linear_x+angular_z])

    forward(servo_angle = [0, -20, 20, 0], sleepTime=0.08, servo_rate=[linear_x-angular_z, linear_x+angular_z])

    forward(servo_angle = [0, 38, -38, 0], sleepTime=0.08, servo_rate=[linear_x-angular_z, linear_x+angular_z])

    forward(servo_angle = [40, 40, -40, -40], sleepTime=0.08, servo_rate=[linear_x-angular_z, linear_x+angular_z])

def sitDown():
    deviation = [0 ,-100, 100, 100]
    setServoPulse(9, 1500 + deviation[2], 1000)
    setServoPulse(10, 1500 + deviation[3], 1000)
    setServoPulse(6, 1500 + deviation[0], 1000)
    setServoPulse(8, 1500 + deviation[1], 1000)
    sleep(1)

    setServoPulse(9, 700 + deviation[2], 300)
    setServoPulse(10, 2300 + deviation[3], 300)
    setServoPulse(6, 500 + deviation[0], 300)
    setServoPulse(8, 2500 + deviation[1], 300)
    sleep(0.3)

    setServoPulse(9, 1000 + deviation[2], 150)
    setServoPulse(10, 2000 + deviation[3], 150)
    setServoPulse(6, 2000 + deviation[0], 150)
    setServoPulse(8, 1000 + deviation[1], 150)
    sleep(0.15)

    setServoPulse(9, 1400 + deviation[2], 1000)
    setServoPulse(10, 1600 + deviation[3], 1000)
    setServoPulse(6, 2000 + deviation[0], 1000)
    setServoPulse(8, 1000 + deviation[1], 1000)
    sleep(1)

def standUp():
    deviation = [0 ,-100, 100, 100]
    setServoPulse(9, 1400 + deviation[2], 1000)
    setServoPulse(10, 1600 + deviation[3], 1000)
    setServoPulse(6, 1500 + deviation[0], 1000)
    setServoPulse(8, 1500 + deviation[1], 1000)
    sleep(1)

    setServoPulse(9, 800 + deviation[2], 1000)
    setServoPulse(10, 2200 + deviation[3], 1000)
    setServoPulse(6, 1500 + deviation[0], 1000)
    setServoPulse(8, 1500 + deviation[1], 1000)
    sleep(1)
    setServoPulse(9, 1500 + deviation[2], 1000)
    setServoPulse(10, 1500 + deviation[3], 1000)
    setServoPulse(6, 1500 + deviation[0], 1000)
    setServoPulse(8, 1500 + deviation[1], 1000)
    sleep(1)

def sit_act(pSB_Left_Vertical_Axis, pSB_Right_Vertical_Axis):
    deviation = [0 ,-100, 30, 50]
    setServoPulse(9, 1400 + deviation[2],  150)
    setServoPulse(10, 1600 + deviation[3], 150)
    setServoPulse(6, pSB_Left_Vertical_Axis*800+1500 + deviation[0], 150)
    setServoPulse(8, pSB_Right_Vertical_Axis*800+1500 + deviation[1], 150)
    sleep(0.15)


SERVO_MIDDLE_VALUE = 1500

if __name__ == "__main__":
    pass