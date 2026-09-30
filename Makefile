ifeq ($(HOSTTYPE),)
	HOSTTYPE	:=	$(shell uname -m)_$(shell uname -s)
endif

NAME			:=	libft_malloc_$(HOSTTYPE).so
CC				:=	gcc
AR				:=	ar rcs
FLAGS			:=	-Werror -Wall -Wextra -g3 -c -Iinclude


OBJS_DIR		:=	objs

SRCS			:=	malloc.c \

OBJS			:=	$(OBJS_DIR)/$(SRCS:%.c=%.o)


# RULES
all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -shared -o $(NAME) $(OBJS)

$(OBJS_DIR)/%.o: srcs/%.c
	@mkdir -p $(OBJS_DIR)
	$(CC) $(FLAGS) -fPIC $< -o $@

clean:
	rm -f $(NAME)
	rm -f a.out

fclean: clean
	rm -rf $(OBJS_DIR)
	make -C libft/ fclean

re: fclean all

test: $(NAME)
	cc main.c -L. -lft_malloc_x86_64_Linux -Ilibft -Iinclude

.PHONY: all clean fclean re test
