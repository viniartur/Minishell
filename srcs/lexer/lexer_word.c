/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 18:36:37 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 19:49:18 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	emit_segment(t_tklist lst, t_token_type type, char *buf, int len)
{
	buf[len] = '\0';
	if (len > 0)
		return (create_and_add_token(lst, type, buf, len));
	if (type == TOKEN_QUOTE || type == TOKEN_DQUOTE)
		return (create_and_add_token(lst, type, "", 0));
	return (LEXER_SUCCESS);
}

static int	is_word_break(char c)
{
	return (ft_isspace(c) || c == '|' || c == '<' || c == '>'
		|| c == '$' || c == ';' || c == '&' || c == '\0');
}

static int	handle_quoted(t_lexer *lexer, t_tklist lst, char *buf)
{
	char	quote_start;
	int		len;

	len = 0;
	quote_start = lexer->current;
	advance_lexer(lexer);
	while (lexer->current != '\0' && lexer->current != quote_start)
	{
		buf[len++] = lexer->current;
		advance_lexer(lexer);
	}
	if (lexer->current != quote_start)
	{
		write(STDERR_FILENO, "minishell: syntax error: unclosed quote\n", 40);
		return (LEXER_ERROR);
	}
	advance_lexer(lexer);
	if (quote_start == '\'')
		return (emit_segment(lst, TOKEN_QUOTE, buf, len));
	return (emit_segment(lst, TOKEN_DQUOTE, buf, len));
}

static int	handle_quote_in_word(t_lexer *lexer, t_tklist lst,
		char *buf, int *len)
{
	int	result;

	result = LEXER_SUCCESS;
	if (*len > 0)
		result = emit_segment(lst, TOKEN_WORD, buf, *len);
	*len = 0;
	if (result == LEXER_SUCCESS)
		result = handle_quoted(lexer, lst, buf);
	return (result);
}

int	word_loop(t_lexer *lexer, t_tklist lst, char *buf)
{
	int	len;
	int	result;

	len = 0;
	result = LEXER_SUCCESS;
	while (lexer->current != '\0' && result == LEXER_SUCCESS)
	{
		if (lexer->current == '\'' || lexer->current == '"')
		{
			result = handle_quote_in_word(lexer, lst, buf, &len);
			continue ;
		}
		if (is_word_break(lexer->current))
			break ;
		buf[len++] = lexer->current;
		advance_lexer(lexer);
	}
	if (result == LEXER_SUCCESS && len > 0)
		result = emit_segment(lst, TOKEN_WORD, buf, len);
	return (result);
}
