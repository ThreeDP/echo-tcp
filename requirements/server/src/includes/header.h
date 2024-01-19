#ifndef __header_H__
# define __header_H__

# include <sys/socket.h>
# include <sys/types.h>
# include <cstring>
# include <arpa/inet.h>
# include <unistd.h>
# include <stdlib.h>
# include <errno.h>
# include <iostream>
# include <stack>

# define HEADER_SIZE 4
# define MAX_LINE 65536
# define MSG_SIZE 2
# define MAX_TEXT_SIZE (MAX_LINE - HEADER_SIZE - MSG_SIZE)
# define LOGIN_RESPONSE_TYPE 1
# define TEXT_RESPONSE_TYPE 3

typedef struct s_header {
    uint16_t        messageSize;
    uint8_t         messageType;
    uint8_t         messageSequence;
}               t_header;

typedef struct s_echo_request {
    t_header        header;
    uint16_t        messageSize;
    char            cipherMessage[MAX_TEXT_SIZE];
}               t_echo_request;

typedef struct s_echo_response {
    t_header        header;
    uint16_t        messageSize;
    char            plainMessage[MAX_TEXT_SIZE];
}               t_echo_response;

#endif