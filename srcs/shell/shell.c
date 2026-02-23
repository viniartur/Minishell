#include "minishell.h"

void	init_shell(t_shell *shell, char **envp)
{
	shell->exit_status = EXIT_SUCCESS;
	shell->should_exit = 0;
	shell->tokens = NULL;
	shell->ast = NULL;
	shell->env = ft_copy_env(envp);
	g_signal = 0;
}

static int	is_builtin_exit(t_token *tokens)
{
	if (!tokens || tokens->type != TOKEN_WORD)
		return (0);
	if (ft_strcmp(tokens->value, "exit") == 0)
		return (1);
	return (0);
}

static void	process_command(t_shell *shell, char *input)
{
    if (!input || !*input)
        return ;

    // 1. LEXER
    shell->tokens = tokenize(input);
    if (!shell->tokens || shell->tokens->type == TOKEN_EOF)
    {
        if (shell->tokens)
            free_tokens(shell->tokens);
		shell->tokens = NULL;
        return ;
    }
    print_tokens(shell->tokens);  // <- ver os tokens
	if (is_builtin_exit(shell->tokens))
	{
		shell->should_exit = 1;
		free_tokens(shell->tokens);
		shell->tokens = NULL;
		return ;
	}
    // 2. PARSER
    shell->ast = parse(shell->tokens);
    if (shell->ast)
    {
        print_ast(shell->ast, 0);  // <- ver a AST
        free_ast(shell->ast);
        shell->ast = NULL;
    }
    free_tokens(shell->tokens);
    shell->tokens = NULL;
}

void	handle_input(t_shell *shell, char *input)
{
	if (!input || !*input)
		return ;
	process_command(shell, input);
}

void	main_loop(t_shell *shell)
{
	char	*input;

	while (1)
	{
		input = get_user_input();
		if (input == NULL) // CTRL + D
		{
			write(STDOUT_FILENO, "exit\n", 5);
			break ;
		}
		if (*input)
			add_to_history(input);
		handle_input(shell, input);
		free(input);
		if (shell->should_exit)
			break ;
	}
}

void	cleanup_shell(t_shell *shell)
{
	int	i;

	if (shell->tokens)
		free_tokens(shell->tokens);
	if (shell->ast)
		free_ast(shell->ast);
	if (shell->env)
	{
		i = 0;
		while (shell->env[i])
		{
			free(shell->env[i]);
			i++;
		}
		free(shell->env);
		shell->env = NULL;
	}
	rl_clear_history();
}
