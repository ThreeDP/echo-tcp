#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

# include <sys/socket.h>
# include <sys/types.h>
# include <cstring>
# include <string>
# include <arpa/inet.h>
# include <unistd.h>
# include <stdlib.h>
# include <errno.h>
# include <iostream>
# include <netdb.h>

#include <stdio.h>

# define MAX_LINE 4096

typedef struct sockaddr_in sock_address;
typedef struct addrinfo addrinfo;

class Client {
    int             _sockFd;
    std::string     _serverHost;
    std::string     _serverPort;
    addrinfo        _clientAddr;
    addrinfo        *_sockAddr;
    std::string     _username;
    std::string     _password;
    Client(void){};

    public:
        Client(std::string sh, std::string sp, std::string user, std::string pass);
        ~Client(void);

        void    confingClient(void);
        void    authenticateToTheServer(void);
        void    sendMessageToServer(void);
        void    connectToServer(void);
};

void    err_quit(std::string str);

#endif