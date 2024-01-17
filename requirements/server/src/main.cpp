#include "./includes/Server.hpp"

int main(int ac, char **av) {
    if (ac != 3) {
        std::cerr << "error usage: ./server <Server IP> <Server Port>" << std::endl;
        return (1);
    }
    Server      server(av[1], av[2]);

    server.configServer();
    server.putServerToListen();
}