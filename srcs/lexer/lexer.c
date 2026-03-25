/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 18:10:00 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 19:41:31 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	update_space_flag(t_lexer *lexer, t_token *before,
		t_token *tokens)
{
	t_token	*first_new;

	if (before)
		first_new = before->next;
	else
		first_new = tokens;
	if (first_new)
		first_new->preceded_by_space = lexer->had_space;
}

static int	dispatch_token(t_lexer *lexer, t_token **tokens, t_token **last)
{
	if (lexer->current == '|')
		return (handle_pipe(lexer, tokens, last));
	if (lexer->current == '<' || lexer->current == '>')
		return (handle_redirection(lexer, tokens, last));
	if (lexer->current == '$')
		return (handle_dollar(lexer, tokens, last));
	if (lexer->current == ';')
		return (handle_semicolon(lexer, tokens, last));
	if (lexer->current == '&')
		return (handle_ampersand(lexer, tokens, last));
	return (handle_word(lexer, tokens, last));
}

static void	tokenize_loop(t_lexer *lexer, t_token **tokens, t_token **last)
{
	t_token	*before;

	while (lexer->current != '\0' && !lexer->error)
	{
		skip_whitespace(lexer);
		if (lexer->current == '\0')
			break ;
		before = *last;
		lexer->error = (dispatch_token(lexer, tokens, last) == LEXER_ERROR);
		if (!lexer->error)
			update_space_flag(lexer, before, *tokens);
	}
}

t_token	*tokenize(const char *input)
{
	t_lexer	*lexer;
	t_token	*tokens;
	t_token	*last;
	int		has_error;

	if (!input)
		return (NULL);
	lexer = init_lexer(input);
	if (!lexer)
		return (NULL);
	tokens = NULL;
	last = NULL;
	tokenize_loop(lexer, &tokens, &last);
	if (!lexer->error)
		lexer->error = (finalize_tokens(lexer, &tokens, &last) == LEXER_ERROR);
	has_error = lexer->error;
	free(lexer);
	if (has_error)
		return (free_tokens(tokens), NULL);
	return (tokens);
}
