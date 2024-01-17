#include "./includes/Client.hpp"

void    err_quit(std::string str) {
    std::cerr << str << std::endl;
    exit(1);
}

Client::Client(std::string sh, std::string sp, std::string user, std::string pass) \
: _serverHost(sh), _serverPort(std::stoul(sp)), _username(user), _password(pass) {
    bzero(&this->_clientAddr, sizeof(this->_clientAddr));
    this->_sockFd = 0;
}

Client::~Client(void) {

}

void    Client::confingClient(void) {
    this->_sockFd = socket(AF_INET, SOCK_STREAM, 0);
    this->_clientAddr.sin_family = AF_INET;
    this->_clientAddr.sin_port = htons(this->_serverPort);
    inet_pton(AF_INET, (const char *)this->_serverHost.c_str(), &this->_clientAddr.sin_addr);
}

void    Client::connectToServer(void) {
    connect(this->_sockFd, (const sockaddr *) &this->_clientAddr, sizeof(this->_clientAddr));
    std::cout << "Connect on host: " << this->_serverHost << " | Port: " << this->_serverPort << std::endl;
}

void    Client::authenticateToTheServer(void) {

}

void    Client::sendMessageToServer(void) {
    char    send_line[MAX_LINE];
    char    recv_line[MAX_LINE];

    while (std::cin.getline(send_line, MAX_LINE)) {
        if (send(this->_sockFd, send_line, strlen(send_line), 0) == -1)
            err_quit("error");

        if (recv(this->_sockFd, recv_line, MAX_LINE, 0) == -1)
            err_quit("invite_action_on_server: server terminated prematurely");

        std::cout << recv_line << std::endl;
        bzero(&send_line, sizeof(send_line));
        bzero(&recv_line, sizeof(recv_line));
    }
}
