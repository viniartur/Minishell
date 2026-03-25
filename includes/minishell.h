/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 19:55:11 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 20:11:36 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
# include <dirent.h>
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
	TOKEN_AMPERSAND,
	TOKEN_ASSIGN
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	int				len;
	int				preceded_by_space;
	struct s_token	*next;
}	t_token;

typedef struct s_tklist
{
	t_token	**tokens;
	t_token	**last;
}	t_tklist;

typedef struct s_lexer
{
	const char	*input;
	size_t		input_len;
	int			pos;
	int			start;
	char		current;
	int			in_quote;
	int			error;
	int			had_space;
}	t_lexer;

typedef struct s_redir
{
	int				type;
	char			*file;
	char			*content;
	int				expand;
	int				fd;
	struct s_redir	*next;
}	t_redir;

typedef enum e_node_type
{
	NODE_PIPELINE,
	NODE_COMMAND,
	NODE_REDIR,
}	t_node_type;

typedef struct s_command
{
	char	**argv;
	int		argc;
	t_redir	*redirs;
}	t_command;

typedef struct s_ast_node
{
	t_node_type		type;
	union u_ast_data
	{
		struct s_pipeline
		{
			struct s_ast_node	*left;
			struct s_ast_node	*right;
		}	pipeline;
		t_command	*cmd;
		t_redir		*redir;
	}	data;
}	t_ast_node;

typedef struct s_shell
{
	char		**env;
	int			exit_status;
	int			should_exit;
	int			in_child;
	t_token		*tokens;
	t_ast_node	*ast;
}	t_shell;

typedef struct s_parser
{
	t_token	*tokens;
	t_token	*current;
	t_shell	*shell;
}	t_parser;

extern volatile sig_atomic_t	g_signal;

int			main(int argc, char **argv, char **envp);
void		close_inherited_fds(void);
void		init_shell(t_shell *shell, char **envp);
void		main_loop(t_shell *shell);
void		process_command(t_shell *shell, char *input);;
void		cleanup_shell(t_shell *shell);
void		handle_input(t_shell *shell, char *input);
int			handle_ast(t_shell *shell);
int			handle_tokens(t_shell *shell, char *input);
void		free_shell(t_shell *shell);
void		free_child_memory(t_shell *shell);
void		child_exit(t_shell *shell, int status);
void		display_prompt(void);
char		*get_user_input(void);
void		add_to_history(char *input);
t_token		*tokenize(const char *input);
void		free_tokens(t_token *tokens);
void		print_tokens(t_token *tokens);
void		setup_signals(void);
void		handle_signals(int sig);
t_lexer		*init_lexer(const char *input);
void		advance_lexer(t_lexer *lexer);
char		peek_lexer(t_lexer *lexer, int offset);
t_token		*create_token(t_token_type type, const char *value, int len);
void		skip_whitespace(t_lexer *lexer);
int			is_valid_var_char_lexer(char c);
void		add_token_to_list(t_token **tokens, t_token **last,
				t_token *new_token);
int			create_and_add_token(t_tklist lst, t_token_type type,
				const char *value, int len);
int			finalize_tokens(t_lexer *lexer, t_token **tokens, t_token **last);
int			handle_dollar(t_lexer *lexer, t_token **tokens, t_token **last);
int			handle_redirection(t_lexer *lexer, t_token **tokens,
				t_token **last);
int			emit_segment(t_tklist lst, t_token_type type, char *buf, int len);
int			word_loop(t_lexer *lexer, t_tklist lst, char *buf);
int			handle_pipe(t_lexer *lexer, t_token **tokens, t_token **last);
int			handle_word(t_lexer *lexer, t_token **tokens, t_token **last);
int			handle_semicolon(t_lexer *lexer, t_token **tokens, t_token **last);
int			handle_ampersand(t_lexer *lexer, t_token **tokens, t_token **last);
t_ast_node	*parse(t_token *tokens, t_shell *shell);
void		free_ast(t_ast_node *ast);
char		*expand_token(t_parser *parser, t_token *tok);
char		*expand_exit_code(t_parser *parser, t_token *tok);
char		*expand_env_var(t_parser *parser, t_token *tok);
char		*expand_dollar(t_parser *parser);
char		*join_piece(char *result, char *piece);
int			is_arg_token(t_token *tok);
int			should_stop_arg(t_token *cur, int count);
char		*get_argument_piece(t_parser *parser, t_token *cur);
char		*finish_argument(char *result, int count);
char		*collect_argument(t_parser *parser);
int			should_stop_heredoc(t_token *cur, int count);
char		*collect_heredoc_piece(t_token *cur, int *expand);
int			append_heredoc_piece(t_parser *parser, char **result,
				t_token *cur, int *expand);
char		*collect_heredoc_delim(t_parser *parser, int *out_expand);
t_ast_node	*parse_command_list(t_parser *parser);
t_ast_node	*parse_pipeline(t_parser *parser);
t_ast_node	*parse_simple_command(t_parser *parser);
t_ast_node	*parse_command(t_parser *parser);
int			match_token(t_parser *parser, t_token_type type);
t_token		*current_token(t_parser *parser);
void		advance_parser(t_parser *parser);
char		*expand_dollar(t_parser *parser);
char		*expand_token(t_parser *parser, t_token *tok);
t_token		*get_next_token(t_token **tokens);
int			match(t_token **tokens, t_token_type type);
void		expect(t_token **tokens, t_token_type type);
t_command	*create_command(void);
void		add_argument(t_command *cmd, char *arg);
void		add_redirection(t_command *cmd, t_redir *redir);
t_redir		*create_redirection(int type, char *file, int expand);
t_ast_node	*create_ast_node(t_node_type type);
t_ast_node	*create_command_node(t_command *cmd);
t_ast_node	*create_pipeline_node(t_ast_node *left, t_ast_node *right);
void		free_redirs(t_redir *redirs);
void		free_command(t_command *cmd);
char		*ft_strdup(const char *s);
size_t		ft_strlen(const char *s);
void		*ft_memcpy(void *dst, const void *src, size_t n);
char		*ft_substr(const char *s, unsigned int start, size_t len);
size_t		ft_strcspn(const char *s, const char *reject);
int			ft_strcmp(const char *s1, const char *s2);
int			ft_strncmp(const char *s1, const char *s2, size_t n);
char		*ft_strchr(const char *s, int c);
int			ft_isspace(int c);
void		ft_putstr_fd(const char *s, int fd);
char		**ft_copy_env(char **envp);
char		*ft_strjoin(char const *s1, char const *s2);
char		**ft_split(char const *s, char c);
int			is_valid_var_char(char c);
char		*join_strings(char *s1, char *s2);
void		free_split_result(char **split);
char		*get_command_path(char *cmd, char **env);
char		*find_path_variable(char **env);
char		*check_access(char **paths, char *cmd);
int			is_builtin(char *cmd);
int			exec_builtin(t_command *cmd, t_shell *shell);
void		remove_env_entry(t_shell *shell, int index);
void		print_export_sorted(char **env);
void		print_sorted_env(char **env);
void		sort_env(char **env, int count);
char		**copy_env(char **env, int count);
int			env_count(char **env);
int			update_existing_env(t_shell *shell, char *entry, int len);
int			add_new_env(t_shell *shell, char *entry);
int			is_valid_identifier(const char *str);
int			export_strcmp(const char *a, const char *b);
void		unset_single(t_shell *shell, char *var);
void		update_pwd(t_shell *shell, char *oldcwd);
char		*resolve_cd_target(t_command *cmd, t_shell *shell);
int			parse_exit_code(char *arg);
void		update_pwd(t_shell *shell, char *oldcwd);
int			is_numeric_arg(const char *arg);
int			builtin_echo(t_command *cmd);
int			builtin_cd(t_command *cmd, t_shell *shell);
int			builtin_pwd(void);
int			builtin_exit(t_command *cmd, t_shell *shell);
int			builtin_env(t_shell *shell);
int			builtin_export(t_command *cmd, t_shell *shell);
int			builtin_export_single(char *entry, t_shell *shell);
int			builtin_unset(t_command *cmd, t_shell *shell);
int			is_n_flag(const char *arg);
int			is_redirection_token(t_token *tok);
char		*get_redir_file(t_parser *parser, int type, int *expand);
int			add_redir_node(t_command *cmd, int type, char *file, int expand);
int			parse_redirections(t_parser *parser, t_command *cmd);
char		*expand_all_variables(t_shell *shell, const char *str);
char		*expand_variable(t_shell *shell, const char *str, int *i);
char		*expand_exit_status(t_shell *shell);
char		*pid_to_str(int n);
char		*extract_var_name(const char *str, int start);
char		*get_env_value(t_shell *shell, const char *var_name);
char		*int_to_str(int n, int digits);
int			execute_ast(t_ast_node *node, t_shell *shell);
int			execute_command(t_command *cmd, t_shell *shell);
int			execute_pipeline(t_ast_node *node, t_shell *shell);
void		child_left(t_ast_node *node, t_shell *shell, int *fd);
void		child_right(t_ast_node *node, t_shell *shell, int *fd);
int			handle_pipeline_status(int sl, int sr, t_shell *shell);
char		**dup_argv(char **argv, int argc);
void		free_argv(char **argv);
int			handle_redirections(t_redir *redir);
int			execute_builtin_with_redir(t_command *cmd, t_shell *shell);
void		child_sigint_handler(int sig);
int			handle_output_redir(t_redir *r);
int			handle_heredoc_redir(t_redir *r);
void		restore_fds(int in, int out);
void		execve_wrapper(t_command *cmd, t_shell *shell);
void		exec_child(t_command *cmd, t_shell *shell);
int			handle_exit_status(int status, t_shell *shell);
int			handle_append_redir(t_redir *r);
int			handle_input_redir(t_redir *r);
int			collect_heredoc(t_redir *redir, t_shell *shell);
int			prepare_heredocs(t_redir *redir, t_shell *shell);
int			prepare_ast_heredocs(t_ast_node *node, t_shell *shell);
int			init_heredoc(t_redir *redir, int *pipefd);
int			fork_heredoc(int *pipefd, t_redir *redir, t_shell *shell);
int			handle_heredoc_status(int status, t_shell *shell, \
			int *pipefd, t_redir *redir);
int			heredoc_fail(t_shell *shell, int *pipefd, t_redir *redir);
void		read_pipe_content(int fd, t_redir *redir);
void		run_heredoc_child(int *pipefd, t_redir *redir, t_shell *shell);

#endif
