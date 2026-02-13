NAME		= minishell

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -g
RM			= rm -f

INC_DIR		= includes

# Configuração readline para Ubuntu/WSL
RL_INC		= -I/usr/include/readline
RL_LIB		= -lreadline

# Arquivos fonte
SRCS		= main.c \
			  srcs/shell/shell.c \
			  srcs/utils/utils.c \
			  srcs/utils/prompt.c \
			  srcs/parser/lexer.c \
			  srcs/parser/lexer_utils.c

OBJS		= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@echo "🔗 Linking $(NAME)..."
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(RL_LIB)
	@echo "✅ Minishell compiled successfully!"

%.o: %.c
	@echo "📦 Compiling $<..."
	@$(CC) $(CFLAGS) $(RL_INC) -I$(INC_DIR) -c $< -o $@

clean:
	@echo "🧹 Cleaning object files..."
	@$(RM) $(OBJS)

fclean: clean
	@echo "🧹 Removing executable..."
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
