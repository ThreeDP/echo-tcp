#include "./includes/Login.hpp"

void	Login::checkAndmemcpy(size_t sizeStr, size_t fixedSize, char *dest, const char *src) {
	if (sizeStr < fixedSize)
		memcpy(dest, src, sizeStr);
	else
		memcpy(dest, src, fixedSize);
}

void	Login::mountRequest(uint8_t seq) {
	bzero(&this->_sendBuf, sizeof(this->_sendBuf));
	bzero(&this->_labelRequest, sizeof(this->_labelRequest));
	this->_labelRequest.header = (t_header) {.messageSize=LOGIN_BUFFER_SIZE, .messageType=0, .messageSequence=seq};
	this->checkAndmemcpy(this->_username.size(), LOGIN_USER_SIZE, this->_labelRequest.username, this->_username.c_str());
	this->checkAndmemcpy(this->_password.size(), LOGIN_PASS_SIZE, this->_labelRequest.password, this->_password.c_str());
	memcpy(this->_sendBuf, &this->_labelRequest, LOGIN_BUFFER_SIZE);
}

bool	Login::checkRequest(void) {
	t_header	headerResponse;
	uint16_t	response;

	headerResponse = (t_header){0};
	response = 0;
	memcpy(&headerResponse, this->_recvBuf, sizeof(headerResponse));
	memcpy(&response, &this->_recvBuf[sizeof(headerResponse)], sizeof(response));
	if (!response)
		return false;
	return true;
}

bool	Login::login(int sockFD, uint8_t seq) {
	ssize_t	bytes_send;
	ssize_t bytes_recv;

	this->mountRequest(seq);
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
	return true;
}