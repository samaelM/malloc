NAME	:=	libft.a 

CC		:=	cc
FLAGS	:=	-Wall -Wextra -Werror -g3

AR		:=	ar
RM		:=	rm -f

FILES	:=	ft_strdup.c				\
			ft_atoi.c				\
			ft_bzero.c				\
			ft_isalnum.c			\
			ft_isalpha.c			\
			ft_isascii.c			\
			ft_isdigit.c			\
			ft_isprint.c			\
			ft_memchr.c				\
			ft_memcmp.c				\
			ft_memcpy.c				\
			ft_memmove.c			\
			ft_memset.c				\
			ft_strchr.c				\
			ft_strlcat.c			\
			ft_strlcpy.c			\
			ft_strlen.c				\
			ft_strncmp.c			\
			ft_strnstr.c			\
			ft_split.c				\
			ft_strrchr.c			\
			ft_tolower.c			\
			ft_toupper.c			\
			ft_calloc.c				\
			ft_itoa.c				\
			ft_strjoin.c			\
			ft_strmapi.c			\
			ft_substr.c				\
			ft_putchar_fd.c			\
			ft_putendl_fd.c			\
			ft_putnbr_fd.c			\
			ft_putstr_fd.c			\
			ft_striteri.c			\
			ft_strtrim.c			\
			ft_printf.c				\
			ft_printf_utilities.c	\
			ft_lstnew.c				\
			ft_lstadd_front.c		\
			ft_lstsize.c			\
			ft_lstlast.c			\
			ft_lstadd_back.c		\
			ft_lstdelone.c			\
			ft_lstclear.c			\
			ft_lstiter.c			\
			ft_lstmap.c				\
			get_next_line.c			\
			ft_perrorf.c			\

OBJ		:=	${FILES:.c=.o}


RED		:=	\033[0;31m
GREEN	:=	\033[0;32m
YELLOW	:=	\033[0;33m
WHITE	:=	\033[0m
BOLD	:=	\033[1m
FLASH	:=	\033[6m
RETURN	:=	\033[2K\r

all: $(NAME)

${NAME}: ${OBJ}
	@printf "$(RETURN)$(RED)[COMPILING 🛞 ]$(WHITE) $< :"
	@$(AR) rcs ${NAME} ${OBJ}
	@printf "$(RETURN)$(GREEN)[DONE! ✅]$(WHITE) $@\n"

.c.o:
	@printf "$(RETURN)$(RED)[LINKING 🛞 ]$(WHITE) $@ :"
	@${CC} $(CFLAGS) -c -o $@ $< -g -L.. -lft_malloc_x86_64_Linux
	@printf "$(RETURN)$(GREEN)[DONE! ✅]$(WHITE) $< -> $@\n"

clean:
	@$(RM) $(OBJ)
	@printf "$(YELLOW)[CLEANED] Objects removed.$(WHITE)\n"

fclean: clean
	@$(RM) $(NAME)
	@printf "$(YELLOW)[CLEANED] Lib removed.$(WHITE)\n"

re: clean all
