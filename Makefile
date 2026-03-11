ifeq ($(HOSTTYPE),)
HOSTTYPE	:=	$(shell uname -m)_$(shell uname -s)
endif

NAME		=	libft_malloc_$(HOSTTYPE).so
CC			=	cc
FLAGS		=	-Werror -Wall -Wextra -g3 -c

SRCS		=	malloc.c \
				free.c \
				realloc.c \
				show_alloc_mem.c\

OBJS		=	$(SRCS:%.c=%.o)

all: $(NAME)

$(NAME): $(OBJS)
	ar rc $(NAME) $(OBJS)

%.o:%.c
	$(CC) $(FLAGS) $< -o $@

clean:
	rm $(OBJS)

fclean: clean
	rm $(NAME)

re: fclean all

test: $(NAME)
	cc main.c -L. -lft_malloc_x86_64_Linux

.PHONY: all clean fclean re
