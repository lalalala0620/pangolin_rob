import threading
import socket
import json
import time
import pickle
from queue import Queue

class RobotPickle():
    def __init__(self, id, linear_cmd, angular_cmd, joy_brake):
        self.ID = id
        self.Linear = linear_cmd
        self.Angular = angular_cmd
        self.Joy_brake = joy_brake

class RobotInfo():
    def __init__(self):
        self.controller = None
        self.linear_cmd = 0
        self.angular_cmd = 0
        self.joy_brake = False
    
    def setMovingInfo(self, linear_cmd, angular_cmd):
        self.linear_cmd = linear_cmd
        self.angular_cmd = angular_cmd

class LightInfo():
    def __init__(self):
        self.lux = 0

class Server():
    def __init__(self, server_ip, robot_manager_ip, unity_ip, pico_ip,  port, robot_num):

        self.incoming_UDP_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.incoming_UDP_socket.bind((server_ip, port))
        self.incoming_TCP_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.incoming_TCP_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.incoming_TCP_socket.bind((server_ip, port))
        self.incoming_TCP_socket.listen(5)
        self.robot_num = robot_num
        self.unity_ip = unity_ip
        self.pico_ip = pico_ip
        self.robot_manager_ip = robot_manager_ip
        self.unitySocketClient = UnitySocketClient(self.robot_num)
        self.picoSocketClient = PicoSocketClient(self.incoming_UDP_socket)
        # self.robot_list = RobotInfo()
        self.robotManagerClient = RobotManagerClient(robot_num, robot_manager_ip)
        self.createCmdVelThread()

    def scanForClientConnection(self):
        while 1:
            self.picoSocketClient.scanForPicoConnection()
            tcp_client_conn, tcp_client_addr = self.incoming_TCP_socket.accept()
            print(tcp_client_addr[0], self.unity_ip)
            if tcp_client_addr[0] == self.unity_ip:
                print("2")
                self.unitySocketClient.scanForUnityClientConnection(tcp_client_conn, tcp_client_addr)
            elif tcp_client_addr[0] == self.robot_manager_ip:
                print("3")
                self.robotManagerClient.scanForRMConnection(tcp_client_conn, tcp_client_addr) 

    def calculateCmdVel(self):
        while 1:
            unity_data = self.unitySocketClient.getUnityData()
            pico_data = self.picoSocketClient.getPicoData()
            if pico_data >= 1000:
                print("stop!!!")
                for robot_index in range(self.robot_num):
                    unity_data[robot_index].linear_cmd = 1.0
                    unity_data[robot_index].angular_cmd = 1.0
                    unity_data[robot_index].joy_brake = True

            self.robotManagerClient.setRobotManagerData(unity_data)

    def createCmdVelThread(self):
        vel_thread = threading.Thread(target=self.calculateCmdVel, args=())
        vel_thread.start()


class UnitySocketClient():
    def __init__(self, robot_num):
        self.robot_num = robot_num
        self.robot_list = []
        for _ in range(robot_num):
            robot = RobotInfo()
            self.robot_list.append(robot)


        self.robot_manager_ip = robot_manager_ip
    
    def handleClient(self, client_socket, addr):
        controller_robot = -1
        while True:
            data = client_socket.recv(2048)
            if data:
                try:
                    self.robot_list[controller_robot].joy_brake = False

                    decode_data = data.decode() 
                    robot_info = json.loads(decode_data[:decode_data.index("}") + 1])
                    robot_info["Linear"] = int(robot_info["Linear"] * 100) / 100
                    robot_info["Angular"] = int(robot_info["Angular"] * 100) / 100
                    if robot_info["ID"] == controller_robot and controller_robot != -1:
                        self.robot_list[controller_robot].linear_cmd = robot_info["Linear"]
                        self.robot_list[controller_robot].angular_cmd = robot_info["Angular"]
                        
                    elif robot_info["ID"] != -1 and self.robot_list[robot_info["ID"]].controller == None and controller_robot == -1 :
                        controller_robot = robot_info["ID"]
                        self.robot_list[controller_robot].controller = addr
                        self.robot_list[controller_robot].linear_cmd = robot_info["Linear"]
                        self.robot_list[controller_robot].angular_cmd = robot_info["Angular"]
                    elif robot_info["ID"] != -1 and robot_info["ID"] != controller_robot and self.robot_list[robot_info["ID"]].controller == None:
                        self.robot_list[controller_robot].controller = None
                        controller_robot = robot_info["ID"]
                        self.robot_list[controller_robot].controller = addr
                        self.robot_list[controller_robot].linear_cmd = robot_info["Linear"]
                        self.robot_list[controller_robot].angular_cmd = robot_info["Angular"]
                    elif robot_info["ID"] == -1:
                        self.robot_list[controller_robot].controller = None
                        controller_robot = -1
                    # for robot_index in range(self.robot_num):
                    #     print("id : ", robot_index, "controller", self.robot_list[robot_index].controller)
                
                except Exception as e:
                    print(e)
            else:
                break
        client_socket.close()
        self.robot_list[controller_robot].controller = None

    def scanForUnityClientConnection(self, unity_client_conn, unity_client_addr):
        try:
            print("Connect to Unity Client Success, addr: {}".format(unity_client_addr))
            client_thread = threading.Thread(target=self.handleClient, args=(unity_client_conn, 
                                            unity_client_addr))
            client_thread.start()
    
        except socket.error:
            print(socket.error)

    def getUnityData(self):
        return self.robot_list


class PicoSocketClient():
    def __init__(self, incoming_pico_socket):
        self.pico_data = 0
        self.incoming_pico_socket = incoming_pico_socket
    
    def handleClient(self, client_socket):
        while True:
            data = client_socket.recvfrom(2048)
            # print(len(data), data)
            if data:
                try:
                    decode_data = data[0].decode('utf-8') 
                    pico_data = json.loads(decode_data)
                    self.pico_data = pico_data["light"]
                    print(self.pico_data)
                except Exception as e:
                    print(e)
            else: break
        client_socket.close()

    def scanForPicoConnection(self):
        try:
            client_thread = threading.Thread(target=self.handleClient, args=(self.incoming_pico_socket,
                                            ))
            client_thread.start()
    
        except socket.error:
            print(socket.error)

    def getPicoData(self):
        return self.pico_data

class RobotManagerClient():
    def __init__(self, robot_num, robot_manager_ip):
        self.robot_manager_ip = robot_manager_ip
        self.robot_num = robot_num
        self.robot_list = 0

    def scanForRMConnection(self, rm_client_conn, rm_client_addr):
        try:
            print("Connect to RM Success, addr: {}".format(rm_client_addr))
            client_thread = threading.Thread(target=self.sendAllRobotInfo, args=(rm_client_conn, ))
            client_thread.start()
    
        except socket.error:
            print(socket.error)

    def sendAllRobotInfo(self, robot_manager_socket):
        while 1:
            for robot_index in range(self.robot_num):
                if self.robot_list[robot_index].controller != None:
                    robot_info = RobotPickle(robot_index, 
                                            self.robot_list[robot_index].linear_cmd,
                                            self.robot_list[robot_index].angular_cmd,
                                            self.robot_list[robot_index].joy_brake,
                                            )
                elif self.robot_list[robot_index].controller == None:
                    robot_info = RobotPickle(robot_index, 0.0, 0.0, self.robot_list[robot_index].joy_brake)
                print(self.robot_list[robot_index].joy_brake)
                robot_info_pickle = pickle.dumps(robot_info)
                robot_manager_socket.send(robot_info_pickle)
                time.sleep(0.06)
        robot_manager_socket.close()

    def setRobotManagerData(self, robot_list):
        self.robot_list = robot_list






if __name__ == "__main__":

    # unity_queue = Queue()
    # pico_queue = Queue()
    server_ip = '192.168.100.178'
    robot_manager_ip = '192.168.100.9' 
    pico_ip = '192.168.100.10' 
    unity_ip = '192.168.100.186' 
    port = 8000
    robot_num = 1
    demo_server = Server(server_ip, robot_manager_ip, unity_ip, pico_ip, port, robot_num)
    demo_server.scanForClientConnection()