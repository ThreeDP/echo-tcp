#include "./includes/Server.hpp"

int main(int ac, char **av) {
    if (ac != 3) {
        std::cerr << "error usage: ./server <Server IP> <Server Port>" << std::endl;
        return (1);
    }
    wait_signal();
    Server      server(av[1], av[2]);
    try {
        server.configServer();
        server.putServerToListen();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(errno);
    }
    std::cout << "[ Closing the server application. ]" << std::endl;
}