#include "minishell.h"

static void		handle_quotes(t_lexer *lexer);
static int		handle_dollar(t_lexer *lexer, t_token **tokens, t_token **last);
static int		handle_redirection(t_lexer *lexer, t_token **tokens, t_token **last);
static int		handle_pipe(t_lexer *lexer, t_token **tokens, t_token **last);
static int		handle_word(t_lexer *lexer, t_token **tokens, t_token **last);
static int		handle_semicolon(t_lexer *lexer, t_token **tokens, t_token **last);
static int		handle_ampersand(t_lexer *lexer, t_token **tokens, t_token **last);
static int		handle_assign(t_lexer *lexer, t_token **tokens, t_token **last);
static void		add_token_to_list(t_token **tokens, t_token **last, t_token *new_token);
static int		create_and_add_token(t_token **tokens, t_token **last,
					t_token_type type, const char *value, int len);
static int		finalize_tokens(t_lexer *lexer, t_token **tokens, t_token **last);

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

static int	handle_dollar(t_lexer *lexer, t_token **tokens, t_token **last)
{
	if (create_and_add_token(tokens, last, TOKEN_DOLLAR, NULL, 1) == LEXER_ERROR)
		return (LEXER_ERROR);
	advance_lexer(lexer);
	lexer->start = lexer->pos;
	return (LEXER_SUCCESS);
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

static int	handle_pipe(t_lexer *lexer, t_token **tokens, t_token **last)
{
	if (create_and_add_token(tokens, last, TOKEN_PIPE, NULL, 1) == LEXER_ERROR)
		return (LEXER_ERROR);
	advance_lexer(lexer);
	lexer->start = lexer->pos;
	return (LEXER_SUCCESS);
}

static int	is_literal_quote(t_lexer *lexer)
{
	if (lexer->in_quote == 1 && lexer->current == '"')
		return (1);
	if (lexer->in_quote == 2 && lexer->current == '\'')
		return (1);
	return (0);
}

static int	handle_word(t_lexer *lexer, t_token **tokens, t_token **last)
{
    char    *buf;
    int     len;
    int     result;

    buf = malloc(lexer->input_len + 1); // máximo possível é o input inteiro
    if (!buf)
        return (LEXER_ERROR);
    len = 0;
    while (lexer->current != '\0')
    {
        if (lexer->current == '\'' || lexer->current == '"')
        {
            if (is_literal_quote(lexer))
            {
                buf[len++] = lexer->current;
                advance_lexer(lexer);
            }
            else
                handle_quotes(lexer);
            continue ;
        }
        if (lexer->in_quote != 0)
        {
            buf[len++] = lexer->current;
            advance_lexer(lexer);
            continue ;
        }
        if (ft_isspace(lexer->current) ||
            lexer->current == '|' || lexer->current == '<' ||
            lexer->current == '>' || lexer->current == '$' ||
            lexer->current == ';' || lexer->current == '&' ||
            lexer->current == '=' || lexer->current == '\0')
            break ;
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

static int	handle_semicolon(t_lexer *lexer, t_token **tokens, t_token **last)
{
	if (create_and_add_token(tokens, last, TOKEN_SEMICOLON, NULL, 1) == LEXER_ERROR)
		return (LEXER_ERROR);
	advance_lexer(lexer);
	return (LEXER_SUCCESS);
}


static int	handle_ampersand(t_lexer *lexer, t_token **tokens, t_token **last)
{
	if (create_and_add_token(tokens, last, TOKEN_AMPERSAND, NULL, 1) == LEXER_ERROR)
		return (LEXER_ERROR);
	advance_lexer(lexer);
	return (LEXER_SUCCESS);
}

static int	handle_assign(t_lexer *lexer, t_token **tokens, t_token **last)
{
	if (create_and_add_token(tokens, last, TOKEN_ASSIGN, NULL, 1) == LEXER_ERROR)
		return (LEXER_ERROR);
	advance_lexer(lexer);
	return (LEXER_SUCCESS);
}

static void	add_token_to_list(t_token **tokens, t_token **last, t_token *new_token)
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

static int	create_and_add_token(t_token **tokens, t_token **last,
				t_token_type type, const char *value, int len)
{
	t_token	*new_token;

	new_token = create_token(type, value, len);
	if (!new_token)
		return (LEXER_ERROR);
	add_token_to_list(tokens, last, new_token);
	return (LEXER_SUCCESS);
}

static int	finalize_tokens(t_lexer *lexer, t_token **tokens, t_token **last)
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
		else if (lexer->current == '=')
			lexer->error = (handle_assign(lexer, &tokens, &last) == LEXER_ERROR);
		else
			lexer->error = (handle_word(lexer, &tokens, &last) == LEXER_ERROR);
	}
	if (!lexer->error)
		lexer->error = (finalize_tokens(lexer, &tokens, &last) == LEXER_ERROR);
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

/*
 * Imprime a lista de tokens — útil para debug.
 * Formato: [TIPO: 'valor']
 */
void	print_tokens(t_token *tokens)
{
	static const char	*names[] = {
		"EOF", "ERROR", "PIPE", "REDIR_IN", "REDIR_OUT",
		"HEREDOC", "APPEND", "QUOTE", "DQUOTE", "DOLLAR",
		"WORD", "SPACE", "SEMICOLON", "AMPERSAND", "ASSIGN"
	};

	printf("=== TOKENS ===\n");
	while (tokens)
	{
		if (tokens->type >= 0 && tokens->type <= TOKEN_ASSIGN)
		{
			printf("[%s", names[tokens->type]);
			if (tokens->value)
				printf(": '%s'", tokens->value);
			printf("]\n");
		}
		else
			printf("[UNKNOWN TYPE: %d]\n", tokens->type);
		tokens = tokens->next;
	}
	printf("==============\n");
}
