#include "./includes/client.hpp"

void    runServer(Client client) {
    try {
        client.confingClient();
        client.connectToServer();
        client.sendMessageToServer();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(errno);
    }
}

int main(int ac, char **av) {
    if (ac > 5 || ac < 2) {
        std::cerr << "usage: ./client <IPaddress> <ServerPort> <UserName> <PassWord>" << std::endl;
        exit(1);
    }
    wait_signal();
    if (ac == 3) {
        Client      client(av[1], av[2]);
        runServer(client);
    }
    else if (ac == 4) {
        Client      client(av[1], av[2], av[3]);
        runServer(client);
    }
    else {
        Client      client(av[1], av[2], av[3], av[4]);
        runServer(client);
    }
    std::cout << "[ Closing the client application. ]" << std::endl; 
}