COMPOSE 			= ./requirements/
CONTAINERS_IMGS		= client-img server-img
CONTAINERS_PS		= client-service server-service

all: build up

build:
	docker-compose -f $(COMPOSE) build

up:
	docker-compose -f $(COMPOSE) up -d

down:
	docker-compose -f $(COMPOSE) down 

clean-p:
	docker rm -rf $(CONTAINERS_PS)

clean-i:
	docker rmi -rf $(CONTAINERS_IMGS)

fclean: clean-p clean-i

re:  fclean all

.PHONY: all build up down clean fclean