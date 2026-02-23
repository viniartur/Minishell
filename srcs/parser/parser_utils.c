#include "minishell.h"

t_token	*get_next_token(t_token **tokens)
{
	t_token	*current;

	if (!tokens || !*tokens)
		return (NULL);
	current = *tokens;
	*tokens = (*tokens)->next;
	return (current);
}

// verify if current token is of correct type expected
int	match(t_token **tokens, t_token_type type)
{
	if (!tokens || !*tokens)
		return (FALSE);
	return ((*tokens)->type == type);
}

// verify if is consuming token type expected
void	expect(t_token **tokens, t_token_type type)
{
	if (!tokens || !*tokens)
	{
		printf("minishell: syntax error: unexpected EOF\n");
		return ;
	}
	if ((*tokens)->type != type)
	{
		printf("minishell: syntax error near unexpected token\n");
		return ;
	}
	*tokens = (*tokens)->next;
}

// to create new command struct (initiate struct)
t_command	*create_command(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->argv = NULL;
	cmd->argc = 0;
	cmd->redirs = NULL;
	return (cmd);
}

// add argument to command
void	add_argument(t_command *cmd, char *arg)
{
	char	**new_argv;
	int		i;

	if (!cmd || !arg)
		return ;
	new_argv = malloc(sizeof(char *) * (cmd->argc + 2));
	if (!new_argv)
		return ;
	i = 0;
	while (i < cmd->argc)
	{
		new_argv[i] = cmd->argv[i];
		i++;
	}
	new_argv[i] = ft_strdup(arg);
	new_argv[i + 1] = NULL;
	free(cmd->argv);
	cmd->argv = new_argv;
	cmd->argc++;
}

// create/init the new redir struct
t_redir	*create_redirection(int type, char *file)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->file = ft_strdup(file);
	redir->fd = -1; // will be defined during exec
	redir->next = NULL;
	return (redir);
}

// add redir at command list
void	add_redirection(t_command *cmd, t_redir *redir)
{
	t_redir * last;

	if (!cmd || !redir)
		return ;
	if (!cmd->redirs)
	{
		cmd->redirs = redir;
		return ;
	}
	last = cmd->redirs;
	while (last->next)
		last = last->next;
	last->next = redir;
}
