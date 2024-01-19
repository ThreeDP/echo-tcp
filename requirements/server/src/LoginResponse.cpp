#include "./includes/LoginResponse.hpp"

std::ostream &operator<<(std::ostream &os, t_login_request const &lr) {
	os << "\n|================= HEADER =================|" << std::endl;
    os << "| Message Size: " << static_cast<int>(lr.header.messageSize) << "\n";
    os << "| Message Type: " << static_cast<int>(lr.header.messageType) << "\n";
    os << "| Message Sequence: " << static_cast<int>(lr.header.messageSequence) << "\n";
    os << "|================== BODY ==================|" << std::endl;
    os << "| Usernames: " << lr.username << std::endl;
    os << "| Password: " << lr.password << std::endl << std::endl;
	return (os);
}

uint8_t	LoginResponse::checkSum(std::string str) {
	uint8_t	sum;
	ssize_t	size;

    sum = 0;
	size = str.size();
	for (ssize_t i = 0; i < size; i++) {
		sum += str[i];
	}
	return (~sum);
}

void	LoginResponse::genInitialKey(std::string user, std::string pass, uint8_t seq) {
	uint32_t	initialKey;

	initialKey = (seq << 16) | \
		(this->checkSum(user.c_str()) << 8) | \
		this->checkSum(pass.c_str());
	this->_keys.push(initialKey);
}

void	LoginResponse::nextKey(void) {
	uint32_t	newKey;
	
	newKey = (this->_keys.top() * 1103515245 + 12345) % 0x7FFFFFFF;
	this->_keys.push(newKey);
}

void	LoginResponse::decryptMessage(char *str, ssize_t size) {
	for (ssize_t i = 0; i < size; i++) {
		str[i] = static_cast<uint8_t>(str[i] ^ (this->_keys.top() % 256));
	}
	this->nextKey();
}

void    LoginResponse::mountResponse(void) {
    t_login_request lReq;
    t_login_response lRes;

    bzero(&lReq, LOGIN_BUFFER_SIZE);
    bzero(&lRes, LOGIN_RESPOSE_SIZE);
    memcpy(&lReq, this->_recvBuf, LOGIN_BUFFER_SIZE);
    std::cout << lReq;
    lRes.header.messageSize = LOGIN_RESPOSE_SIZE;
    lRes.header.messageType = LOGIN_RESPONSE_TYPE;
    lRes.header.messageSequence = lReq.header.messageSequence;
    if (!std::strcmp(lReq.username, "") || !std::strcmp(lReq.password, ""))
        lRes.status = 0;
    else {
        lRes.status = 1;
        this->genInitialKey(lReq.username, lReq.password, lReq.header.messageSequence);
    }
    memcpy(this->_sendBuf, &lRes, sizeof(lRes));
}

bool    LoginResponse::loginAuthentication(int sockFD) {

	ssize_t	bytes_send;
	ssize_t bytes_recv;

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