/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_redir.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 18:36:37 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 19:49:34 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_dollar_var(t_lexer *lx, t_tklist lst)
{
	char	*buf;
	int		len;
	int		result;

	buf = malloc(lx->input_len + 1);
	if (!buf)
		return (LEXER_ERROR);
	len = 0;
	while (lx->current != '\0' && is_valid_var_char_lexer(lx->current))
	{
		buf[len++] = lx->current;
		advance_lexer(lx);
	}
	buf[len] = '\0';
	if (len > 0)
		result = create_and_add_token(lst, TOKEN_WORD, buf, len);
	else
		result = LEXER_SUCCESS;
	free(buf);
	return (result);
}

int	handle_dollar(t_lexer *lx, t_token **tokens, t_token **last)
{
	t_tklist	lst;

	lst = (t_tklist){tokens, last};
	if (create_and_add_token(lst, TOKEN_DOLLAR, NULL, 1) == LEXER_ERROR)
		return (LEXER_ERROR);
	advance_lexer(lx);
	lx->start = lx->pos;
	if (lx->current == '?')
	{
		advance_lexer(lx);
		return (create_and_add_token(lst, TOKEN_WORD, "?", 1));
	}
	return (handle_dollar_var(lx, lst));
}

static t_token_type	get_redir_type(t_lexer *lexer, int *len)
{
	if (lexer->current == '<')
	{
		if (peek_lexer(lexer, 1) == '<')
		{
			*len = 2;
			return (TOKEN_HEREDOC);
		}
		*len = 1;
		return (TOKEN_REDIR_IN);
	}
	if (peek_lexer(lexer, 1) == '>')
	{
		*len = 2;
		return (TOKEN_APPEND);
	}
	*len = 1;
	return (TOKEN_REDIR_OUT);
}

int	handle_redirection(t_lexer *lexer, t_token **tokens, t_token **last)
{
	t_token_type	type;
	int				len;
	t_tklist		lst;

	lst = (t_tklist){tokens, last};
	type = get_redir_type(lexer, &len);
	if (create_and_add_token(lst, type, NULL, len) == LEXER_ERROR)
		return (LEXER_ERROR);
	advance_lexer(lexer);
	if (len == 2)
		advance_lexer(lexer);
	lexer->start = lexer->pos;
	return (LEXER_SUCCESS);
}
