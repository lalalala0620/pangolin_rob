#include"Socket_manager.h"



int Socket_manager::createServerSocket() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error: Could not create socket." << std::endl;
        exit(1);
    }
    return serverSocket;
}

void Socket_manager::configureServerAddress(struct sockaddr_in& serverAddress) {
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8000); 
    serverAddress.sin_addr.s_addr = INADDR_ANY; 
}

int Socket_manager::bindServerSocket(int serverSocket, struct sockaddr_in& serverAddress) {
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error: Could not bind to port." << std::endl;
        close(serverSocket);
        exit(1);
    }
    return serverSocket;
}

int Socket_manager::listenForConnection(int serverSocket) {
    if (listen(serverSocket, 1) == -1) {
        std::cerr << "Error: Could not listen to socket." << std::endl;
        close(serverSocket);
        exit(1);
    }
    return serverSocket;
}

int Socket_manager::acceptClientConnection(int serverSocket, struct sockaddr_in& clientAddress, socklen_t& clientAddressLength) {
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
    if (clientSocket == -1) {
        std::cerr << "Error: Could not accept client connection." << std::endl;
        close(serverSocket);
        exit(1);
    }
    return clientSocket;
}

void Socket_manager::receiveAndProcessJSONData(int clientSocket, char buffer[], size_t bufferSize , double& yAxis, double& zAxis) {
    int bytesRead = recv(clientSocket, buffer, bufferSize, 0);
    if (bytesRead == -1) {
        std::cerr << "Error receiving data." << std::endl;
    } else if (bytesRead == 0) {
        std::cerr << "Connection closed by the remote host." << std::endl;
    } else {
        try {
            json jsonData = json::parse(buffer);
            yAxis = jsonData["yAxis"];
            zAxis = jsonData["zAxis"];
            // std::cout << "Received JSON Data:" << jsonData << std::endl;
        } catch (json::parse_error& e) {
            // std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        }
    }
    return;
}
int Socket_manager::socket_init(){
    //socket init
    int serverSocket = createServerSocket();

    struct sockaddr_in serverAddress;
    configureServerAddress(serverAddress);

    serverSocket = bindServerSocket(serverSocket, serverAddress);
    serverSocket = listenForConnection(serverSocket);

    std::cout << "Server listening on port 8000..." << std::endl;

    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = acceptClientConnection(serverSocket, clientAddress, clientAddressLength);

    std::cout << "Client connected." << std::endl;

    float receivedYAxis, receivedZAxis;
    char buffer[4096];
    size_t bufferSize = sizeof(buffer); 

    return clientSocket;
}
