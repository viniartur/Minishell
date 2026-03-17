#include "minishell.h"

static int		handle_dollar(t_lexer *lexer, t_token **tokens, t_token **last);
static int		handle_redirection(t_lexer *lexer, t_token **tokens, t_token **last);
static int		handle_pipe(t_lexer *lexer, t_token **tokens, t_token **last);
static int		handle_word(t_lexer *lexer, t_token **tokens, t_token **last);
static int		handle_semicolon(t_lexer *lexer, t_token **tokens, t_token **last);
static int		handle_ampersand(t_lexer *lexer, t_token **tokens, t_token **last);
static void		add_token_to_list(t_token **tokens, t_token **last, t_token *new_token);
static int		create_and_add_token(t_token **tokens, t_token **last,
				t_token_type type, const char *value, int len);
static int		finalize_tokens(t_lexer *lexer, t_token **tokens, t_token **last);

static int	is_valid_var_char_lexer(char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
		(c >= '0' && c <= '9') || c == '_');
}

static int	handle_dollar(t_lexer *lexer, t_token **tokens, t_token **last)
{
	char	*buf;
	int		len;
	int		result;

	if (create_and_add_token(tokens, last, TOKEN_DOLLAR, NULL, 1) == LEXER_ERROR)
		return (LEXER_ERROR);
	advance_lexer(lexer); /* Consome o '$' */
	lexer->start = lexer->pos;
	/* Se o próximo char for '?', cria WORD: '?' para expand_dollar tratar $? */
	if (lexer->current == '?')
	{
		advance_lexer(lexer);
		return (create_and_add_token(tokens, last, TOKEN_WORD, "?", 1));
	}
	/* Lê apenas caracteres válidos de nome de variável [a-zA-Z0-9_] */
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
		result = LEXER_SUCCESS; /* $ isolado — expand_dollar trata como '$' literal */
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

static int	handle_pipe(t_lexer *lexer, t_token **tokens, t_token **last)
{
	if (create_and_add_token(tokens, last, TOKEN_PIPE, NULL, 1) == LEXER_ERROR)
		return (LEXER_ERROR);
	advance_lexer(lexer);
	lexer->start = lexer->pos;
	return (LEXER_SUCCESS);
}

/*
 * emit_segment — emite um token do segmento acumulado em buf[0..len-1].
 * Para QUOTE/DQUOTE vazios ("" ou ''), emite o token mesmo com len == 0.
 */
static int	emit_segment(t_token **tokens, t_token **last,
			t_token_type type, char *buf, int len)
{
	buf[len] = '\0';
	if (len > 0)
		return (create_and_add_token(tokens, last, type, buf, len));
	if (type == TOKEN_QUOTE || type == TOKEN_DQUOTE)
		return (create_and_add_token(tokens, last, type, "", 0));
	return (LEXER_SUCCESS);
}

/*
 * handle_word — lê uma palavra do input e emite um ou mais tokens,
 * um por segmento de aspas diferente.
 *
 * Palavra mista:  oi" $USER '$USER ola'"' "$USER" BEM?'
 * Tokens emitidos (sem preceded_by_space entre eles — o parser concatena):
 *   TOKEN_WORD   "oi"                  -> expande $VAR
 *   TOKEN_DQUOTE " $USER '$USER ola'"  -> expande $VAR
 *   TOKEN_QUOTE  ' "$USER" BEM?'       -> literal, SEM expansão
 *
 * Assim single-quotes suprimem corretamente a expansão de variáveis.
 */
static int	handle_word(t_lexer *lexer, t_token **tokens, t_token **last)
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
		/* Início de um segmento entre aspas */
		if (lexer->current == '\'' || lexer->current == '"')
		{
			/* Emite o segmento WORD acumulado antes das aspas */
			if (len > 0)
			{
				result = emit_segment(tokens, last, TOKEN_WORD, buf, len);
				len = 0;
				if (result != LEXER_SUCCESS)
					break ;
			}
			/* Lê o segmento quotado num novo token */
			quote_start = lexer->current;
			advance_lexer(lexer); /* Consome aspa de abertura */
			while (lexer->current != '\0' && lexer->current != quote_start)
			{
				buf[len++] = lexer->current;
				advance_lexer(lexer);
			}
			if (lexer->current != quote_start)
			{
				free(buf);
				write(STDERR_FILENO, "minishell: syntax error: unclosed quote\n", 41);
				return (LEXER_ERROR);
			}
			advance_lexer(lexer); /* Consome aspa de fechamento */
			result = emit_segment(tokens, last,
					(quote_start == '\'') ? TOKEN_QUOTE : TOKEN_DQUOTE,
					buf, len);
			len = 0;
			continue ;
		}
		/* Delimitadores externos: para o loop */
		if (ft_isspace(lexer->current) ||
			lexer->current == '|' || lexer->current == '<' ||
			lexer->current == '>' || lexer->current == '$' ||
			lexer->current == ';' || lexer->current == '&' ||
			lexer->current == '\0')
			break ;
		buf[len++] = lexer->current;
		advance_lexer(lexer);
	}
	/* Emite o segmento WORD final (se houver) */
	if (result == LEXER_SUCCESS && len > 0)
		result = emit_segment(tokens, last, TOKEN_WORD, buf, len);
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
		/* Salva ponteiro anterior ao handler: handle_word pode emitir vários
		 * tokens numa palavra mista (WORD+DQUOTE+QUOTE). Só o PRIMEIRO deve
		 * receber preceded_by_space — os demais estão colados sem espaço. */
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
		/* Marca apenas o PRIMEIRO token novo com o espaço anterior */
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
