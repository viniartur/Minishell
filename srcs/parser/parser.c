#include "minishell.h"

typedef struct s_parser
{
	t_token	*tokens;	/* Lista de tokens (início) */
	t_token	*current;	/* Token atual */
	t_shell	*shell;
}	t_parser;

/* Protótipos das funções internas (static) */
static t_ast_node	*parse_command_list(t_parser *parser);
static t_ast_node	*parse_pipeline(t_parser *parser);
static t_ast_node	*parse_command(t_parser *parser);
static t_ast_node	*parse_simple_command(t_parser *parser);
static int			parse_redirections(t_parser *parser, t_command *cmd);

static void	advance_parser(t_parser *parser)
{
	if (parser && parser->current)
		parser->current = parser->current->next;
}

static t_token	*current_token(t_parser *parser)
{
	if (!parser)
		return (NULL);
	return (parser->current);
}

static int	match_token(t_parser *parser, t_token_type type)
{
	if (!parser || !parser->current)
		return (FALSE);
	return (parser->current->type == type);
}

/*
 * Decide como expandir o token com base no tipo de aspas:
 *   TOKEN_QUOTE  -> aspas simples: sem expansão, valor literal
 *   TOKEN_DQUOTE -> aspas duplas:  expande variáveis dentro
 *   TOKEN_WORD   -> sem aspas:     expande variáveis
 */
static char	*expand_token(t_parser *parser, t_token *tok)
{
	if (!tok || !tok->value)
		return (ft_strdup(""));
	if (tok->type == TOKEN_QUOTE)
		return (ft_strdup(tok->value));
	return (expand_all_variables(parser->shell, tok->value));
}

/*
 * Expande $VAR ou $? a partir do token que segue o TOKEN_DOLLAR.
 */
static char	*expand_dollar(t_parser *parser)
{
	t_token	*tok;
	char	*value;

	tok = current_token(parser);
	if (!tok || tok->type == TOKEN_EOF)
		return (ft_strdup("$"));
	/* $$ -> PID do shell */
	if (tok->type == TOKEN_DOLLAR)
	{
		advance_parser(parser);
		return (pid_to_str(getpid()));
	}
	if (tok->type == TOKEN_WORD && tok->value
		&& tok->value[0] == '?' && tok->value[1] == '\0')
	{
		advance_parser(parser);
		return (expand_exit_status(parser->shell));
	}
	if (tok->type == TOKEN_WORD && tok->value)
	{
		value = get_env_value(parser->shell, tok->value);
		advance_parser(parser);
		if (value)
			return (ft_strdup(value));
		return (ft_strdup(""));
	}
	return (ft_strdup("$"));
}

t_ast_node	*parse(t_token *tokens, t_shell *shell)
{
	t_ast_node	*ast;
	t_parser	parser;

	if (!tokens || !shell)
		return (NULL);
	parser.tokens = tokens;
	parser.current = tokens;
	parser.shell = shell;
	//printf("\nPARSER: Initializing parsing...\n");
	ast = parse_command_list(&parser);
	if (ast && parser.current && parser.current->type != TOKEN_EOF)
	{
		printf("minishell: syntax error: unexpected token\n");
		free_ast(ast);
		return (NULL);
	}
	// if (ast)
	// 	printf("PARSER: successfully concluded\n");
	// else
	// 	printf("PARSER: Error on parsing\n");
	return (ast);
}

/* command_list = pipeline ( ';' pipeline )* */
static t_ast_node	*parse_command_list(t_parser *parser)
{
	t_ast_node	*left;
	t_ast_node	*right;

	if (!parser || !parser->current)
		return (NULL);
	left = parse_pipeline(parser);
	if (!left)
		return (NULL);
	while (match_token(parser, TOKEN_SEMICOLON))
	{
		advance_parser(parser);
		/* ';' no fim da linha é válido — para se não houver mais comando */
		if (!parser->current || parser->current->type == TOKEN_EOF)
			break ;
		right = parse_pipeline(parser);
		if (!right)
		{
			free_ast(left);
			return (NULL);
		}
		left = create_pipeline_node(left, right);
	}
	return (left);
}

/* pipeline = command ( '|' command )* */
static t_ast_node	*parse_pipeline(t_parser *parser)
{
	t_ast_node	*left;
	t_ast_node	*right;

	if (!parser || !parser->current)
		return (NULL);
	left = parse_command(parser);
	if (!left)
		return (NULL);
	while (match_token(parser, TOKEN_PIPE)) // if still has pipes, continue
	{
		advance_parser(parser);
		right = parse_command(parser);
		if (!right)
		{
			free_ast(left);
			return (NULL);
		}
		left = create_pipeline_node(left, right);
	}
	return (left);
}

/* command = simple_command (redirection)* */
static t_ast_node	*parse_command(t_parser *parser)
{
	t_ast_node	*node;
	t_command	*cmd;

	node = parse_simple_command(parser);
	if (!node || node->type != NODE_COMMAND)
		return (node);
	cmd = node->data.cmd;
	if (!parse_redirections(parser, cmd))
	{
		free_ast(node);
		return (NULL);
	}
	return (node);
}

/*
 * is_arg_token — retorna 1 se o token pode fazer parte de um argumento.
 */
static int	is_arg_token(t_token *tok)
{
	if (!tok)
		return (0);
	return (tok->type == TOKEN_WORD
		|| tok->type == TOKEN_DQUOTE
		|| tok->type == TOKEN_QUOTE
		|| tok->type == TOKEN_DOLLAR);
}

/*
 * collect_argument — lê todos os tokens contíguos que formam um único
 * argumento e retorna a string resultante concatenada.
 *
 * Resolve casos como:
 *   $?$?        -> "00"             (dois DOLLARs seguidos = um argumento)
 *   "$HOME"txt  -> "/home/usertxt"  (DQUOTE + WORD colados)
 */
static char	*collect_argument(t_parser *parser)
{
	char	*result;
	char	*piece;
	t_token	*current;
	int		count;

	result = ft_strdup("");
	if (!result)
		return (NULL);
	count = 0;
	current = current_token(parser);
	/* Tokens contíguos (sem espaço entre eles) são concatenados no mesmo argumento.
	 * preceded_by_space > 0 indica separação — o loop para, e parse_simple_command
	 * iniciará um novo argumento na próxima iteração. */
	while (current && is_arg_token(current))
	{
		/* Após consumir ao menos um token, espaço antes significa novo argumento */
		if (count > 0 && current->preceded_by_space)
			break ;
		if (current->type == TOKEN_DOLLAR)
		{
			advance_parser(parser);
			piece = expand_dollar(parser);
		}
		else
		{
			piece = expand_token(parser, current);
			advance_parser(parser);
		}
		if (!piece)
			break ;
		result = join_strings(result, piece);
		free(piece);
		count++;
		current = current_token(parser);
	}
	return (result);
}

/*
 * simple_command → (WORD | QUOTE | DQUOTE | '$' WORD)*
 *
 * Expansão de variáveis respeita o tipo de aspas do token:
 *   TOKEN_QUOTE  -> aspas simples: sem expansão (ex: '$HOME' -> $HOME literal)
 *   TOKEN_DQUOTE -> aspas duplas:  expande $VAR (ex: "$HOME" -> /home/user)
 *   TOKEN_WORD   -> sem aspas:     expande $VAR
 *   TOKEN_DOLLAR -> expande variável do token seguinte
 *
 * Tokens contíguos são concatenados em um único argumento:
 *   $?$? -> "00"   |   "$HOME"_text -> "/home/user_text"
 */
static t_ast_node	*parse_simple_command(t_parser *parser)
{
	t_command	*cmd;
	t_token		*current;
	char		*arg;

	cmd = create_command();
	if (!cmd)
		return (NULL);
	current = current_token(parser);
	while (current && is_arg_token(current))
	{
		arg = collect_argument(parser);
		add_argument(cmd, arg);
		free(arg);
		current = current_token(parser);
	}
	if (cmd->argc == 0 && !cmd->redirs)
	{
		free_command(cmd);
		return (NULL);
	}
	return (create_command_node(cmd));
}

/*
 * Processa redirecionamentos: <, >, >>, <<
 * Retorna 1 em sucesso, 0 em erro.
 *
 * Para HEREDOC:
 *   - O delimitador é o token imediato (WORD ou QUOTE).
 *   - Se o delimitador veio entre aspas (TOKEN_QUOTE), expand=0 (literal).
 *   - O conteúdo já foi coletado por collect_heredoc() em shell.c
 *     e está em redir->content — não há mais tokens de conteúdo aqui.
 */
static int	parse_redirections(t_parser *parser, t_command *cmd)
{
	t_token	*current;
	t_redir	*redir;
	int		type;
	int		expand;
	char	*file;

	current = current_token(parser);
	while (current
		&& (current->type == TOKEN_REDIR_IN
			|| current->type == TOKEN_REDIR_OUT
			|| current->type == TOKEN_APPEND
			|| current->type == TOKEN_HEREDOC))
	{
		type = (int)current->type;
		advance_parser(parser);
		current = current_token(parser);
		if (!current || !is_arg_token(current))
		{
			printf("minishell: syntax error near redirection\n");
			return (0);
		}
		if (type == (int)TOKEN_HEREDOC)
		{
			/* Delimitador: TOKEN_QUOTE = literal (sem expansão no conteúdo) */
			expand = (current->type != TOKEN_QUOTE);
			file = ft_strdup(current->value ? current->value : "");
			advance_parser(parser);
		}
		else
		{
			expand = 1;
			file = collect_argument(parser);
		}
		redir = create_redirection(type, file, expand);
		free(file);
		if (!redir)
			return (0);
		add_redirection(cmd, redir);
		current = current_token(parser);
	}
	return (1);
}