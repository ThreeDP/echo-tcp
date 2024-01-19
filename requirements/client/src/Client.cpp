#include "./includes/Client.hpp"

void    printTest1(t_echo_response eRes) {
    std::cout << "[=============== HEADER Res ===============]" << std::endl;
    std::cout << "Message Size: '" << static_cast<int>(eRes.header.messageSize) << "'\n";
    std::cout << "Message Type: '" << static_cast<int>(eRes.header.messageType) << "'\n";
    std::cout << "Message Sequence: '" << static_cast<int>(eRes.header.messageSequence) << "'\n";
    std::cout << "[=============== BODY ===============]" << std::endl;
    std::cout << "Message Size: '" << eRes.messageSize << "'\n";
    std::cout << "Plain Text: '" << eRes.plainMessage << "'\n";
}

void    printTest2(t_echo_request eReq) {
    std::cout << "[=============== HEADER Req ===============]" << std::endl;
    std::cout << "Message Size: '" << static_cast<int>(eReq.header.messageSize) << "'\n";
    std::cout << "Message Type: '" << static_cast<int>(eReq.header.messageType) << "'\n";
    std::cout << "Message Sequence: '" << static_cast<int>(eReq.header.messageSequence) << "'\n";
    std::cout << "[=============== BODY ===============]" << std::endl;
    std::cout << "Message Size: '" << eReq.messageSize << "'\n";
    std::cout << "Cipher Message: '" << eReq.cipherMessage << "'\n";
}

Client::Client(std::string sh, std::string sp, std::string user, std::string pass) \
: _serverHost(sh), _serverPort(sp), _autheticate(LoginRequest(user, pass)) {
    bzero(&this->_clientAddr, sizeof(this->_clientAddr));
    this->_sockFd = 0;
}

Client::~Client(void) {
    close(this->_sockFd);
    free(this->_sockAddr);
}

ssize_t    Client::mountRequest(char *send, char *line, uint8_t msgSeq) {
    uint16_t        size;
    t_echo_request  eReq;
    uint16_t        lineSize;

    bzero(send, MAX_LINE);
    bzero(&eReq, sizeof(eReq));
    lineSize = strlen(line);
    size = lineSize + HEADER_SIZE + MSG_SIZE;
    eReq.header = (t_header) {.messageSize=size, .messageType=TEXT_REQUEST_TYPE, .messageSequence=msgSeq};
    eReq.messageSize = lineSize;
    this->_autheticate.encryptMessage(line, lineSize);
    memcpy(&eReq.cipherMessage, line, lineSize);
    memcpy(send, &eReq, MAX_CIPHER_SIZE);
    bzero(line, MAX_LINE);
    printTest2(eReq);
    return eReq.header.messageSize;
}

t_echo_response    Client::unmountResponse(char *line) {
    t_echo_response eRes;

    bzero(&eRes, sizeof(eRes));
    memcpy(&eRes, line, MAX_LINE);
    bzero(line, MAX_LINE);
    return eRes;
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

void    Client::sendMessageToServer(void) {
    ssize_t         size;
    char            line[MAX_LINE];

    bzero(&line, sizeof(line));
    for (uint8_t seqMsg; std::cin.getline(line, MAX_LINE); seqMsg++) {
        if (line == std::string(""))
            continue ;
        if (line == std::string("exit"))
            break ;
        this->sendMessage(line, seqMsg);
        this->recvMessage();
        if (seqMsg > 255)
            seqMsg = 0;
    }
}

void    Client::sendMessage(char *line, uint8_t seqMsg) {
    ssize_t size;
    ssize_t bytes;
    char    buf[MAX_LINE];

    size = this->mountRequest(buf, line, seqMsg);
    bytes = send(this->_sockFd, buf, size, 0);
    if (bytes == -1) {
        std::string err = "Error: sending data. [ " + std::string(strerror(errno)) + " ].";
        throw(Except(err));
    }
}

void    Client::recvMessage(void) {
    t_echo_response eRes;
    ssize_t         bytes;
    char            buf[MAX_LINE];
    
    bzero(buf, sizeof(buf));
    bzero(&eRes, sizeof(eRes));
    bytes = recv(this->_sockFd, buf, MAX_LINE, 0);
    if (bytes == -1) {
        std::string err = "Error: receiving data [ " + std::string(strerror(errno)) + " ].";
        throw(Except(err));
    }
    eRes = this->unmountResponse(buf);
    printTest1(eRes);
}
