FROM alpine:3.19

RUN apk update
RUN apk add build-base
RUN apk add make
RUN apk add vim
RUN apk add cmake

RUN mkdir client
WORKDIR client
COPY ./conf/start_client.sh /client
RUN chmod +x /client/start_client.sh

ENTRYPOINT ["ash", "start_client.sh"]