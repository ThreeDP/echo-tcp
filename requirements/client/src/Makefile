

NAME						=	client
FLAGS						=	-g3
HEADERS_PATH				=	./includes/
HEADERS						=	$(addprefix $(HEADERS_PATH), \
									Client.hpp LoginRequest.hpp header.h signals.h)
SRCS						=	main.cpp Client.cpp LoginRequest.cpp signals.cpp
OBJ_PATH					=	./objs/
OBJS						=	$(SRCS:%.cpp=$(OBJ_PATH)%.o)

TESTS						=	hello_test

all: $(NAME)

$(OBJ_PATH)%.o: %.cpp $(HEADERS)
	mkdir -p $(OBJ_PATH)
	c++ $(FLAGS) -I ./ -c $< -o $@

$(NAME): $(OBJS)
	echo $(OBJS)
	c++ $(OBJS) -o $(NAME)

build_tests:
	cmake -S ./test -B ./test/build
	cmake --build ./test/build

run_tests: $(TESTS)

$(TESTS):
	./test/build/$@

clean:
	rm -rf $(OBJ_PATH)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY:
	all clean fclean re objs

