#include "minishell.h"

static t_ast_node	*parse_pipeline(t_token **tokens);
static t_ast_node	*parse_command(t_token **tokens);
static int			number_of_redirections(t_token **tokens, t_command *cmd);
static t_redir		*parse_redirection(t_token **tokens);

//  redirection → '<' WORD | '>' WORD | '>>' WORD | '<<' WORD
static t_redir	*parse_redirection(t_token **tokens)
{
	t_token	*redir_token;
	t_token	*file_token;
	t_redir	*redir;

	if (!tokens || !*tokens)
		return (NULL);
	redir_token = *tokens;
	if (redir_token->type != TOKEN_REDIR_IN && redir_token->type != TOKEN_REDIR_OUT &&
	redir_token->type != TOKEN_APPEND && redir_token->type != TOKEN_HEREDOC)
		return (NULL);
	*tokens = (*tokens)->next;
	if (!*tokens || (*tokens)->type != TOKEN_WORD)
	{
		printf("minishell: syntax error near redirection\n");
		return (NULL);
	}
	file_token = *tokens;
	*tokens = (*tokens)->next;
	redir = create_redirection(redir_token->type, file_token->value);
	return (redir);
}

// return number of redirs founded
static int number_of_redirections(t_token **tokens, t_command *cmd)
{
	int		count;
	t_redir	*redir;

	count = 0;
	while (*tokens && ((*tokens)->type == TOKEN_REDIR_IN ||
(*tokens)->type == TOKEN_REDIR_OUT || (*tokens)->type == TOKEN_APPEND ||
(*tokens)->type == TOKEN_HEREDOC))
	{
		redir = parse_redirection(tokens);
		if (!redir)
			return (-1);
		add_redirection(cmd, redir);
		count++;
	}
	return (count);
}

// simple_command → (WORD)* (redirection)*
static t_ast_node	*parse_command(t_token **tokens)
{
	t_command	*cmd;
	t_ast_node	*node;

	cmd = create_command();
	if (!cmd)
		return (NULL);
	while (*tokens && (*tokens)->type != TOKEN_PIPE
		&& (*tokens)->type != TOKEN_EOF)
	{
		if ((*tokens)->type == TOKEN_WORD)
		{
			add_argument(cmd, (*tokens)->value);
			*tokens = (*tokens)->next;
		}
		else if ((*tokens)->type == TOKEN_REDIR_IN
			|| (*tokens)->type == TOKEN_REDIR_OUT
			|| (*tokens)->type == TOKEN_APPEND
			|| (*tokens)->type == TOKEN_HEREDOC)
		{
			if (number_of_redirections(tokens, cmd) == -1)
			{
				free_command(cmd);
				return (NULL);
			}
		}
		else
			break ;
	}
	if (cmd->argc == 0 && !cmd->redirs)
	{
		free_command(cmd);
		return (NULL);
	}
	node = create_command_node(cmd);
	if (!node)
		free_command(cmd);
	return (node);
}

static t_ast_node	*parse_pipeline(t_token **tokens)
{
	t_ast_node	*left;
	t_ast_node	*right;

	if (!tokens || !*tokens)
		return (NULL);
	left = parse_command(tokens); // first command
	if (!left)
		return (NULL);
	while (*tokens && (*tokens)->type == TOKEN_PIPE) // if still has pipes, continue
	{
		*tokens = (*tokens)->next;
		right = parse_command(tokens);
		if (!right)
		{
			free_ast(left);
			return (NULL);
		}
		left = create_pipeline_node(left, right);
	}
	return (left);
}

t_ast_node	*parse(t_token *tokens)
{
	t_ast_node	*ast;

	if (!tokens)
		return (NULL);
	printf("\nPARSER: Initializing parsing...\n");
	ast = parse_pipeline(&tokens);
	if (tokens && tokens->type != TOKEN_EOF)
	{
		printf("minishell: syntax error: unexpected token\n");
		free_ast(ast);
		return (NULL);
	}
	if (ast)
		printf("PARSER: successfully concluded\n");
	else
		printf("PARSER: Error on parsing\n");
	return (ast);
}
