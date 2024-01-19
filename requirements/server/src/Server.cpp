#include "./includes/Server.hpp"

Server::Server(std::string sh, std::string sp) : _serverHost(std::stoul(sh)), _serverPort(std::stoul(sp)), _backlog(8) {
    bzero(&this->_serverAddr, sizeof(this->_serverAddr));
}

Server::~Server(void) {
    
}

void    Server::configServer(void) {
    this->_listenFD = socket(AF_INET, SOCK_STREAM, 0);
    this->_serverAddr.sin_family = AF_INET;
    this->_serverAddr.sin_addr.s_addr = htonl(this->_serverHost);
    this->_serverAddr.sin_port = htons(this->_serverPort);
    if (bind(this->_listenFD, (const sockaddr *) &this->_serverAddr, sizeof(this->_serverAddr)) == -1) {
        std::string err = "Error: Binding to the address [ " + std::string(strerror(errno)) + " ].";
        throw(Except(err));
    }
}

void    Server::putServerToListen(void) {
    if (listen(this->_listenFD, this->_backlog) == -1) {
        std::string err = "Error: Unable to listen on the socket [ " + std::string(strerror(errno)) + " ].";
        throw(Except(err));     
    }
    std::cout << "Listen on host: " << this->_serverHost << " | Port: " << this->_serverPort << std::endl;
    while (1) {
        this->_childLen = sizeof(this->_clientAddr);
        this->_connectFD = accept(this->_listenFD, (sockaddr *) &this->_clientAddr, &this->_childLen);
        if (this->_connectFD == -1) {
            std::cout << "Error: Unable to accept incoming connection" << std::endl;
            continue ;
        }
        if ((this->_childPid = fork()) == 0) {
            close(this->_listenFD);
            if (this->_login.loginAuthentication(this->_connectFD, 0) == false)
                exit(1);
            this->handleServerCalls();
            exit(0);
        }
        close(this->_connectFD);
    }
}

void    Server::handleServerCalls(void) {
    ssize_t     n;
    char        buf[MAX_LINE];

    while (1) {
        while ((n = recv(this->_connectFD, buf, MAX_LINE, 0)) > 0) {
            if (send(this->_connectFD, buf, n, 0) == -1) {
                std::cerr << "Error: sending data. " << std::string(strerror(errno)) << " ]." << std::endl;
                exit(1);
            }
        }
        if (n < 0 && errno == EINTR)
            continue ;
        else if (n < 0) {
            std::cerr << "Error: receiving data. " << std::string(strerror(errno)) << " ]." << std::endl;
            exit(1);
        }
        break ;
    }
}