import threading
import socket
import json
import time
import pickle
from queue import Queue

class RobotPickle():
    def __init__(self, id, linear_cmd, angular_cmd, light):
        self.ID = id
        self.Linear = linear_cmd
        self.Angular = angular_cmd
        self.Light = light

class RobotInfo():
    def __init__(self):
        self.controller = None
        self.linear_cmd = 0
        self.angular_cmd = 0
    
    def setMovingInfo(self, linear_cmd, angular_cmd):
        self.linear_cmd = linear_cmd
        self.angular_cmd = angular_cmd

class UnitySocketClient():
    def __init__(self, address, port, robot_num, robot_manager_ip):
        self.robot_num = robot_num
        self.robot_list = []

        for _ in range(robot_num):
            robot = RobotInfo()
            self.robot_list.append(robot)

        self.incoming_unity_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.incoming_unity_socket.bind((address, port))
        self.incoming_unity_socket.listen(5)
        self.robot_manager_ip = robot_manager_ip

        self.dt = 0
    
    def handleClient(self, client_socket, addr):
        controller_robot = -1
        while True:
            data = client_socket.recv(2048)
            if data:
                try:
                    # now = time.time()
                    # print("t: {}".format(now - self.dt))
                    # self.dt = now
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
                    # elif robot_info["ID"] != -1 and self.robot_list[robot_info["ID"]].controller != None and self.robot_list[robot_info["ID"]].controller != addr:
                    #     print("robot" + str(robot_info["ID"]) + "was controled")
                    elif robot_info["ID"] == -1:
                        self.robot_list[controller_robot].controller = None
                        controller_robot = -1
                    for robot_index in range(self.robot_num):
                        print("id : ", robot_index, "controller", self.robot_list[robot_index].controller)
                    # print(self.robot_list)
                    unity_queue.put(self.robot_list)
                
                except Exception as e:
                    print(e)
            else:
                break
        client_socket.close()
        self.robot_list[controller_robot].controller = None

    def scanForUnityClientConnection(self):
        try:
            unity_client_conn, unity_client_addr = self.incoming_unity_socket.accept()
            print("Connect to Unity Client Success, addr: {}".format(unity_client_addr))
            client_thread = threading.Thread(target=self.handleClient, args=(unity_client_conn, 
                                            unity_client_addr))
            client_thread.start()
    
        except socket.error:
            print(socket.error)

class LightSocketClient():
    def __init__(self, address, port):

        self.incoming_light_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.incoming_light_socket.bind((address, port))
        self.incoming_light_socket.listen(5)
        self.light_data = 0

        self.dt = 0
    
    def handleClient(self, client_socket, addr):
        while True:
            data = client_socket.recv(2048)
            if data:
                try:
                    decode_data = data.decode() 
                    self.light_data = json.loads(decode_data[:decode_data.index("}") + 1])
                    light_queue.put(self.light_data)
                except Exception as e:
                    print(e)
            else:
                break
        client_socket.close()

    def scanForLightSensorConnection(self):
        try:
            light_client_conn, light_client_addr = self.incoming_light_socket.accept()
            print("Connect to Light Client Success, addr: {}".format(light_client_addr))
            client_thread = threading.Thread(target=self.handleClient, args=(light_client_conn, 
                                            light_client_addr))
            client_thread.start()
    
        except socket.error:
            print(socket.error)

class RobotManagerClient():
    def __init__(self, address, port, robot_num, robot_manager_ip, unity_queue, light_queue):
        self.robot_num = robot_num
        self.robot_list = []

        for _ in range(robot_num):
            robot = RobotInfo()
            self.robot_list.append(robot)

        self.incoming_robot_manager_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.incoming_robot_manager_socket.bind((address, port))
        self.incoming_robot_manager_socket.listen(5)


        self.robot_manager_ip = robot_manager_ip
        self.unity_queue = unity_queue
        self.light_queue = light_queue

        self.dt = 0
    def sendAllRobotInfo(self, robot_manager_socket):
        while 1:
            unity_queue_data = self.getUnityData()
            light_queue_data = self.getLightData()
            print(unity_queue_data, light_queue_data)
            for robot_index in range(self.robot_num):
                if self.robot_list[robot_index].controller != None:
                    robot_info = RobotPickle(robot_index, 
                                            self.robot_list[robot_index].linear_cmd,
                                            self.robot_list[robot_index].angular_cmd,
                                            )
                elif self.robot_list[robot_index].controller == None:
                    robot_info = RobotPickle(robot_index, 0.0, 0.0)
                robot_info_pickle = pickle.dumps(robot_info)
                robot_manager_socket.send(robot_info_pickle)
                time.sleep(0.06)
            
        robot_manager_socket.close()
    
    def getUnityData(self):
        if self.unity_queue != None:
            unity_queue_data = self.unity_queue.get()
            self.unity_queue.queue.clear()
            print(unity_queue_data)
        return unity_queue_data

    def getLightData(self):
        if self.light_queue != None:
            light_queue_data = self.light_queue.get()
            self.light_queue.queue.clear()
            print(light_queue_data)
        return light_queue_data


if __name__ == "__main__":

    unity_queue = Queue()
    light_queue = Queue()
    server_ip = '192.168.100.178'
    robot_manager_ip = '192.168.100.9' 
    port = 8000
    demo_server = SendToRobotManager(server_ip, port, 5, robot_manager_ip, unity_queue, light_queue)
    # demo_server = UnitySocketReceiver(server_ip, port, 5, robot_manager_ip)
    demo_server.sendAllRobotInfo()

        