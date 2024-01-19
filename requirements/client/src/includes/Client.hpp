#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

# include "LoginRequest.hpp"

typedef struct sockaddr_in sock_address;
typedef struct addrinfo addrinfo;

class Client {
    int             _sockFd;
    std::string     _serverHost;
    std::string     _serverPort;
    addrinfo        _clientAddr;
    addrinfo        *_sockAddr;
    LoginRequest    _autheticate;
    Client(void){};
    t_echo_response     unmountResponse(char *line);
    ssize_t             mountRequest(char *send, char *line, uint8_t msgSeq);

    public:
        Client(std::string sh, std::string sp, std::string user, std::string pass);
        ~Client(void);

        void    confingClient(void);
        void    sendMessageToServer(void);
        void    connectToServer(void);

        class Except: virtual public std::exception {
            protected:
                std::string error_message;
            public:
                explicit Except(const std::string &msg) : error_message(msg) {}
                virtual ~Except() throw () {}
                virtual const char *what() const throw () { return error_message.c_str(); }
        };
};

#endif