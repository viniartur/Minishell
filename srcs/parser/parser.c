/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 16:46:12 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 17:48:31 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_ast_node	*parse_command(t_parser *parser)
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
		return (free_ast(node), NULL);
	return (node);
}

t_ast_node	*parse_simple_command(t_parser *parser)
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
		return (free_command(cmd), NULL);
	return (create_command_node(cmd));
}

t_ast_node	*parse_pipeline(t_parser *parser)
{
	t_ast_node	*left;
	t_ast_node	*right;

	left = parse_command(parser);
	if (!left)
		return (NULL);
	while (match_token(parser, TOKEN_PIPE))
	{
		advance_parser(parser);
		right = parse_command(parser);
		if (!right)
			return (free_ast(left), NULL);
		left = create_pipeline_node(left, right);
	}
	return (left);
}

t_ast_node	*parse_command_list(t_parser *parser)
{
	t_ast_node	*left;
	t_ast_node	*right;

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
			return (free_ast(left), NULL);
		left = create_pipeline_node(left, right);
	}
	return (left);
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
