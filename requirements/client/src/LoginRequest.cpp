#include "./includes/LoginRequest.hpp"

std::ostream &operator<<(std::ostream &os, t_header const &h) {
	os << "\n|================= HEADER =================|" << std::endl;
    os << "| Message Size: " << static_cast<int>(h.messageSize) << "\n";
    os << "| Message Type: " << static_cast<int>(h.messageType) << "\n";
    os << "| Message Sequence: " << static_cast<int>(h.messageSequence) << "\n";
    os << "|================== BODY ==================|" << std::endl;
    os << "| Status: ";
	return (os);
}

void	LoginRequest::checkAndmemcpy(size_t sizeStr, size_t fixedSize, char *dest, const char *src) {
	if (sizeStr < fixedSize)
		memcpy(dest, src, sizeStr);
	else
		memcpy(dest, src, fixedSize);
}

void	LoginRequest::mountRequest(uint8_t seq) {
	bzero(&this->_sendBuf, sizeof(this->_sendBuf));
	bzero(&this->_labelRequest, sizeof(this->_labelRequest));
	this->_labelRequest.header = (t_header) {.messageSize=LOGIN_BUFFER_SIZE, .messageType=LOGIN_REQUEST_TYPE, .messageSequence=seq};
	this->checkAndmemcpy(this->_username.size(), LOGIN_USER_SIZE, this->_labelRequest.username, this->_username.c_str());
	this->checkAndmemcpy(this->_password.size(), LOGIN_PASS_SIZE, this->_labelRequest.password, this->_password.c_str());
	memcpy(this->_sendBuf, &this->_labelRequest, LOGIN_BUFFER_SIZE);
}

bool	LoginRequest::checkRequest(void) {
	t_header	headerResponse;
	uint16_t	response;

	headerResponse = (t_header){0};
	response = 0;
	memcpy(&headerResponse, this->_recvBuf, sizeof(headerResponse));
	memcpy(&response, &this->_recvBuf[sizeof(headerResponse)], sizeof(response));
	std::cout << headerResponse << response << "\n\n";
	if (!response)
		return false;
	return true;
}

uint8_t	LoginRequest::checkSum(std::string str) {
	uint8_t	sum;
	ssize_t	size;

	sum = 0;
	size = str.size();
	for (ssize_t i = 0; i < size; i++) {
		sum += str[i];
	}
	return (~sum);
}

void	LoginRequest::genInitialKey(void) {
	uint32_t	initialKey;

	initialKey = (this->_labelRequest.header.messageSequence << 16) | \
		(this->checkSum(this->_username) << 8) | \
		this->checkSum(this->_password);
	this->_keys.push(initialKey);
}

void	LoginRequest::nextKey(void) {
	uint32_t	newKey;
	
	newKey = (this->_keys.top() * 1103515245 + 12345) % 0x7FFFFFFF;
	this->_keys.push(newKey);
}

void	LoginRequest::encryptMessage(char *str, ssize_t size) {
	for (ssize_t i = 0; i < size; i++) {
		uint8_t num = (static_cast<uint8_t>(str[i]) ^ this->_keys.top()) % 256;
		str[i] = static_cast<uint8_t>(num);
	}
	this->nextKey();
}

bool	LoginRequest::login(int sockFD) {
	ssize_t	bytes_send;
	ssize_t bytes_recv;

	this->mountRequest((this->checkSum(this->_username) * this->checkSum(this->_password)) % 256);
	bytes_send = send(sockFD, this->_sendBuf, LOGIN_BUFFER_SIZE, 0);
	if (bytes_send == -1)
		return false;
	bytes_recv = recv(sockFD, this->_recvBuf, LOGIN_BUFFER_SIZE, 0);
	if (bytes_recv == -1)
		return false;
	if (!this->checkRequest()) {
		std::cerr << "FAILED: Login or password incorrect" << std::endl;
		return false;
	}
	this->genInitialKey();
	return true;
}