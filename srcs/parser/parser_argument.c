/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_argument.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 16:49:02 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 17:52:33 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_arg_token(t_token *tok)
{
	if (!tok)
		return (0);
	return (tok->type == TOKEN_WORD
		|| tok->type == TOKEN_DQUOTE
		|| tok->type == TOKEN_QUOTE
		|| tok->type == TOKEN_DOLLAR);
}

int	should_stop_arg(t_token *cur, int count)
{
	if (count > 0 && cur->preceded_by_space)
		return (1);
	return (0);
}

char	*get_argument_piece(t_parser *parser, t_token *cur)
{
	char	*res;

	if (cur->type == TOKEN_DOLLAR)
	{
		advance_parser(parser);
		return (expand_dollar(parser));
	}
	res = expand_token(parser, cur);
	advance_parser(parser);
	return (res);
}

char	*finish_argument(char *result, int count)
{
	if (result[0] == '\0' && count == 0)
	{
		free(result);
		return (NULL);
	}
	return (result);
}

char	*collect_argument(t_parser *parser)
{
	char	*result;
	char	*piece;
	t_token	*cur;
	int		count;

	result = ft_strdup("");
	if (!result)
		return (NULL);
	count = 0;
	cur = current_token(parser);
	while (cur && is_arg_token(cur))
	{
		if (should_stop_arg(cur, count))
			break ;
		piece = get_argument_piece(parser, cur);
		if (!piece)
			return (free(result), NULL);
		result = join_piece(result, piece);
		if (!result)
			return (NULL);
		count++;
		cur = current_token(parser);
	}
	return (finish_argument(result, count));
}
