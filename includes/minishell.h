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
# define PARSER_SUCCESS 0
# define PARSER_ERROR 1

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
	TOKEN_SPACE,
	TOKEN_SEMICOLON,
	TOKEN_AMPERSAND, // &
	TOKEN_ASSIGN
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	int				len;
	int				preceded_by_space; /* 1 se havia espaco antes deste token */
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
	int			had_space; /* 1 se skip_whitespace avancou antes do token atual */
}	t_lexer;

typedef struct s_redir
{
	int				type; 	// REDIR_IN, REDIR_OUT, APPEND, HEREDOC
	char			*file; 	// File or delimitator
	int				fd; 	// File descriptor for excec
	struct s_redir	*next; 	// next redir
}	t_redir;

typedef enum e_node_type
{
	NODE_PIPELINE,
	NODE_COMMAND,
	NODE_REDIR,
}	t_node_type;

typedef struct s_command	// command built in or external
{
	char	**argv;		// array of arguments
	int		argc;		// number of arguments
	t_redir	*redirs;	//redirect list
}	t_command;

typedef struct s_ast_node
{
	t_node_type		type;	// node type
	union
	{
		// for NODE_PIPELINE
		struct
		{
			struct s_ast_node	*left;	// firts command
			struct s_ast_node	*right;	// rest of pipeline
		}	pipeline;
		// for NODE_COMMAND
		t_command	*cmd;
		// for NODE_REDIR
		t_redir		*redir;
	}	data;
}	t_ast_node;


typedef struct s_shell
{
	char		**env;
	int			exit_status;
	int			should_exit;
	t_token		*tokens;
	t_ast_node	*ast;
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

/* parser.c */
t_ast_node	*parse(t_token *tokens, t_shell *shell);
void		free_ast(t_ast_node *ast);
void		print_ast(t_ast_node *ast, int level);  /* Para debug */

/* parser_utils.c */
t_token		*get_next_token(t_token **tokens);
int			match(t_token **tokens, t_token_type type);
void		expect(t_token **tokens, t_token_type type);
t_command	*create_command(void);
void		add_argument(t_command *cmd, char *arg);
void		add_redirection(t_command *cmd, t_redir *redir);
t_redir		*create_redirection(int type, char *file);

/* ast.c */
t_ast_node	*create_ast_node(t_node_type type);
t_ast_node	*create_command_node(t_command *cmd);
t_ast_node	*create_pipeline_node(t_ast_node *left, t_ast_node *right);
void		free_redirs(t_redir *redirs);
void		free_command(t_command *cmd);

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

/* utils.c - Novas funções */
char	**ft_copy_env(char **envp);
char	*ft_strjoin(char const *s1, char const *s2);
char	**ft_split(char const *s, char c);

/* motor / executor_utils.c */
char	*get_command_path(char *cmd, char **env);

/* srcs/builtins/ */
int     is_builtin(char *cmd);
int     exec_builtin(t_command *cmd, t_shell *shell);
int     builtin_echo(t_command *cmd);
int     builtin_cd(t_command *cmd, t_shell *shell);
int     builtin_pwd(void);
int     builtin_env(t_shell *shell);
int     builtin_export(t_command *cmd, t_shell *shell);
int     builtin_unset(t_command *cmd, t_shell *shell);
int     builtin_exit(t_command *cmd, t_shell *shell);

/* expanção variáveis */
char	*expand_all_variables(t_shell *shell, const char *str);
char	*expand_variable(t_shell *shell, const char *str, int *i);
char	*expand_exit_status(t_shell *shell);
char	*get_env_value(t_shell *shell, const char *var_name);

/* expanção utils */
int		is_valid_var_char(char c);
char	*extract_var_name(const char *str, int start);
char	*join_strings(char *s1, char *s2);
void	free_split_result(char **split);

#endif
