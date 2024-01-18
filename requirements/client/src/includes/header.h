#ifndef __HEADER_H__
# define __HEADER_H__

# include <sys/socket.h>
# include <sys/types.h>
# include <cstring>
# include <string>
# include <arpa/inet.h>
# include <unistd.h>
# include <stdlib.h>
# include <errno.h>
# include <iostream>
# include <netdb.h>
# include <stdio.h>

# define HEADER_SIZE 4
# define MAX_LINE 65536
# define MAX_CIPHER_SIZE (MAX_LINE - HEADER_SIZE)

typedef struct s_header {
    uint16_t        messageSize;
    uint8_t         messageType;
    uint8_t         messageSequence;
}               t_header;

#endif