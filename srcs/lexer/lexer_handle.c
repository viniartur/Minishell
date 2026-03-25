/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_handle.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 18:11:43 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 19:49:27 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	handle_pipe(t_lexer *lexer, t_token **tokens, t_token **last)
{
	t_tklist	lst;

	lst = (t_tklist){tokens, last};
	if (create_and_add_token(lst, TOKEN_PIPE, NULL, 1) == LEXER_ERROR)
		return (LEXER_ERROR);
	advance_lexer(lexer);
	lexer->start = lexer->pos;
	return (LEXER_SUCCESS);
}

int	handle_word(t_lexer *lexer, t_token **tokens, t_token **last)
{
	char		*buf;
	int			result;
	t_tklist	lst;

	buf = malloc(lexer->input_len + 1);
	if (!buf)
		return (LEXER_ERROR);
	lst = (t_tklist){tokens, last};
	result = word_loop(lexer, lst, buf);
	free(buf);
	return (result);
}

int	handle_semicolon(t_lexer *lexer, t_token **tokens, t_token **last)
{
	t_tklist	lst;

	lst = (t_tklist){tokens, last};
	if (create_and_add_token(lst, TOKEN_SEMICOLON, NULL, 1) == LEXER_ERROR)
		return (LEXER_ERROR);
	advance_lexer(lexer);
	return (LEXER_SUCCESS);
}

int	handle_ampersand(t_lexer *lexer, t_token **tokens, t_token **last)
{
	t_tklist	lst;

	lst = (t_tklist){tokens, last};
	if (create_and_add_token(lst, TOKEN_AMPERSAND, NULL, 1) == LEXER_ERROR)
		return (LEXER_ERROR);
	advance_lexer(lexer);
	return (LEXER_SUCCESS);
}
