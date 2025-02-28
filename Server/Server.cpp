#include "Server.hpp"
#include <iostream>

Server::Server(const int serverPort) : serverPort(serverPort), serverSocket(INVALID_SOCKET), isRunning(false), connectedClients(0) {
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        throw std::runtime_error("WSAStartup failed");
    }
}

Server::~Server() {
    stopServer();
    WSACleanup();
}

Server& Server::getInstance(const int serverPort) {
    static Server instance(serverPort);
    return instance;
}

const int Server::getServerPort() const {
    return (serverSocket == INVALID_SOCKET) ? -1 : serverPort;
}

bool Server::isServerRunning() const {
    return isRunning;
}

const int Server::getConnectedClients() const {
    return connectedClients;
}

void Server::startServer() {
    if (isRunning) {
        std::cerr << "Server is already running." << std::endl;
        return;
    }

    if (!createSocket() || !bindSocket() || !listenConnection()) {
        stopServer();
        return;
    }

    isRunning = true;
    std::thread([this]() {
        this->acceptConnection();
        }).detach();
}

void Server::stopServer() {
    if (!isRunning) {
        return;
    }

    isRunning = false;
    closesocket(serverSocket);
    serverSocket = INVALID_SOCKET;

    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    threads.clear();

    std::cout << "Server stopped." << std::endl;
}

const int Server::printUsage() {
    int param;
    std::cout << "Usage:" << std::endl;
    std::cout << "     1. Start the server" << std::endl;
    std::cout << "     2. Stop the server" << std::endl;
    std::cout << "     3. Check connected clients" << std::endl;
    std::cout << "     4. Close the app" << std::endl;

    std::cout << "Choice: ";
    std::cin >> param;

    return param;
}

bool Server::createSocket() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        return false;
    }

    std::cout << "Socket created successfully." << std::endl;
    return true;
}

bool Server::bindSocket() {
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(serverPort);

    if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        return false;
    }

    std::cout << "Socket successfully bound to port " << serverPort << "." << std::endl;
    return true;
}

bool Server::listenConnection() {
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        return false;
    }

    std::cout << "Server is listening on port " << serverPort << "." << std::endl;
    return true;
}

void Server::acceptConnection() {
    while (isRunning) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            if (isRunning) {
                std::cerr << "Accept failed" << std::endl;
            }
            continue;
        }

        std::lock_guard<std::mutex> lock(mtx);
        threads.emplace_back(&Server::handleClient, this, clientSocket);
    }
}

void Server::handleClient(SOCKET clientSocket) {
    connectedClients++;
    char buffer[1024];
    int bytesReceived;

    while (isRunning) {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            break;
        }

        std::string receivedData(buffer, bytesReceived);
        if (receivedData.empty()) {
            std::cerr << "Received empty string from client." << std::endl;
            const char* response = "Error: Empty string received.";
            send(clientSocket, response, strlen(response), 0);
            continue;
        }

        std::string modifiedData = doubleConsonants(receivedData);
        send(clientSocket, modifiedData.c_str(), modifiedData.size(), 0);
    }

    closesocket(clientSocket);

    connectedClients--;
}

std::string Server::doubleConsonants(const std::string& inputStr) const {
    if (inputStr.empty()) {
        std::cout << "Working" << std::endl;
        return "";
    }

    std::string result;
    result.reserve(inputStr.size() * 2);

    const char* vowels = "aeiouAEIOU";

    for (char letter : inputStr) {
        result += letter;
        if (isalpha(letter) && !strchr(vowels, letter)) {
            result += letter;
        }
    }

    return result;
}