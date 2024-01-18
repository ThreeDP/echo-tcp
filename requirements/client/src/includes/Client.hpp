#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

# include "Login.hpp"

typedef struct sockaddr_in sock_address;
typedef struct addrinfo addrinfo;

typedef struct s_echo_request {
    t_header        header;
    uint16_t        messageSize;
    char            cipherMessage[MAX_CIPHER_SIZE];
}               t_echo_request;

class Client {
    int             _sockFd;
    std::string     _serverHost;
    std::string     _serverPort;
    addrinfo        _clientAddr;
    addrinfo        *_sockAddr;
    Login           _autheticate;
    Client(void){};

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