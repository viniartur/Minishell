/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 16:49:52 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 17:02:17 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*join_piece(char *result, char *piece)
{
	char	*tmp;

	if (!piece)
		return (NULL);
	if (piece[0] == '\0')
	{
		free(piece);
		return (result);
	}
	tmp = join_strings(result, piece);
	free(piece);
	return (tmp);
}

t_token	*get_next_token(t_token **tokens)
{
	t_token	*current;

	if (!tokens || !*tokens)
		return (NULL);
	current = *tokens;
	*tokens = (*tokens)->next;
	return (current);
}

void	add_redirection(t_command *cmd, t_redir *redir)
{
	t_redir	*last;

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

void	advance_parser(t_parser *parser)
{
	if (parser && parser->current)
		parser->current = parser->current->next;
}

t_token	*current_token(t_parser *parser)
{
	if (!parser)
		return (NULL);
	return (parser->current);
}
