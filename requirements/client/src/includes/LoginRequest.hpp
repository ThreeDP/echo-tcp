#ifndef __LOGIN_REQUEST_HPP__
# define __LOGIN_REQUEST_HPP__

# include "./header.h"

# define LOGIN_USER_SIZE 32
# define LOGIN_PASS_SIZE 32
# define LOGIN_RESPOSE_SIZE (HEADER_SIZE + 2)
# define LOGIN_BUFFER_SIZE (LOGIN_PASS_SIZE + LOGIN_USER_SIZE + HEADER_SIZE)

typedef struct s_login_request {
	t_header		header;
	char			username[LOGIN_USER_SIZE];
	char			password[LOGIN_PASS_SIZE];
}					t_login_request;

class LoginRequest {
	const std::string		_username;
	const std::string		_password;
	t_login_request			_labelRequest;
	char					_sendBuf[LOGIN_BUFFER_SIZE];
	char					_recvBuf[LOGIN_RESPOSE_SIZE];
	std::stack<uint32_t>	_keys;

	void	mountRequest(uint8_t seq);
	bool	checkRequest(void);
	void	checkAndmemcpy(size_t sizeStr, size_t fixedSize, char *dest, const char *src);
	void	genInitialKey(void);
	uint8_t	checkSum(std::string str);
	void	nextKey(void);

	public:
		LoginRequest(void) : _username(""), _password("") {}
		LoginRequest(std::string user) : _username(user), _password("") {}
		LoginRequest(std::string user, std::string pass) : _username(user), _password(pass) {}
		~LoginRequest(void){}
   
		void	encryptMessage(char *str, ssize_t size);
		bool	login(int sockFD);
};

std::ostream &operator<<(std::ostream &os, t_header const &h);

#endif