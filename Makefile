ifeq ($(HOSTTYPE),)
	HOSTTYPE	:=	$(shell uname -m)_$(shell uname -s)
endif

NAME			:=	libft_malloc_$(HOSTTYPE).so
CC				:=	gcc
FLAGS			:=	-Werror -Wall -Wextra -g3 -c -Iinclude -MMD -MP


OBJS_DIR		:=	objs

SRCS			:=	malloc.c	\
					realloc.c		\
					free.c		\
					mutils.c	\
					pools.c	\

OBJS			:=	$(SRCS:%.c=$(OBJS_DIR)/%.o)

DEPS			:=	$(OBJS:%.o=%.d)

# RULES
all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -shared -o $(NAME) $(OBJS)

$(OBJS_DIR)/%.o: srcs/%.c
	@mkdir -p $(OBJS_DIR)
	$(CC) $(FLAGS) -fPIC $< -o $@

-include $(DEPS)

clean:
	rm -f $(NAME)
	rm -f a.out

fclean: clean
	rm -rf $(OBJS_DIR)
# 	make -C libft/ fclean

re: fclean all

test: $(NAME)
	cc main.c -L. -lft_malloc_x86_64_Linux -Ilibft -Iinclude -g3

.PHONY: all clean fclean re test
