#include "./includes/client.hpp" 

int main(int ac, char **av) {
    if (ac != 5) {
        std::cerr << "usage: ./client <UserName> <PassWord> <IPaddress> <ServerPort>" << std::endl;
        exit(1);
    }   
    Client      client(av[3], av[4], av[1], av[2]);
    try {
        client.confingClient();
        client.connectToServer();
        // client.authenticateToTheServer();
        client.sendMessageToServer();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(errno);
    }
    std::cout << "[ Closing the client application. ]" << std::endl; 
}