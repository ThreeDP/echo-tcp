#include "./includes/client.hpp" 

int main(int ac, char **av) {
    if (ac != 5)
        err_quit("usage: ./client <UserName> <PassWord> <IPaddress> <ServerPort>");
    Client      client(av[3], av[4], av[1], av[2]);

    client.confingClient();
    client.connectToServer();
    // client.authenticateToTheServer();
    client.sendMessageToServer();
}