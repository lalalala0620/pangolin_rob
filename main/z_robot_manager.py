import argparse
import curses
import zenoh
import json
from dataclasses import dataclass
from datetime import datetime
from pycdr2 import IdlStruct
from pycdr2.types import int8, int32, uint32, uint8, float64, float32, sequence, array
from typing import List
import threading
import traceback
import time
from multi_robot_datatype import BatteryState, Joy, OverViewState, JointStates, UWBState, Vector3, Twist, LaserScan, DriveStatus
import socket
import numpy as np
import pickle
from queue import Queue
from zenoh import Reliability, Sample

import cmath

class RobotPickle():
    def __init__(self, id ,linear, angular, joy_brake):
        self.ID = id
        self.Linear = linear
        self.Angular = angular
        self.Joy_brake = joy_brake

class SocketClient():

    def __init__(self, addr, port):

        self.robot_manager = RobotStatusManager(robot_num=1)
        # self.robot_manager.activeStatusManager()

        self.server_address = addr
        self.server_port = port
        self.connection_success = False
        self.DEFAULT_LEN = 200
        self.force_quit = False
        self.process_thread = threading.Thread()

        self.server_cmd = None
        self.connectToServer()
    
    def connectToServer(self):
        if not self.connection_success:
            try:
                self.unity_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.unity_socket.connect((self.server_address, self.server_port))
                print("Successful Connect to Unity Server")
                self.connection_success = True
                self.process_thread = threading.Thread(target=self.processData, daemon=True)
                self.process_thread.start()
            except ConnectionRefusedError:
                self.connection_success = False
    
    def processData(self):
        while not self.force_quit:
            msg_data = self.unity_socket.recv(2048)
            if len(msg_data) < self.DEFAULT_LEN:
                self.decodeData(msg_data)
                
                self.robot_manager.sendRobotCMD(self.server_cmd)
    
    def decodeData(self, msg_data):
        server_data = pickle.loads(msg_data)
        print("ID {} L_cmd {} A_cmd {} JOY {} ".format(server_data.ID, server_data.Linear, server_data.Angular, server_data.Joy_brake))
        self.server_cmd = server_data

    def getCMDInfo(self):
        return self.server_cmd
    
    def closeSocketServer(self):
        self.force_quit = True
        self.unity_socket.close()
        self.robot_manager.closeStatusManager()

class PangolinState():

    def __init__(self, id, zenoh_config):
        
        self.id = str(id)
        self.input_prefix = 'pangolin' + self.id

        self.battery_state = None
        self.joint_state = None
        self.battery_state_sub_ = None
        self.joint_state_sub_ = None

        self.zenoh_session = zenoh.open(zenoh_config)
        # self.createTopicSub()
    
    def pubTwistCMD(self, linear_cmd, angular_cmd):
        cmd_topic = self.input_prefix + '/cmd_vel'
        # print(cmd_topic)
        cmd = Twist(linear= Vector3(x=linear_cmd, y=0.0, z=0.0),
                    angular=Vector3(x=0.0, y=0.0, z=angular_cmd))
        self.zenoh_session.put(cmd_topic, cmd.serialize())
        
    def closeConnect(self):
        if self.battery_state_sub_ is not None:
            self.battery_state_sub_.undeclare()
        if self.joint_state_sub_ is not None:
            self.joint_state_sub_.undeclare()


class TurtlebotState():

    def __init__(self, id, zenoh_config):
        
        self.id = str(id)
        self.input_prefix = 'turtlebot' + self.id + '/rt'

        self.battery_state = None
        self.joint_state = None
        self.battery_state_sub_ = None
        self.joint_state_sub_ = None

        self.zenoh_session = zenoh.open(zenoh_config)
        # self.createTopicSub()

    def createTopicSub(self):
        self.battery_state_sub_ = self.zenoh_session.declare_subscriber('{}/battery_state'.format(self.input_prefix), self.batteryStateListener)
        self.joint_state_sub_ = self.zenoh_session.declare_subscriber('{}/joint_states'.format(self.input_prefix), self.jointStateListener)
    
    def pubTwistCMD(self, linear_cmd, angular_cmd):
        cmd_topic = self.input_prefix + '/cmd_vel'
        # print(cmd_topic)
        cmd = Twist(linear= Vector3(x=linear_cmd / 5, y=0.0, z=0.0),
                    angular=Vector3(x=0.0, y=0.0, z=angular_cmd / 2))
        self.zenoh_session.put(cmd_topic, cmd.serialize())

    def batteryStateListener(self, sample):
        self.battery_state = BatteryState.deserialize(sample.payload)
        # print('[ voltage: {}, capacity: {}, percentage: {}]'.format(self.battery_state.voltage,
        #                                 self.battery_state.capacity, self.battery_state.percentage))
    
    def jointStateListener(self, sample):
        self.joint_state = JointStates.deserialize(sample.payload)
        # print('[name: {}, velocity: {}]'.format(self.joint_state.name, self.joint_state.velocity))

    def getJointState(self):
        if self.joint_state is not None:
            return self.joint_state
    
    def getBatteryState(self):
        if self.battery_state is not None:
            return self.battery_state
        
    def closeConnect(self):
        if self.battery_state_sub_ is not None:
            self.battery_state_sub_.undeclare()
        if self.joint_state_sub_ is not None:
            self.joint_state_sub_.undeclare()


class EvpiState():

    def __init__(self, id, zenoh_config):
        
        self.id = str(id)
        self.input_prefix = 'evpi' + self.id + '/rt'

        self.joy = None
        self.joy_sub = None
        self.last_cmd = Twist(linear= Vector3(x=(0.0*2 -1), y=0.0, z=0.0),
                    angular=Vector3(x=0.0, y=0.0, z=0.0 / 2))

        self.last_drive_cmd = DriveStatus(drive_status= 0, message="")
        self.zenoh_session = zenoh.open(zenoh_config)
        # self.createTopicSub()

    # def createTopicSub(self):
    #     self.joy_sub = self.zenoh_session.declare_subscriber('{}/joy'.format(self.input_prefix), self.joyListener)

    def pubTwistCMD(self, linear_cmd, angular_cmd):
        cmd_topic = self.input_prefix + '/cmd_vel'
        # print(linear_cmd*2 -1)
        
        cmd = Twist(linear= Vector3(x=(linear_cmd*2 -1), y=0.0, z=0.0),
                    angular=Vector3(x=0.0, y=0.0, z=angular_cmd / 2))
        if self.last_cmd != cmd:
            # print(self.getJoy())
            print("12345678")
            self.zenoh_session.put(cmd_topic, cmd.serialize())
        self.last_cmd = cmd 
        
    def rqDriveStatus(self, status):
        drive_status_topic = 'evpi' + self.id + '/rq' +'/driver/drive_statusRequest'
        cmd = DriveStatus(drive_status=status, message="")
        if self.last_drive_cmd != cmd:
            print(drive_status_topic, cmd)
            self.zenoh_session.put(drive_status_topic, cmd.serialize())
        
        

    # def joyListener(self, sample):
    #     self.joy = Joy.deserialize(sample.payload)
    #     print('[name: {}, velocity: {}]'.format(self.joy.axes, self.joy.buttons))

    # def getJoy(self):
    #     if self.joy is not None:
    #         return self.joy
        
    def closeConnect(self):
        if self.battery_state_sub_ is not None:
            self.battery_state_sub_.undeclare()
        if self.joint_state_sub_ is not None:
            self.joint_state_sub_.undeclare()

            



class RobotStatusManager():
    
    def __init__(self, robot_num = 1):  

        """ private definition """
        self.zenoh_config_ = None
        self.zenoh_session_ = None
        self.battery_state_sub_ = None
        self.joint_state_sub_ = None
        self.update_thread_enable_ = False

        self.robot_id = None

        self.turtlebot_list = list()
        self.pangolin_list = list()
        self.evpi_list = list()
        self.MAX_ROBOT_NUM = robot_num

        # self.input_prefix_ = 'rt' #default setting

        self.output_prefix_ = 'turtlebot'

        """ public definition """

        self.zenohInit()
        


    def setArgParser(self) -> argparse.ArgumentParser:
        arg_parser = argparse.ArgumentParser(prog='robot-manager',
                                                description='zenoh robot manager')
        arg_parser.add_argument('-e', '--connect', type=str, metavar='ENDPOINT', action='append')
        arg_parser.add_argument('-l', '--listen', type=str, metavar='ENDPOINT', action='append')
        arg_parser.add_argument('-d', '--delay', type=float, default=0.05, help='delay between each frame in seconds')
        arg_parser.add_argument('-m', '--mode', type=str, default='client')
        arg_parser.add_argument('-c', '--config', type=str, metavar='FILE')

        return arg_parser
        
        
    
    def zenohInit(self):
        zenoh_arg = self.setArgParser().parse_args()
        self.zenoh_config_ = zenoh.config_from_file(zenoh_arg.config) if zenoh_arg.config is not None else zenoh.Config()
        if zenoh_arg.mode is not None:
            self.zenoh_config_.insert_json5(zenoh.config.MODE_KEY, json.dumps(zenoh_arg.mode))
        if zenoh_arg.connect is not None:
            self.zenoh_config_.insert_json5(zenoh.config.CONNECT_KEY, json.dumps(zenoh_arg.connect))
        if zenoh_arg.listen is not None:
            self.zenoh_config_.insert_json5(zenoh.config.LISTEN_KEY, json.dumps(zenoh_arg.listen))

        self.zenoh_session_ = zenoh.open(self.zenoh_config_)

        self.initRobotCommunication()
        
    def initRobotCommunication(self):
        for id in range(self.MAX_ROBOT_NUM):
            # turlebot_state = TurtlebotState(id, zenoh_config= self.zenoh_config_)
            # self.turtlebot_list.append(turlebot_state)
            # pangolin_state = PangolinState(id, zenoh_config= self.zenoh_config_)
            # self.pangolin_list.append(pangolin_state)
            evpi_state = EvpiState(id, zenoh_config= self.zenoh_config_)
            self.evpi_list.append(evpi_state)
        print("Init Robot State Successful")
    
    def sendRobotCMD(self, cmd):
        if cmd is not None:
            # print("cmd                      ID {} linear {} angular {} joy {}".format(cmd.ID, cmd.Linear, cmd.Angular, cmd.Joy_brake))
            if cmd.ID < self.MAX_ROBOT_NUM:
                # self.pangolin_list[cmd.ID].pubTwistCMD(cmd.Linear, cmd.Angular)
                # self.turtlebot_list[cmd.ID].pubTwistCMD(cmd.Linear, cmd.Angular)
                self.evpi_list[cmd.ID].pubTwistCMD(cmd.Angular, cmd.Linear)
                
                if cmd.Joy_brake == True:
                    print(cmd.Joy_brake)
                    self.evpi_list[cmd.ID].rqDriveStatus(1)
                else:
                    self.evpi_list[cmd.ID].rqDriveStatus(2)
                # self.evpi_list[cmd.ID].pubJoyBrake(cmd.Joy_brake)
        else:
            for id in range(self.MAX_ROBOT_NUM):
                # self.pangolin_list[id].pubTwistCMD(0, 0)
                # self.turtlebot_list[id].pubTwistCMD(0, 0)
                self.evpi_list[id].pubTwistCMD(0, 0)


    def closeStatusManager(self):
        if self.update_thread_enable_:
            self.update_thread_enable_ = False
        if self.is_uwb_master:
            self.uwb_system.closeSystem()
        self.zenoh_session_.close()


if __name__ == "__main__":

    server_ip = "192.168.100.178"
    server_port = 8000
    manager = SocketClient(server_ip, server_port)

    while True:
        try:
            cmd = input("CMD: ")
            if cmd == "q":
                break
        except Exception as e:
            traceback.print_exc()
            break
    manager.closeSocketServer()
    manager.closeStatusManager()