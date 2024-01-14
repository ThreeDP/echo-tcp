FROM alpine:3.19

RUN apk update
RUN apk add build-base
RUN apk add make
RUN apk add vim
RUN apk add cmake

COPY ./conf/start_client.sh /
RUN chmod +x start_client.sh

RUN mkdir client
WORKDIR client

ENTRYPOINT ["ash", "/start_client.sh"]