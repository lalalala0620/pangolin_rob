#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <nlohmann/json.hpp>


using json = nlohmann::json;


int main(){
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1){
        std::cerr << "Error: Could not create socket." << std::endl;
        return 1;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8000);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1){
        std::cerr << "Error: Could not bind to port." << std::endl;
        return 1;
    }
        
    if (listen(serverSocket, 1) == -1){
        std::cerr << "Error: Could not listen to socket." << std::endl;
        return 1;
    }
    
    std::cout << "Server listening on port 8000..." << std::endl;

    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
    if (clientSocket == -1){
        std::cerr << "Error: Could not accept client connection." << std::endl;
        return 1;
    }

    std::cout << "Client connect." << std::endl;

    char buffer[4096]; 

    while (true){

    memset(buffer, 0, sizeof(buffer));

    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesRead == -1) {
        std::cerr << "Error receiving data." << std::endl;
        
    } 
    else if (bytesRead == 0) {
        std::cerr << "Connection closed by the remote host." << std::endl;

    } 
    else {

        std::string receivedData(buffer, bytesRead);

        try {
            json jsonData = json::parse(receivedData);
            
            float yAxis = jsonData["yAxis"];
            float zAxis = jsonData["zAxis"];

            std::cout << "Received JSON Data:" << jsonData << std::endl;
            std::cout << "yAxis: " << yAxis << std::endl;
            std::cout << "zAxis: " << zAxis << std::endl;
        } catch (json::parse_error& e) {
            // std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            
        }
    }
    }
    close(clientSocket);
    close(serverSocket);

    return 0;
}