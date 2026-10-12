ifeq ($(HOSTTYPE),)
	HOSTTYPE	:=	$(shell uname -m)_$(shell uname -s)
endif

NAME			:=	libft_malloc_$(HOSTTYPE).so
SYMLINK			:=	libft_malloc.so

CC				:=	gcc
FLAGS			:=	-Werror -Wall -Wextra -g3 -c -Iinclude -MMD -MP

LIBFT			:=	libft/libft.a

OBJS_DIR		:=	objs

SRCS			:=	malloc.c	\
					realloc.c	\
					free.c		\
					mutils.c	\
					pools.c		\

OBJS			:=	$(SRCS:%.c=$(OBJS_DIR)/%.o)

DEPS			:=	$(OBJS:%.o=%.d)

RED				:=	\033[0;31m
GREEN			:=	\033[0;32m
YELLOW			:=	\033[0;33m
WHITE			:=	\033[0m
BOLD			:=	\033[1m
FLASH			:=	\033[6m
RETURN			:=	\033[2K\r

# RULES
all: $(NAME)

$(NAME): $(LIBFT) $(OBJS)
	@printf "$(RETURN)$(RED)[LINKING 🛞 ]$(WHITE) $@ :"

	@$(CC) -shared -o $(NAME) $(OBJS) $(LIBFT)
	@ln -f -s $(NAME) $(SYMLINK)

	@printf "$(RETURN)$(GREEN)[DONE! ✅]$(WHITE) $@\n"
	@printf "$(RED)$(BOLD)   ____________  \n"
	@printf "  (   READY !  ) \n"
	@printf "    ͞ v ͞ ͞ ͞ ͞ ͞ ͞ ͞ ͞ \n"
	@printf "   /\\/\\ \n"
	@printf "  ( $(FLASH)o$(RED)$(BOLD)w$(FLASH)o$(RED)$(BOLD))\n"
	@printf "  / >*<\\ \n$(WHITE)"

$(OBJS_DIR)/%.o: srcs/%.c
	@mkdir -p $(OBJS_DIR)
	@printf "$(RETURN)$(RED)[COMPILING 🛞 ]$(WHITE) $< :"
	@$(CC) $(FLAGS) -fPIC $< -o $@
	@printf "$(RETURN)$(GREEN)[DONE! ✅]$(WHITE) $< -> $@\n"

$(LIBFT):
	@make -s -C libft/ all

-include $(DEPS)

clean:
	@rm -rf $(OBJS_DIR)
	@printf "$(YELLOW)[CLEANED] Object directory removed.$(WHITE)\n"

fclean: clean
	@rm -f $(NAME)
	@rm -f $(SYMLINK)
	@printf "$(YELLOW)[CLEANED] Lib and link removed.$(WHITE)\n"
	@make -C libft/ fclean

re: fclean all

test: $(NAME)
	cc main.c -L. -lft_malloc -Iinclude -g3 -o test

.PHONY: all clean fclean re test
