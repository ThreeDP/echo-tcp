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
    if (this->_autheticate.login(this->_sockFd, 0) == false) {
        std::string err = "Error: autheticate.";
        throw(Except(err));
    }
    std::cout << "Connect on host: " << this->_serverHost << " | Port: " << this->_serverPort << std::endl;
}

ssize_t    Client::mountRequest(char *send, char *line, uint8_t msgSeq) {
    uint16_t        size;
    t_echo_request  eReq;

    bzero(send, MAX_LINE);
    bzero(&eReq, sizeof(eReq));
    size = strlen(line) + HEADER_SIZE;
    eReq.header = (t_header) {.messageSize=size, .messageType=TEXT_REQUEST_TYPE, .messageSequence=msgSeq};
    this->_autheticate.encryptMessage(line, size - HEADER_SIZE);
    memcpy(&eReq.cipherMessage, &line, size - HEADER_SIZE);
    memcpy(send, &eReq, eReq.header.messageSize);
    bzero(line, MAX_LINE);
    return eReq.header.messageSize;
}

t_echo_response    Client::unmountResponse(char *line) {
    t_echo_response eRes;

    bzero(&eRes, sizeof(eRes));
    memcpy(&eRes, line, MAX_LINE);
    bzero(line, MAX_LINE);
    return eRes;
}

void    Client::sendMessageToServer(void) {
    ssize_t         bytes_sent;
    char            buf[MAX_LINE];
    ssize_t         bytes_received;
    char            communication[MAX_LINE];
    t_echo_response eRes;

    bzero(&buf, sizeof(buf));
    for (uint8_t i; std::cin.getline(buf, MAX_LINE); i++) {
        bytes_sent = 0;
        bytes_received = 0;
        bzero(&eRes, sizeof(eRes));
        if (buf == std::string(""))
            continue ;
        if (buf == std::string("exit"))
            break ;
        this->mountRequest(communication, buf, i);
        if ((bytes_sent = send(this->_sockFd, communication, strlen(communication), 0)) == -1) {
            std::string err = "Error: sending data. [ " + std::string(strerror(errno)) + " ].";
            throw(Except(err));
        }
        bzero(communication, sizeof(communication));
        if ((bytes_received = recv(this->_sockFd, communication, MAX_LINE, 0)) == -1) {
            std::string err = "Error: receiving data [ " + std::string(strerror(errno)) + " ].";
            throw(Except(err));
        }
        eRes = this->unmountResponse(communication);
        std::cout << eRes.plainMessage << std::endl;
        if (i > 255)
            i = 0;
    }
}
