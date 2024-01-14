FROM alpine:3.19

RUN apk update
RUN apk add build-base
RUN apk add make
RUN apk add vim
RUN apk add cmake

COPY ./conf/start_server.sh /
RUN chmod +x start_server.sh

RUN mkdir server
WORKDIR server

ENTRYPOINT ["ash", "/start_server.sh"]