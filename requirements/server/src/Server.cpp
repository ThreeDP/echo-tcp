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

void    printTest1(t_echo_request eReq) {
    std::cout << "[=============== HEADER Req ===============]" << std::endl;
    std::cout << "Message Size: '" << static_cast<int>(eReq.header.messageSize) << "'\n";
    std::cout << "Message Type: '" << static_cast<int>(eReq.header.messageType) << "'\n";
    std::cout << "Message Sequence: '" << static_cast<int>(eReq.header.messageSequence) << "'\n";
    std::cout << "[=============== BODY ===============]" << std::endl;
    std::cout << "Message: '" << eReq.cipherMessage << "'\n";
}

t_echo_request  Server::unmountRequest(char *line) {
    t_echo_request  eReq;

    bzero(&eReq, sizeof(eReq));
    memcpy(&eReq, line, MAX_LINE);
    bzero(line, MAX_LINE);
    return eReq;
}

void    printTest2(t_echo_response eRes) {
    std::cout << "[=============== HEADER Res ===============]" << std::endl;
    std::cout << "Message Size: '" << static_cast<int>(eRes.header.messageSize) << "'\n";
    std::cout << "Message Type: '" << static_cast<int>(eRes.header.messageType) << "'\n";
    std::cout << "Message Sequence: '" << static_cast<int>(eRes.header.messageSequence) << "'\n";
    std::cout << "[=============== BODY ===============]" << std::endl;
    std::cout << "Message: '" << eRes.plainMessage << "'\n";
}

ssize_t    Server::mountResponse(char *send, t_echo_request *eReq, uint8_t msgSeq) {
    uint16_t        size;
    t_echo_response eRes;

    bzero(send, MAX_LINE);
    bzero(&eRes, sizeof(eRes));
    size = eReq->messageSize + HEADER_SIZE + MSG_SIZE;
    eRes.header = (t_header) {.messageSize=size, .messageType=TEXT_RESPONSE_TYPE, .messageSequence=msgSeq};
    this->_login.decryptMessage(eReq->cipherMessage, eReq->messageSize);
    memcpy(&eRes.plainMessage, eReq->cipherMessage, eReq->messageSize);
    memcpy(send, &eRes, eRes.header.messageSize);
    printTest2(eRes);
    return eRes.header.messageSize;
}

void    Server::handleServerCalls(void) {
    ssize_t         n;
    char            buf[MAX_LINE];
    t_echo_request  eReq;
    ssize_t         size;

    bzero(&buf, sizeof(buf));
    while (1) {
        while ((recv(this->_connectFD, buf, MAX_LINE, 0)) > 0) {
            eReq = this->unmountRequest(buf);
            printTest1(eReq);
            bzero(&buf, sizeof(buf));
            size = this->mountResponse(buf, &eReq, 0);
            if (send(this->_connectFD, buf, size, 0) == -1) {
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