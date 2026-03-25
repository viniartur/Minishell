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
			  srcs/shell/shell_free.c \
			  srcs/shell/shell_utils.c \
			  srcs/utils/utils.c \
			  srcs/utils/signals.c \
			  srcs/utils/utils2.c \
			  srcs/utils/utils3.c \
			  srcs/utils/utils4.c \
			  srcs/utils/prompt.c \
			  srcs/lexer/lexer.c \
			  srcs/lexer/lexer_handle.c \
			  srcs/lexer/lexer_word.c \
			  srcs/lexer/lexer_redir.c \
			  srcs/lexer/lexer_utils.c \
			  srcs/lexer/lexer_utils2.c \
			  srcs/parser/parser.c \
			  srcs/parser/parser_utils.c \
			  srcs/parser/parser_utils2.c \
			  srcs/parser/parser_heredoc.c \
			  srcs/parser/parser_redir.c \
			  srcs/parser/parser_expansion.c \
			  srcs/parser/parser_argument.c \
		      srcs/ast/ast.c \
			  srcs/ast/ast_free.c \
			  srcs/builtins/builtins.c \
			  srcs/builtins/builtins_env.c \
			  srcs/builtins/builtins_env_utils.c \
			  srcs/builtins/builtins_env_utils2.c \
			  srcs/builtins/builtins_nav.c \
			  srcs/builtins/builtins_nav2.c \
			  srcs/var_expansion/expansion.c \
			  srcs/var_expansion/expansion_utils.c \
			  srcs/executor/executor.c \
			  srcs/executor/executor_utils.c \
			  srcs/executor/executor_utils2.c \
			  srcs/executor/executor_utils3.c \
			  srcs/executor/executor_pipeline.c \
			  srcs/executor/executor_heredoc.c \
			  srcs/executor/executor_heredoc_utils.c \
			  srcs/executor/executor_heredoc_utils2.c 

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

val: all
	@valgrind -q \
		--leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		--track-fds=yes \
		--suppressions=$(PWD)/readline.supp \
		./minishell

.PHONY: all clean fclean re val
