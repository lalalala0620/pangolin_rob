#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    // 创建一个Socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error: Could not create socket." << std::endl;
        return 1;
    }

    // 准备服务器地址结构体
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8000);  // 服务器端口号
    serverAddress.sin_addr.s_addr = inet_addr("192.168.1.137");  // 服务器IP地址

    // 连接到服务器
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error: Could not connect to server." << std::endl;
        return 1;
    }

    std::cout << "Connected to server." << std::endl;

    // 发送数据到服务器
    int message = 100;
    if (send(clientSocket, &message, sizeof(message), 0) == -1) {
        std::cerr << "Error: Could not send data to server." << std::endl;
        return 1;
    }

    // 接收服务器的响应
    int buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        std::cerr << "Error: Could not receive data from server." << std::endl;
        return 1;
    }

    std::cout << "Received from server: " << buffer << std::endl;

    // 关闭Socket连接
    close(clientSocket);

    return 0;
}