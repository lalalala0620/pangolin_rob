import sys
import threading
from PWMServoControl import *
import time
import PuppyPI_GAITandIK
import numpy as np
# import ActionGroupControl as AGC

servo = PWMServo()

servo.setThreshold(2,1000,2300)
servo.setThreshold(6,1000,2300)

servo.setThreshold(4,1000,2300)
servo.setThreshold(8,1000,2300)


def getServoPulse(id):
    return 0#getBusServoPulse(id)

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

def allServoRelease():
    servo.unload(1)
    servo.unload(2)
    servo.unload(3)
    servo.unload(4)
    servo.unload(5)
    servo.unload(6)
    servo.unload(7)
    servo.unload(8)


puppy = PuppyPI_GAITandIK.Puppy_Control()
def puppyMove(linear, angular, leg_height):
    if linear > angular:
        left_leg_move  = np.sign(linear)
        right_leg_move = np.sign(linear)
    else:
        left_leg_move  = np.sign(angular)
        right_leg_move = np.sign(angular)        

    upper_leg_1, upper_leg_2, upper_leg_3, upper_leg_4, lower_leg_1, lower_leg_2, lower_leg_3, lower_leg_4 = puppy.runTrot(left_leg_move, right_leg_move, leg_height)
    speed = puppy.getSpeed()
    print(speed)
    spt = 0.05
    setServoPulse(3, int(upper_leg_1*2000/180+500), spt*1000)
    setServoPulse(4, int(lower_leg_1*2000/180+500), spt*1000)
    setServoPulse(1, int(upper_leg_2*2000/180+500), spt*1000)
    setServoPulse(2, int(lower_leg_2*2000/180+500), spt*1000)
    setServoPulse(5, int(upper_leg_3*2000/180+500), spt*1000)
    setServoPulse(6, int(lower_leg_3*2000/180+500), spt*1000)
    setServoPulse(7, int(upper_leg_4*2000/180+500), spt*1000)
    setServoPulse(8, int(lower_leg_4*2000/180+500), spt*1000)
    time.sleep(spt)
    # print(leg_angle)
    # time.sleep(2)
# def runActionGroup(num):
#     threading.Thread(target=AGC.runAction, args=(num, )).start()    

# def stopActionGroup():    
#     AGC.stop_action_group()  

if __name__ == "__main__":
    i = 0
    puppy.setSpeed(1)
    while i < 100:
        puppyMove(1, 0, 10)
        i+=1
    allServoRelease()
    # setServoPulse(1, 1500, 2000)
    # setServoPulse(2, 1500, 2000)
    # setServoPulse(3, 1500, 2000)
    # setServoPulse(4, 1100, 2000)
    # setServoPulse(5, 1500, 2000)
    # setServoPulse(6, 1500, 2000)
    # setServoPulse(7, 1500, 2000)    
    # setServoPulse(8, 1500, 2000)
    # time.sleep(2)
    # setServoPulse(1, 500, 2000)
    # time.sleep(3)
    # servo.setPulse(6, 0, 0)
    # time.sleep(1)
    # unloadServo(5)
    # unloadServo(6)
    # allServoRelease()
    # unloadServo(5)
    # pass