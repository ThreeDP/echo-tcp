#ifndef __LOGIN_RESPONSE_HPP__
# define __LOGIN_RESPONSE_HPP__

# include "./header.h"

# define LOGIN_USER_SIZE 32
# define LOGIN_PASS_SIZE 32
# define LOGIN_RESPOSE_SIZE (HEADER_SIZE + 2)
# define LOGIN_BUFFER_SIZE (LOGIN_PASS_SIZE + LOGIN_USER_SIZE + HEADER_SIZE)

typedef struct s_login_response {
    t_header        header;
    uint16_t        status;
}               t_login_response;

typedef struct s_login_request {
	t_header		header;
	char			username[LOGIN_USER_SIZE];
	char			password[LOGIN_PASS_SIZE];
}					t_login_request;

class LoginResponse {
    t_login_response        _labelResponse;
    char				    _sendBuf[LOGIN_RESPOSE_SIZE];
	char				    _recvBuf[LOGIN_BUFFER_SIZE];
    std::stack<uint32_t>    _keys;

    uint8_t	checkSum(std::string str);
	void	genInitialKey(std::string user, std::string pass, uint8_t seq);
	void	nextKey(void);
    void    mountResponse(void);

    public:
        LoginResponse() {};
        ~LoginResponse(void) {};

	    void        decryptMessage(char *str, ssize_t size);
        bool        loginAuthentication(int sockFD);
};

std::ostream &operator<<(std::ostream &os, t_login_request const &h);

#endif