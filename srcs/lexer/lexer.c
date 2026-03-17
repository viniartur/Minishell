/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 18:10:00 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/10 18:16:05 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_dollar(t_lexer *lexer, t_token **tokens, t_token **last)
{
	char	*buf;
	int		len;
	int		result;

	if (create_and_add_token(tokens, last, TOKEN_DOLLAR, NULL, 1) == LEXER_ERROR)
		return (LEXER_ERROR);
	advance_lexer(lexer);
	lexer->start = lexer->pos;
	if (lexer->current == '?')
	{
		advance_lexer(lexer);
		return (create_and_add_token(tokens, last, TOKEN_WORD, "?", 1));
	}
	buf = malloc(lexer->input_len + 1);
	if (!buf)
		return (LEXER_ERROR);
	len = 0;
	while (lexer->current != '\0' && is_valid_var_char_lexer(lexer->current))
	{
		buf[len++] = lexer->current;
		advance_lexer(lexer);
	}
	buf[len] = '\0';
	if (len > 0)
		result = create_and_add_token(tokens, last, TOKEN_WORD, buf, len);
	else
		result = LEXER_SUCCESS;
	free(buf);
	return (result);
}

static int	handle_redirection(t_lexer *lexer, t_token **tokens, t_token **last)
{
	t_token_type	type;
	int				len;

	if (lexer->current == '<')
	{
		if (peek_lexer(lexer, 1) == '<')
		{
			type = TOKEN_HEREDOC;
			len = 2;
		}
		else
		{
			type = TOKEN_REDIR_IN;
			len = 1;
		}
	}
	else
	{
		if (peek_lexer(lexer, 1) == '>')
		{
			type = TOKEN_APPEND;
			len = 2;
		}
		else
		{
			type = TOKEN_REDIR_OUT;
			len = 1;
		}
	}
	if (create_and_add_token(tokens, last, type, NULL, len) == LEXER_ERROR)
		return (LEXER_ERROR);
	advance_lexer(lexer);
	if (len == 2)
		advance_lexer(lexer);
	lexer->start = lexer->pos;
	return (LEXER_SUCCESS);
}

t_token	*tokenize(const char *input)
{
	t_lexer	*lexer;
	t_token	*tokens;
	t_token	*last;
	t_token	*before;
	t_token	*first_new;
	int		has_error;

	if (!input)
		return (NULL);
	lexer = init_lexer(input);
	if (!lexer)
		return (NULL);
	tokens = NULL;
	last = NULL;
	while (lexer->current != '\0' && !lexer->error)
	{
		skip_whitespace(lexer);
		if (lexer->current == '\0')
			break ;
		before = last;
		if (lexer->current == '|')
			lexer->error = (handle_pipe(lexer, &tokens, &last) == LEXER_ERROR);
		else if (lexer->current == '<' || lexer->current == '>')
			lexer->error = (handle_redirection(lexer, &tokens, &last) == LEXER_ERROR);
		else if (lexer->current == '$')
			lexer->error = (handle_dollar(lexer, &tokens, &last) == LEXER_ERROR);
		else if (lexer->current == ';')
			lexer->error = (handle_semicolon(lexer, &tokens, &last) == LEXER_ERROR);
		else if (lexer->current == '&')
			lexer->error = (handle_ampersand(lexer, &tokens, &last) == LEXER_ERROR);
		else
			lexer->error = (handle_word(lexer, &tokens, &last) == LEXER_ERROR);
		if (!lexer->error)
		{
			first_new = before ? before->next : tokens;
			if (first_new)
				first_new->preceded_by_space = lexer->had_space;
		}
	}
	if (!lexer->error)
		lexer->error = (finalize_tokens(lexer, &tokens, &last) == LEXER_ERROR);
	has_error = lexer->error;
	free(lexer);
	if (has_error)
		return (free_tokens(tokens), NULL);
	return (tokens);
}
