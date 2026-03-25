/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 16:52:52 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 17:51:59 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	match_token(t_parser *parser, t_token_type type)
{
	if (!parser || !parser->current)
		return (FALSE);
	return (parser->current->type == type);
}

int	should_stop_heredoc(t_token *cur, int count)
{
	if (count > 0 && cur->preceded_by_space)
		return (1);
	return (0);
}

char	*collect_heredoc_piece(t_token *cur, int *expand)
{
	if (cur->type == TOKEN_QUOTE || cur->type == TOKEN_DQUOTE)
		*expand = 0;
	if (cur->type == TOKEN_DOLLAR)
		return (ft_strdup("$"));
	if (cur->value)
		return (ft_strdup(cur->value));
	return (ft_strdup(""));
}

int	append_heredoc_piece(t_parser *parser, char **result,
	t_token *cur, int *expand)
{
	char	*tmp;
	char	*new;

	tmp = collect_heredoc_piece(cur, expand);
	if (!tmp)
		return (0);
	new = join_strings(*result, tmp);
	free(tmp);
	if (!new)
		return (0);
	*result = new;
	advance_parser(parser);
	return (1);
}

char	*collect_heredoc_delim(t_parser *parser, int *out_expand)
{
	t_token	*cur;
	char	*result;
	int		count;

	result = ft_strdup("");
	if (!result)
		return (NULL);
	*out_expand = 1;
	count = 0;
	cur = current_token(parser);
	while (cur && is_arg_token(cur))
	{
		if (should_stop_heredoc(cur, count))
			break ;
		if (!append_heredoc_piece(parser, &result, cur, out_expand))
			return (free(result), NULL);
		count++;
		cur = current_token(parser);
	}
	return (result);
}
