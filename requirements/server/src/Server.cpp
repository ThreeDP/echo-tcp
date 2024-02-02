#include "./includes/Server.hpp"

std::ostream &operator<<(std::ostream &os, t_echo_response const &eRes) {
	os << "\n|=============== HEADER Res ===============|" << std::endl;
    os << "| Message Size: " << static_cast<int>(eRes.header.messageSize) << "\n";
    os << "| Message Type: " << static_cast<int>(eRes.header.messageType) << "\n";
    os << "| Message Sequence: " << static_cast<int>(eRes.header.messageSequence) << "\n";
    os << "|================== BODY ==================|" << std::endl;
    os << "| Message Size: " << eRes.messageSize << "\n";
    os << "| Plain Message: [ " << eRes.plainMessage << " ]\n\n";
	return (os);
}

std::ostream &operator<<(std::ostream &os, t_echo_request const &eReq) {
	os << "\n|=============== HEADER Req ===============|" << std::endl;
    os << "| Message Size: " << static_cast<int>(eReq.header.messageSize) << "\n";
    os << "| Message Type: " << static_cast<int>(eReq.header.messageType) << "\n";
    os << "| Message Sequence: " << static_cast<int>(eReq.header.messageSequence) << "\n";
    os << "|================== BODY ==================|" << std::endl;
    os << "| Message Size: " << eReq.messageSize << "\n";
    os << "| Cipher Message: [ " << std::hex << std::uppercase;
    for (int i = 0; i < static_cast<int>(eReq.messageSize); i++) {
        uint8_t c = eReq.cipherMessage[i];
        os << static_cast<int>(c);
        if (i < static_cast<int>(eReq.messageSize) - 1)
            os << " ";
    }
    os << std::dec << " ]" << std::endl;
	return (os);
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
    std::cout << eRes;
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
    std::cout << eReq;
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