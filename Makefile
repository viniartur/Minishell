<<<<<<< HEAD
NAME    = minishell
CC      = cc
CFLAGS  = -Wall -Wextra -Werror
RM      = rm -f

SRCS    = main.c
OBJS    = $(SRCS:.c=.o)
=======
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
              srcs/shell/executor_utils.c \
              srcs/utils/utils.c \
              srcs/utils/prompt.c \
              srcs/parser/lexer.c \
              srcs/parser/lexer_utils.c \
              srcs/parser/parser.c \
              srcs/parser/parser_utils.c \
              srcs/parser/ast.c \
              srcs/builtins/builtins.c \
              srcs/builtins/builtins_nav.c \
              srcs/builtins/builtins_env.c

OBJS		= $(SRCS:.c=.o)
>>>>>>> main

all: $(NAME)

$(NAME): $(OBJS)
<<<<<<< HEAD
	$(CC) $(CFLAGS) $(OBJS) -lreadline -o $(NAME)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
=======
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
>>>>>>> main
