#include "./includes/LoginResponse.hpp"

uint8_t	LoginResponse::checkSum(std::string str) {
	uint8_t	sum;
	ssize_t	size;

	size = str.size();
	for (ssize_t i = 0; i < size; i++) {
		sum += str[i];
	}
	return (~sum);
}

void    printTest(t_login_request lr) {
    std::cout << "[=============== HEADER ===============]" << std::endl;
    std::cout << "Message Size: '" << static_cast<int>(lr.header.messageSize) << "'\n";
    std::cout << "Message Type: '" << static_cast<int>(lr.header.messageType) << "'\n";
    std::cout << "Message Sequence: '" << static_cast<int>(lr.header.messageSequence) << "'\n";
    std::cout << "[=============== BODY ===============]" << std::endl;
    std::cout << "Username: '" << lr.username << "'\n";
    std::cout << "Password: '" << lr.password << "'\n";
}

void	LoginResponse::genInitialKey(std::string user, std::string pass, uint8_t seq) {
	uint32_t	initialKey;
	
	initialKey = (seq << 16) | \
		(this->checkSum(user) << 8) | \
		this->checkSum(pass);
	this->_keys.push(initialKey);
}

void	LoginResponse::nextKey(void) {
	uint32_t	newKey;
	
	newKey = (this->_keys.top() * 1103515245 + 12345) % 0x7FFFFFFF;
	this->_keys.push(newKey);
}

void	LoginResponse::decryptMessage(std::string &str) {
	ssize_t	size;

	size = str.size();
	for (ssize_t i = 0; i < size; i++) {
		str[i] ^= (this->_keys.top() % 256);
	}
}

void    LoginResponse::mountResponse(void) {
    t_login_request lr;
    t_login_response lres;

    bzero(&lr, LOGIN_BUFFER_SIZE);
    bzero(&lres, LOGIN_RESPOSE_SIZE);
    memcpy(&lr, this->_recvBuf, LOGIN_BUFFER_SIZE);
    printTest(lr);
    lres.header.messageSize = LOGIN_RESPOSE_SIZE;
    lres.header.messageType = LOGIN_RESPONSE_TYPE;
    lres.header.messageSequence = this->_msgSeq;
    if (!std::strcmp(lr.username, "") || !std::strcmp(lr.password, ""))
        lres.status = 0;
    else
        lres.status = 1;
    memcpy(this->_sendBuf, &lres, sizeof(lres));
}

bool    LoginResponse::loginAuthentication(int sockFD, uint8_t msgSeq) {

	ssize_t	bytes_send;
	ssize_t bytes_recv;

    this->_msgSeq = msgSeq;
    bzero(&this->_recvBuf, sizeof(this->_recvBuf));
    bzero(&this->_sendBuf, sizeof(this->_sendBuf));
	bytes_recv = recv(sockFD, this->_recvBuf, LOGIN_BUFFER_SIZE, 0);
	if (bytes_recv == -1)
		return false;
    this->mountResponse();
    bytes_send = send(sockFD, this->_sendBuf, LOGIN_RESPOSE_SIZE, 0);
	if (bytes_send == -1)
		return false;
    return true;
}