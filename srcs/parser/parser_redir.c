/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 16:58:47 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 17:05:20 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_redirection_token(t_token *tok)
{
	return (tok->type == TOKEN_REDIR_IN
		|| tok->type == TOKEN_REDIR_OUT
		|| tok->type == TOKEN_APPEND
		|| tok->type == TOKEN_HEREDOC);
}

char	*get_redir_file(t_parser *parser, int type, int *expand)
{
	if (type == (int)TOKEN_HEREDOC)
		return (collect_heredoc_delim(parser, expand));
	*expand = 1;
	return (collect_argument(parser));
}

int	add_redir_node(t_command *cmd, int type, char *file, int expand)
{
	t_redir	*redir;

	redir = create_redirection(type, file, expand);
	if (!redir)
		return (0);
	add_redirection(cmd, redir);
	return (1);
}

int	parse_redirections(t_parser *parser, t_command *cmd)
{
	t_token	*cur;
	int		type;
	int		expand;
	char	*file;

	cur = current_token(parser);
	while (cur && is_redirection_token(cur))
	{
		type = (int)cur->type;
		advance_parser(parser);
		cur = current_token(parser);
		if (!cur || !is_arg_token(cur))
			return (printf("minishell: syntax error near redirection\n"), 0);
		file = get_redir_file(parser, type, &expand);
		if (!add_redir_node(cmd, type, file, expand))
			return (0);
		cur = current_token(parser);
	}
	return (1);
}
