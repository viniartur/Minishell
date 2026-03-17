/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_handle.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 18:11:43 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/10 18:15:24 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	handle_pipe(t_lexer *lexer, t_token **tokens, t_token **last)
{
	if (create_and_add_token(tokens, last, TOKEN_PIPE, NULL, 1) == LEXER_ERROR)
		return (LEXER_ERROR);
	advance_lexer(lexer);
	lexer->start = lexer->pos;
	return (LEXER_SUCCESS);
}

int	emit_segment(t_token **tokens, t_token **last, t_token_type type, char *buf, 
	int len)
{
	buf[len] = '\0';
	if (len > 0)
		return (create_and_add_token(tokens, last, type, buf, len));
	if (type == TOKEN_QUOTE || type == TOKEN_DQUOTE)
		return (create_and_add_token(tokens, last, type, "", 0));
	return (LEXER_SUCCESS);
}

int	handle_word(t_lexer *lexer, t_token **tokens, t_token **last)
{
	char			*buf;
	int				len;
	int				result;
	char			quote_start;

	buf = malloc(lexer->input_len + 1);
	if (!buf)
		return (LEXER_ERROR);
	len = 0;
	result = LEXER_SUCCESS;
	while (lexer->current != '\0' && result == LEXER_SUCCESS)
	{
		if (lexer->current == '\'' || lexer->current == '"')
		{
			if (len > 0)
			{
				result = emit_segment(tokens, last, TOKEN_WORD, buf, len);
				len = 0;
				if (result != LEXER_SUCCESS)
					break ;
			}
			quote_start = lexer->current;
			advance_lexer(lexer);
			while (lexer->current != '\0' && lexer->current != quote_start)
			{
				buf[len++] = lexer->current;
				advance_lexer(lexer);
			}
			if (lexer->current != quote_start)
			{
				free(buf);
				write(STDERR_FILENO, "minishell: syntax error: unclosed \
					quote\n", 41);
				return (LEXER_ERROR);
			}
			advance_lexer(lexer);
			result = emit_segment(tokens, last,
					(quote_start == '\'') ? TOKEN_QUOTE : TOKEN_DQUOTE,
					buf, len);
			len = 0;
			continue ;
		}
		if (ft_isspace(lexer->current) || \
			lexer->current == '|' || lexer->current == '<' || \
			lexer->current == '>' || lexer->current == '$' || \
			lexer->current == ';' || lexer->current == '&' || \
			lexer->current == '\0')
			break ;
		buf[len++] = lexer->current;
		advance_lexer(lexer);
	}
	if (result == LEXER_SUCCESS && len > 0)
		result = emit_segment(tokens, last, TOKEN_WORD, buf, len);
	free(buf);
	return (result);
}

int	handle_semicolon(t_lexer *lexer, t_token **tokens, t_token **last)
{
	if (create_and_add_token(tokens, last, TOKEN_SEMICOLON, NULL, 1) \
	== LEXER_ERROR)
		return (LEXER_ERROR);
	advance_lexer(lexer);
	return (LEXER_SUCCESS);
}

int	handle_ampersand(t_lexer *lexer, t_token **tokens, t_token **last)
{
	if (create_and_add_token(tokens, last, TOKEN_AMPERSAND, NULL, 1) \
	== LEXER_ERROR)
		return (LEXER_ERROR);
	advance_lexer(lexer);
	return (LEXER_SUCCESS);
}
