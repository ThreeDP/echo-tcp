PROJECTS_PATH		= ./requirements/
COMPOSE 			= $(addprefix $(PROJECTS_PATH), docker-compose.yml)
CONTAINERS_IMGS		= client-img server-img
CONTAINERS_PS		= client server
CLIENT_PATH			= $(addprefix $(PROJECTS_PATH), client/src)
SERVER_PATH			= $(addprefix $(PROJECTS_PATH), server/src)

all: build up

build:
	docker-compose -f $(COMPOSE) build

up:
	docker-compose -f $(COMPOSE) up -d

client:
	docker exec -it client ash

down:
	docker-compose -f $(COMPOSE) down 

clean-p:
	docker rm -f $(CONTAINERS_PS)

clean-i:
	docker rmi -f $(CONTAINERS_IMGS)

clean:
	$(MAKE) -C $(CLIENT_PATH) $@ --no-print-directory
	$(MAKE) -C $(SERVER_PATH) $@ --no-print-directory

fclean: clean-p clean-i
	$(MAKE) -C $(CLIENT_PATH) $@ --no-print-directory
	$(MAKE) -C $(SERVER_PATH) $@ --no-print-directory

re:  fclean all

.PHONY: all build up down clean fclean client