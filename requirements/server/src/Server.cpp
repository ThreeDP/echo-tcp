#include "./includes/Server.hpp"

void    printTest1(t_echo_request eReq) {
    std::cout << "[=============== HEADER Req ===============]" << std::endl;
    std::cout << "Message Size: '" << static_cast<int>(eReq.header.messageSize) << "'\n";
    std::cout << "Message Type: '" << static_cast<int>(eReq.header.messageType) << "'\n";
    std::cout << "Message Sequence: '" << static_cast<int>(eReq.header.messageSequence) << "'\n";
    std::cout << "[=============== BODY ===============]" << std::endl;
    std::cout << "Message Size: '" << eReq.messageSize << "'\n";
    std::cout << "Cipher Message: '";
    for (int i = 0; i < eReq.messageSize; i++) {
        std::cout << std::hex << static_cast<int>(eReq.cipherMessage[i]) << " ";
    }
    std::cout << "'\n";
}

void    printTest2(t_echo_response eRes) {
    std::cout << "[=============== HEADER Res ===============]" << std::endl;
    std::cout << "Message Size: '" << static_cast<int>(eRes.header.messageSize) << "'\n";
    std::cout << "Message Type: '" << static_cast<int>(eRes.header.messageType) << "'\n";
    std::cout << "Message Sequence: '" << static_cast<int>(eRes.header.messageSequence) << "'\n";
    std::cout << "[=============== BODY ===============]" << std::endl;
    std::cout << "Message Size: '" << eRes.messageSize << "'\n";
    std::cout << "Plain Text: '" << eRes.plainMessage << "'\n";
}


Server::Server(std::string sh, std::string sp) : _serverHost(std::stoul(sh)), _serverPort(std::stoul(sp)), _backlog(8) {
    bzero(&this->_serverAddr, sizeof(this->_serverAddr));
}

Server::~Server(void) {
    while (!this->_socksFDs.empty()) {
        close(this->_socksFDs.top());
        this->_socksFDs.pop();
    }
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

t_echo_request  Server::unmountRequest(char *line) {
    t_echo_request  eReq;

    bzero(&eReq, sizeof(eReq));
    memcpy(&eReq, line, MAX_LINE);
    bzero(line, MAX_LINE);
    return eReq;
}

ssize_t    Server::mountResponse(char *send, t_echo_request *eReq) {
    uint16_t        size;
    t_echo_response eRes;

    bzero(send, MAX_LINE);
    bzero(&eRes, sizeof(eRes));
    size = eReq->messageSize + HEADER_SIZE + MSG_SIZE;
    eRes.header = (t_header) {.messageSize=size, .messageType=TEXT_RESPONSE_TYPE, .messageSequence=eReq->header.messageSequence};
    this->_login.decryptMessage(eReq->cipherMessage, eReq->messageSize);
    memcpy(&eRes.plainMessage, eReq->cipherMessage, eReq->messageSize);
    memcpy(send, &eRes, eRes.header.messageSize);
    printTest2(eRes);
    return eRes.header.messageSize;
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
        this->_socksFDs.push(this->_connectFD);
        if ((this->_childPid = fork()) == 0) {
            wait_sig_child();
            close(this->_listenFD);
            if (this->_login.loginAuthentication(this->_connectFD) == false)
                exit(1);
            this->handleServerCalls(this->_connectFD);
            close(this->_connectFD);
            exit(0);
        }
    }
}

void    Server::handleServerCalls(int connectFD) {
    t_echo_request  eReq;
    ssize_t         bytes;

    while (1) {
        bytes = 0;
        while (1) {
            eReq = this->recvMessage(&bytes, connectFD);
            this->sendMessage(&eReq);
            if (bytes <= 0)
                break ;
        }
        if (bytes < 0 && errno == EINTR)
            continue ;
        else if (bytes < 0) {
            std::cerr << "Error: receiving data. " << std::string(strerror(errno)) << " ]." << std::endl;
            exit(1);
        }
        break ;
    }
}

t_echo_request    Server::recvMessage(ssize_t *bytes, int connectFD) {
    char            buf[MAX_LINE];
    t_echo_request  eReq;

    bzero(buf, sizeof(buf));
    bzero(&eReq, sizeof(eReq));
    *bytes = recv(this->_connectFD, buf, MAX_LINE, 0);
    eReq = this->unmountRequest(buf);
    printTest1(eReq);
    return eReq;
}

void    Server::sendMessage(t_echo_request *eReq) {
    ssize_t size;
    ssize_t bytes;
    char    buf[MAX_LINE];

    bzero(&buf, sizeof(buf));
    size = this->mountResponse(buf, eReq);
    bytes = send(this->_connectFD, buf, size, 0);
    if (bytes == -1) {
        std::cerr << "Error: sending data. " << std::string(strerror(errno)) << " ]." << std::endl;
        exit(1);
    }
}