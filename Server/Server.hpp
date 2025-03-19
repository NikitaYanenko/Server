// included guard
#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>

#pragma comment(lib, "ws2_32.lib")

class Server {
private:
    const int serverPort;
    SOCKET serverSocket;
    WSAData wsadata;
    std::vector<std::thread> threads;
    std::mutex mtx;
    std::atomic<bool> isRunning;
    std::atomic<int> connectedClients;

    Server(const int serverPort);

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    bool createSocket();
    bool bindSocket();
    bool listenConnection();
    void acceptConnection();
    void handleClient(SOCKET clientSocket);

    [[nodiscard]] std::string doubleConsonants(const std::string& inputStr) const;

public:
    ~Server();
    static Server& getInstance(const int serverPort);
    
    [[nodiscard]] const int getServerPort() const;

    [[nodiscard]] bool isServerRunning() const;
    
    [[nodiscard]] const int getConnectedClients() const;

    void startServer();
    void stopServer();

    static const int printUsage(); // base menu
};

#endif // SERVER_H