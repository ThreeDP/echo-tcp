#ifndef __HANDLE_NET_HPP__
# define __HANDLE_NET_HPP__

# include <sys/socket.h>
# include <sys/types.h>
# include <cstring>
# include <arpa/inet.h>
# include <unistd.h>
# include <stdlib.h>
# include <errno.h>
# include <iostream>

# define MAX_LINE 4096

typedef struct sockaddr_in sock_address;

class Server {
    int                 _listenFD;
    int                 _connectFD;
    const unsigned int  _backlog;
    sock_address        _clientAddr;
    sock_address        _serverAddr;
    pid_t               _childPid;
    socklen_t           _childLen;

    uint32_t            _serverHost;
    uint32_t            _serverPort;
    Server(void) : _backlog(8) {};

    public:
        Server(std::string sh, std::string sp);
        ~Server(void);

        void            configServer(void);
        void            putServerToListen(void);
        void            handleServerCalls(void);
};

void    err_sys(std::string str);
#endif