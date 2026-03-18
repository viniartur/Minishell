/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 18:02:24 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/10 18:02:42 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_lexer	*init_lexer(const char *input)
{
	t_lexer	*lexer;

	lexer = malloc(sizeof(t_lexer));
	if (!lexer)
		return (NULL);
	lexer->input = input;
	lexer->input_len = ft_strlen(input);
	lexer->pos = 0;
	lexer->start = 0;
	lexer->current = '\0';
	lexer->in_quote = 0;
	lexer->error = FALSE;
	lexer->had_space = 0;
	if (input && input[0])
		lexer->current = input[0];
	return (lexer);
}

void	advance_lexer(t_lexer *lexer)
{
	lexer->pos++;
	if (lexer->pos >= (int)lexer->input_len)
		lexer->current = '\0';
	else
		lexer->current = lexer->input[lexer->pos];
}

char	peek_lexer(t_lexer *lexer, int offset)
{
	int	peek_pos;

	peek_pos = lexer->pos + offset;
	if (peek_pos < 0 || !lexer->input)
		return ('\0');
	if (peek_pos >= (int)lexer->input_len)
		return ('\0');
	return (lexer->input[peek_pos]);
}

t_token	*create_token(t_token_type type, const char *value, int len)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->len = len;
	token->preceded_by_space = 0;
	token->next = NULL;
	if ((type == TOKEN_WORD || type == TOKEN_QUOTE || type == TOKEN_DQUOTE)
		&& value && len > 0)
	{
		token->value = ft_substr(value, 0, len);
		if (!token->value)
		{
			free(token);
			return (NULL);
		}
	}
	else if ((type == TOKEN_QUOTE || type == TOKEN_DQUOTE) && value && len == 0)
		token->value = ft_strdup("");
	else
		token->value = NULL;
	return (token);
}

void	skip_whitespace(t_lexer *lexer)
{
	if (lexer->in_quote != 0)
		return ;
	lexer->had_space = 0;
	while (ft_isspace(lexer->current))
	{
		lexer->had_space = 1;
		advance_lexer(lexer);
	}
	lexer->start = lexer->pos;
}
