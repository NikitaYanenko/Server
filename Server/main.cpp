#include "Server.hpp"
#include <iostream>

enum NavigationParam {
    PR_START = 1,
    PR_STOP = 2,
    PR_CLIENT = 3,
    PR_EXIT = 4
};

int main() {
    const int port = 8081;
    int param;

    Server& server = Server::getInstance(port);

    do {
        param = Server::printUsage();

        switch (param) {
        case NavigationParam::PR_START:

            if (server.getServerPort() != -1) {
                std::cout << "Server is already running on port " << server.getServerPort() << ".\n";
                break;
            }

            server.startServer();
            std::cout << "Server started on port " << port << ".\n";
            break;

        case NavigationParam::PR_STOP:
            if (server.getServerPort() == -1) {
                std::cout << "Server is not running.\n";
                break;
            }

            server.stopServer();
            break;

        case NavigationParam::PR_CLIENT:
            std::cout << "Connected clients: " << server.getConnectedClients() << std::endl;
            break;

        case NavigationParam::PR_EXIT:
            std::cout << "Exiting..." << std::endl;
            break;


        default:
            std::cout << "Invalid option. Try again.\n";
            break;
        }

        system("pause");

    } while (param != NavigationParam::PR_EXIT);

    if (server.getServerPort() != -1) {
        std::cout << "Stopping the server before exiting...\n";
        server.stopServer();
    }

    std::cout << "See you soon!" << std::endl;
    return 0;
}
