#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <signal.h>
# include <sys/wait.h>
# include <string.h>
# include <fcntl.h>
# include <errno.h>

# define PROMPT "minishell$ "
# define EXIT_SUCCESS 0
# define EXIT_FAILURE 1
# define TRUE 1
# define FALSE 0
# define LEXER_SUCCESS 0
# define LEXER_ERROR 1

typedef enum e_token_type
{
	TOKEN_EOF,
	TOKEN_ERROR,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_HEREDOC,
	TOKEN_APPEND,
	TOKEN_QUOTE,
	TOKEN_DQUOTE,
	TOKEN_DOLLAR,
	TOKEN_WORD,
	TOKEN_SPACE
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	int				len;
	struct s_token	*next;
}	t_token;

typedef struct s_lexer
{
	const char	*input;
	size_t		input_len;
	int			pos;
	int			start;
	char		current;
	int			in_quote;
	int			error;
}	t_lexer;

typedef struct s_shell
{
	char	**env;
	int		exit_status;
	t_token	*tokens;
}	t_shell;

extern volatile sig_atomic_t	g_signal;

/* main.c */
int		main(int argc, char **argv, char **envp);

/* shell.c */
void	init_shell(t_shell *shell, char **envp);
void	main_loop(t_shell *shell);
void	cleanup_shell(t_shell *shell);
void	handle_input(t_shell *shell, char *input);

/* prompt.c */
void	display_prompt(void);
char	*get_user_input(void);
void	add_to_history(char *input);

/* lexer.c */
t_token	*tokenize(const char *input);
void	free_tokens(t_token *tokens);
void	print_tokens(t_token *tokens);

/* lexer_utils.c */
t_lexer	*init_lexer(const char *input);
void	advance_lexer(t_lexer *lexer);
char	peek_lexer(t_lexer *lexer, int offset);
t_token	*create_token(t_token_type type, const char *value, int len);
void	skip_whitespace(t_lexer *lexer);

/* utils.c */
char	*ft_strdup(const char *s);
size_t	ft_strlen(const char *s);
void	*ft_memcpy(void *dst, const void *src, size_t n);
void	free_shell(t_shell *shell);
char	*ft_substr(const char *s, int start, int len);
size_t	ft_strcspn(const char *s, const char *reject);
int		ft_strcmp(const char *s1, const char *s2);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
char	*ft_strchr(const char *s, int c);
int		ft_isspace(int c);

#endif
