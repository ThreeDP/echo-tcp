#ifndef __HANDLE_NET_HPP__
# define __HANDLE_NET_HPP__

# include "./LoginResponse.hpp"

typedef struct sockaddr_in sock_address;

class Server {
    int                 _listenFD;
    int                 _connectFD;
    const unsigned int  _backlog;
    sock_address        _clientAddr;
    sock_address        _serverAddr;
    pid_t               _childPid;
    socklen_t           _childLen;
    LoginResponse       _login;
    std::stack<int>     _socksFDs;

    uint32_t            _serverHost;
    uint32_t            _serverPort;
    Server(void) : _backlog(8) {};

    ssize_t         mountResponse(char *send, t_echo_request *eReq, uint8_t msgSeq);
    t_echo_request  unmountRequest(char *line);
    t_echo_request  recvMessage(ssize_t *bytes, int connectFD);
    void            sendMessage(t_echo_request *eReq);

    public:
        Server(std::string sh, std::string sp);
        ~Server(void);

        void            configServer(void);
        void            putServerToListen(void);
        void            handleServerCalls(int connectFD);

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