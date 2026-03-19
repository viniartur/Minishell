/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 18:00:00 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/19 15:41:46 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

typedef struct s_parser
{
	t_token	*tokens;
	t_token	*current;
	t_shell	*shell;
}	t_parser;

/* Protótipos das funções estáticas */
static void		advance_parser(t_parser *parser);
static t_token	*current_token(t_parser *parser);
static int		match_token(t_parser *parser, t_token_type type);
static char		*expand_token(t_parser *parser, t_token *tok);
static char		*expand_dollar(t_parser *parser);
static int		is_arg_token(t_token *tok);
static char		*collect_argument(t_parser *parser);
static char		*collect_heredoc_delim(t_parser *parser, int *out_expand);
static t_ast_node	*parse_command_list(t_parser *parser);
static t_ast_node	*parse_pipeline(t_parser *parser);
static t_ast_node	*parse_command(t_parser *parser);
static t_ast_node	*parse_simple_command(t_parser *parser);
static int		parse_redirections(t_parser *parser, t_command *cmd);

/* Implementações das funções */

static void	advance_parser(t_parser *parser)
{
	if (parser && parser->current)
		parser->current = parser->current->next;
}

static t_token	*current_token(t_parser *parser)
{
	if (!parser)
		return (NULL);
	return (parser->current);
}

static int	match_token(t_parser *parser, t_token_type type)
{
	if (!parser || !parser->current)
		return (FALSE);
	return (parser->current->type == type);
}

static char	*expand_token(t_parser *parser, t_token *tok)
{
	if (!tok || !tok->value)
		return (ft_strdup(""));
	if (tok->type == TOKEN_QUOTE)
		return (ft_strdup(tok->value));
	return (expand_all_variables(parser->shell, tok->value));
}

static char	*expand_dollar(t_parser *parser)
{
	t_token	*tok;
	char	*value;

	tok = current_token(parser);
	if (!tok || tok->type == TOKEN_EOF)
		return (ft_strdup("$"));
	if (tok->type == TOKEN_DOLLAR)
	{
		advance_parser(parser);
		return (pid_to_str(getpid()));
	}
	if (tok->type == TOKEN_WORD && tok->value
		&& tok->value[0] == '?' && tok->value[1] == '\0')
	{
		advance_parser(parser);
		return (expand_exit_status(parser->shell));
	}
	if (tok->type == TOKEN_WORD && tok->value)
	{
		value = get_env_value(parser->shell, tok->value);
		advance_parser(parser);
		if (value)
			return (ft_strdup(value));
		return (ft_strdup(""));
	}
	return (ft_strdup("$"));
}

t_ast_node	*parse(t_token *tokens, t_shell *shell)
{
	t_ast_node	*ast;
	t_parser	parser;

	if (!tokens || !shell)
		return (NULL);
	parser.tokens = tokens;
	parser.current = tokens;
	parser.shell = shell;
	ast = parse_command_list(&parser);
	if (ast && parser.current && parser.current->type != TOKEN_EOF)
	{
		printf("minishell: syntax error: unexpected token\n");
		free_ast(ast);
		return (NULL);
	}
	return (ast);
}

static t_ast_node	*parse_command_list(t_parser *parser)
{
	t_ast_node	*left;
	t_ast_node	*right;

	if (!parser || !parser->current)
		return (NULL);
	left = parse_pipeline(parser);
	if (!left)
		return (NULL);
	while (match_token(parser, TOKEN_SEMICOLON))
	{
		advance_parser(parser);
		if (!parser->current || parser->current->type == TOKEN_EOF)
			break ;
		right = parse_pipeline(parser);
		if (!right)
		{
			free_ast(left);
			return (NULL);
		}
		left = create_pipeline_node(left, right);
	}
	return (left);
}

static t_ast_node	*parse_pipeline(t_parser *parser)
{
	t_ast_node	*left;
	t_ast_node	*right;

	if (!parser || !parser->current)
		return (NULL);
	left = parse_command(parser);
	if (!left)
		return (NULL);
	while (match_token(parser, TOKEN_PIPE))
	{
		advance_parser(parser);
		right = parse_command(parser);
		if (!right)
		{
			free_ast(left);
			return (NULL);
		}
		left = create_pipeline_node(left, right);
	}
	return (left);
}

static t_ast_node	*parse_command(t_parser *parser)
{
	t_ast_node	*node;
	t_command	*cmd;
	t_token		*cur;

	node = parse_simple_command(parser);
	cur = current_token(parser);
	if (!node && cur && (cur->type == TOKEN_REDIR_IN
			|| cur->type == TOKEN_REDIR_OUT
			|| cur->type == TOKEN_APPEND
			|| cur->type == TOKEN_HEREDOC))
	{
		cmd = create_command();
		if (!cmd)
			return (NULL);
		node = create_command_node(cmd);
		if (!node)
			return (free_command(cmd), NULL);
	}
	if (!node || node->type != NODE_COMMAND)
		return (node);
	cmd = node->data.cmd;
	if (!parse_redirections(parser, cmd))
	{
		free_ast(node);
		return (NULL);
	}
	return (node);
}

static int	is_arg_token(t_token *tok)
{
	if (!tok)
		return (0);
	return (tok->type == TOKEN_WORD
		|| tok->type == TOKEN_DQUOTE
		|| tok->type == TOKEN_QUOTE
		|| tok->type == TOKEN_DOLLAR);
}

/* ===== FUNÇÃO COLLECT_ARGUMENT CORRIGIDA ===== */
/* ===== FUNÇÃO CORRIGIDA ===== */
static char	*collect_argument(t_parser *parser)
{
	char	*result;
	char	*piece;
	t_token	*current;
	int		count;

	result = ft_strdup("");
	if (!result)
		return (NULL);
	
	count = 0;
	current = current_token(parser);
	while (current && is_arg_token(current))
	{
		if (count > 0 && current->preceded_by_space)
			break ;
		
		if (current->type == TOKEN_DOLLAR)
		{
			advance_parser(parser);
			piece = expand_dollar(parser);
		}
		else
		{
			piece = expand_token(parser, current);
			advance_parser(parser);
		}
		
		if (!piece)
		{
			free(result);
			return (NULL);
		}
		
		/* Se piece for vazio, não adiciona */
		if (piece[0] != '\0')
		{
			result = join_strings(result, piece);
			free(piece);
		}
		else
		{
			free(piece);
		}
		
		if (!result)
			return (NULL);
		
		count++;
		current = current_token(parser);
	}
	
	/* Se não coletou nada, libera e retorna NULL */
	if (result[0] == '\0' && count == 0)
	{
		free(result);
		return (NULL);
	}
	
	return (result);
}

static t_ast_node	*parse_simple_command(t_parser *parser)
{
	t_command	*cmd;
	t_token		*current;
	char		*arg;

	cmd = create_command();
	if (!cmd)
		return (NULL);
	current = current_token(parser);
	while (current && is_arg_token(current))
	{
		arg = collect_argument(parser);
		if (arg)
			add_argument(cmd, arg);
		current = current_token(parser);
	}
	if (cmd->argc == 0 && !cmd->redirs)
	{
		free_command(cmd);
		return (NULL);
	}
	return (create_command_node(cmd));
}

/* ===== FUNÇÃO COLLECT_HEREDOC_DELIM CORRIGIDA ===== */
static char	*collect_heredoc_delim(t_parser *parser, int *out_expand)
{
	t_token	*cur;
	char	*result;
	char	*tmp;
	int		count;

	result = ft_strdup("");
	if (!result)
		return (NULL);
	*out_expand = 1;
	count = 0;
	cur = current_token(parser);
	while (cur && is_arg_token(cur))
	{
		if (count > 0 && cur->preceded_by_space)
			break ;
		if (cur->type == TOKEN_QUOTE || cur->type == TOKEN_DQUOTE)
			*out_expand = 0;
		if (cur->type == TOKEN_DOLLAR)
			tmp = ft_strdup("$");
		else if (cur->value)
			tmp = ft_strdup(cur->value);
		else
			tmp = ft_strdup("");
		if (!tmp)
		{
			free(result);
			return (NULL);
		}
		result = join_strings(result, tmp);
		free(tmp);
		tmp = NULL;
		if (!result)
			return (NULL);
		advance_parser(parser);
		count++;
		cur = current_token(parser);
	}
	return (result);
}

static int	parse_redirections(t_parser *parser, t_command *cmd)
{
	t_token	*current;
	t_redir	*redir;
	int		type;
	int		expand;
	char	*file;

	current = current_token(parser);
	while (current
		&& (current->type == TOKEN_REDIR_IN
			|| current->type == TOKEN_REDIR_OUT
			|| current->type == TOKEN_APPEND
			|| current->type == TOKEN_HEREDOC))
	{
		type = (int)current->type;
		advance_parser(parser);
		current = current_token(parser);
		if (!current || !is_arg_token(current))
		{
			printf("minishell: syntax error near redirection\n");
			return (0);
		}
		if (type == (int)TOKEN_HEREDOC)
			file = collect_heredoc_delim(parser, &expand);
		else
		{
			expand = 1;
			file = collect_argument(parser);
		}
		redir = create_redirection(type, file, expand);
		if (!redir)
			return (0);
		add_redirection(cmd, redir);
		current = current_token(parser);
	}
	return (1);
}
