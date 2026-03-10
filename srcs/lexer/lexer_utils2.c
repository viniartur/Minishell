/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 18:05:46 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/10 18:09:23 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_valid_var_char_lexer(char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || \
		(c >= '0' && c <= '9') || c == '_');
}

void	add_token_to_list(t_token **tokens, t_token **last, t_token *new_token)
{
	if (!new_token)
		return ;
	if (!*tokens)
	{
		*tokens = new_token;
		*last = new_token;
	}
	else
	{
		(*last)->next = new_token;
		*last = new_token;
	}
}

int	create_and_add_token(t_token **tokens, t_token **last,
			t_token_type type, const char *value, int len)
{
	t_token	*new_token;

	new_token = create_token(type, value, len);
	if (!new_token)
		return (LEXER_ERROR);
	add_token_to_list(tokens, last, new_token);
	return (LEXER_SUCCESS);
}

int	finalize_tokens(t_lexer *lexer, t_token **tokens, t_token **last)
{
	if (lexer->in_quote != 0)
	{
		write(STDERR_FILENO, "minishell: syntax error: unclosed quote\n", 41);
		return (LEXER_ERROR);
	}
	if (create_and_add_token(tokens, last, TOKEN_EOF, NULL, 0) == LEXER_ERROR)
		return (LEXER_ERROR);
	return (LEXER_SUCCESS);
}

void	free_tokens(t_token *tokens)
{
	t_token	*current;
	t_token	*next;

	current = tokens;
	while (current)
	{
		next = current->next;
		if (current->value)
			free(current->value);
		free(current);
		current = next;
	}
}
