ifeq ($(HOSTTYPE),)
	HOSTTYPE	:=	$(shell uname -m)_$(shell uname -s)
endif

NAME			:=	libft_malloc_$(HOSTTYPE).so
CC				:=	clang
AR				:=	ar rcs
FLAGS			:=	-Werror -Wall -Wextra -g3 -c

SRCS			:=	malloc.c \
					free.c \
					realloc.c \
					show_alloc_mem.c\

OBJS			:=	$(SRCS:%.c=%.o)


# RULES
all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -shared -o $(NAME) $(OBJS)
# 	$(AR) $(NAME) $(OBJS)

%.o: %.c
	$(CC) $(FLAGS) -fPIC $< -o $@

clean:
	rm $(OBJS)

fclean: clean
	rm $(NAME)

re: fclean all

test: $(NAME)
	make -C libft/
	cc main.c -L. -lft_malloc_x86_64_Linux -Ilibft -l:libft/libft.a

.PHONY: all clean fclean re test
