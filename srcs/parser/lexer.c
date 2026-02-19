#include "minishell.h"

static void		handle_quotes(t_lexer *lexer);
static int		handle_operator(t_lexer *l, t_token **tks, t_token **last);
static int		handle_word(t_lexer *lexer, t_token **tks, t_token **last);
static int		add_token(t_token **tks, t_token **last,
					t_token_type type, const char *val, int len);

static void	handle_quotes(t_lexer *lexer)
{
	char	quote_type;

	quote_type = lexer->current;
	if (lexer->in_quote == 0)
	{
		lexer->in_quote = (quote_type == '\'') ? 1 : 2;
		advance_lexer(lexer);
	}
	else if ((lexer->in_quote == 1 && quote_type == '\'')
		|| (lexer->in_quote == 2 && quote_type == '"'))
	{
		lexer->in_quote = 0;
		advance_lexer(lexer);
	}
	else
		advance_lexer(lexer);
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

static int	handle_operator(t_lexer *lexer, t_token **tokens, t_token **last)
{
	t_token_type	type;
	int				len;

	len = 1;
	if (lexer->current == '|')
		type = TOKEN_PIPE;
	else if (lexer->current == '$')
		type = TOKEN_DOLLAR;
	else
		type = get_redir_type(lexer, &len);
	if (add_token(tokens, last, type, NULL, len) == LEXER_ERROR)
		return (LEXER_ERROR);
	while (len--)
		advance_lexer(lexer);
	lexer->start = lexer->pos;
	return (LEXER_SUCCESS);
}

static int is_word_end(t_lexer *lexer)
{
	if (lexer->in_quote != 0)
		return (0);
	if (lexer->current == '\0' || ft_isspace(lexer->current))
		return (1);
	if (lexer->current == '|' || lexer->current == '<'
		|| lexer->current == '>' || lexer->current == '$')
		return (1);
	return (0);
}

static int	handle_word(t_lexer *lexer, t_token **tokens, t_token **last)
{
	int	start;

	start = lexer->pos;
	while (lexer->current != '\0')
	{
		if (lexer->current == '\'' || lexer->current == '"')
		{
			handle_quotes(lexer);
			continue ;
		}
		if (lexer->in_quote == 0 && is_word_end(lexer))
			break ;
		advance_lexer(lexer);
	}
	if (lexer->pos > start)
	{
		if (add_token(tokens, last, TOKEN_WORD,
				&lexer->input[start], lexer->pos - start) == LEXER_ERROR)
			return (LEXER_ERROR);
	}
	return (LEXER_SUCCESS);
}

static void	add_to_list(t_token **tokens, t_token **last, t_token *new)
{
	if (!new)
		return ;
	if (!*tokens)
	{
		*tokens = new;
		*last = new;
	}
	else
	{
		(*last)->next = new;
		*last = new;
	}
}

static int	add_token(t_token **tokens, t_token **last,
				t_token_type type, const char *value, int len)
{
	t_token	*new_token;

	new_token = create_token(type, value, len);
	if (!new_token)
		return (LEXER_ERROR);
	add_to_list(tokens, last, new_token);
	return (LEXER_SUCCESS);
}

static int	is_operator(char c)
{
	return (c == '|' || c == '<' || c == '>' || c == '$');
}

static int	process_token(t_lexer *lexer, t_token **tokens, t_token **last)
{
	if (lexer->current == '\'' || lexer->current == '"')
	{
		handle_quotes(lexer);
		return (LEXER_SUCCESS);
	}
	if (is_operator(lexer->current))
		return (handle_operator(lexer, tokens, last));
	return (handle_word(lexer, tokens, last));
}

static int	finalize_tokens(t_lexer *lexer, t_token **tokens, t_token **last)
{
	if (lexer->in_quote != 0)
	{
		write(STDERR_FILENO, "minishell: syntax error: unclosed quote\n", 41);
		return (LEXER_ERROR);
	}
	if (add_token(tokens, last, TOKEN_EOF, NULL, 0) == LEXER_ERROR)
		return (LEXER_ERROR);
	return (LEXER_SUCCESS);
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
	while (lexer->current != '\0' && !lexer->error)
	{
		skip_whitespace(lexer);
		if (lexer->current == '\0')
			break ;
		if (process_token(lexer, &tokens, &last) == LEXER_ERROR)
			lexer->error = TRUE;
	}
	if (!lexer->error && finalize_tokens(lexer, &tokens, &last) == LEXER_ERROR)
		lexer->error = TRUE;
	has_error = lexer->error;
	free(lexer);
	if (has_error)
		return (free_tokens(tokens), NULL);
	return (tokens);
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

static const char	*get_token_name(t_token_type type)
{
	static const char	*names[] = {
		"EOF", "ERROR", "PIPE", "REDIR_IN", "REDIR_OUT",
		"HEREDOC", "APPEND", "QUOTE", "DQUOTE", "DOLLAR",
		"WORD", "SPACE"
	};

	return (names[type]);
}

void	print_tokens(t_token *tokens)
{
	printf("=== TOKENS ===\n");
	while (tokens)
	{
		printf("[%s", get_token_name(tokens->type));
		if (tokens->value)
			printf(": '%s'", tokens->value);
		printf("]\n");
		tokens = tokens->next;
	}
	printf("==============\n");
}
