#include "./includes/Client.hpp"

void    err_quit(std::string str) {
    std::cerr << str << std::endl;
    exit(1);
}

Client::Client(std::string sh, std::string sp, std::string user, std::string pass) \
: _serverHost(sh), _serverPort(sp), _username(user), _password(pass) {
    bzero(&this->_clientAddr, sizeof(this->_clientAddr));
    this->_sockFd = 0;
}

Client::~Client(void) {
    free(this->_sockAddr);
}

void    Client::confingClient(void) {
    int         status;

    this->_clientAddr.ai_family = AF_INET;
    this->_clientAddr.ai_socktype = SOCK_STREAM;
    this->_sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if ((status = getaddrinfo((const char *) this->_serverHost.c_str(), (const char *)this->_serverPort.c_str(), &this->_clientAddr, &this->_sockAddr)) != 0) {
        std::cerr << "getaddrinfo error: %s" << gai_strerror(status) << std::endl;
    }
}

void    Client::connectToServer(void) {
    std::cout << "Host: " << this->_sockAddr->ai_addr << std::endl;
    if (connect(this->_sockFd, this->_sockAddr->ai_addr, this->_sockAddr->ai_addrlen) == -1)
        std::cout << "Connect error" << std::endl;
    std::cout << "Connect on host: " << this->_serverHost << " | Port: " << this->_serverPort << std::endl;
}

void    Client::authenticateToTheServer(void) {

}

void    Client::sendMessageToServer(void) {
    char    send_line[MAX_LINE];
    char    recv_line[MAX_LINE];

    while (std::cin.getline(send_line, MAX_LINE)) {
        if (std::strcmp(send_line, "exit") == 0)
            break ;
        if (send(this->_sockFd, send_line, strlen(send_line), 0) == -1)
            err_quit("error 12");

        if (recv(this->_sockFd, recv_line, MAX_LINE, 0) == -1)
            err_quit("invite_action_on_server: server terminated prematurely");
        std::cout << recv_line << std::endl;
        bzero(&send_line, sizeof(send_line));
        bzero(&recv_line, sizeof(recv_line));
    }
}
