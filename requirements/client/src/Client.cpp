#include "./includes/Client.hpp"

Client::Client(std::string sh, std::string sp, std::string user, std::string pass) \
: _serverHost(sh), _serverPort(sp), _autheticate(LoginRequest(user, pass)) {
    bzero(&this->_clientAddr, sizeof(this->_clientAddr));
    this->_sockFd = 0;
}

Client::~Client(void) {
    close(this->_sockFd);
    free(this->_sockAddr);
}

void    Client::confingClient(void) {
    int         status;

    this->_clientAddr.ai_family = AF_INET;
    this->_clientAddr.ai_socktype = SOCK_STREAM;
     this->_sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_sockFd == -1) {
        std::string err = "Error: creating socket [ " + std::string(strerror(errno)) + " ].";
        throw(Except(err));
    }
    if ((status = getaddrinfo((const char *) this->_serverHost.c_str(), (const char *)this->_serverPort.c_str(), &this->_clientAddr, &this->_sockAddr)) != 0) {
        std::string err = "getaddrinfo error: " + std::string(gai_strerror(status));
        throw(Except(err));
    }
}

void    Client::connectToServer(void) {

    if (connect(this->_sockFd, this->_sockAddr->ai_addr, this->_sockAddr->ai_addrlen) == -1) {
        char    *errno_msg = strerror(errno);
        std::string err = "Error: connecting to the server [ " + std::string(errno_msg) + " ].";
        throw(Except(err));
    }
    if (this->_autheticate.login(this->_sockFd, 8) == false) {
        std::string err = "Error: autheticate.";
        throw(Except(err));
    }
    std::cout << "Connect on host: " << this->_serverHost << " | Port: " << this->_serverPort << std::endl;
}

void    Client::sendMessageToServer(void) {
    char    send_line[MAX_LINE];
    char    recv_line[MAX_LINE];
    ssize_t bytes_sent;
    ssize_t bytes_received;

    while (std::cin.getline(send_line, MAX_LINE)) {
        bytes_sent = 0;
        bytes_received = 0;
        if (std::strcmp(send_line, "") == 0)
            continue ;
        if (std::strcmp(send_line, "exit") == 0)
            break ;
        if ((bytes_sent = send(this->_sockFd, send_line, strlen(send_line), 0)) == -1) {
            std::string err = "Error: sending data. [ " + std::string(strerror(errno)) + " ].";
            throw(Except(err));
        }
        if ((bytes_received = recv(this->_sockFd, recv_line, MAX_LINE, 0)) == -1) {
            std::string err = "Error: receiving data [ " + std::string(strerror(errno)) + " ].";
            throw(Except(err));
        }
        std::cout << recv_line << std::endl;
        bzero(&send_line, sizeof(send_line));
        bzero(&recv_line, sizeof(recv_line));
    }
}
