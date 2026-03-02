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

	// 2. PARSER
	shell->ast = parse(shell->tokens, shell);
	if (!shell->ast)
	{
		printf("DEBUG: Error on parsing");
		free_tokens(shell->tokens);
		shell->tokens = NULL;
		return ;
	}
	printf("DEBUG: AST after var expansion:\n");
	print_ast(shell->ast, 0);  // <- ver a AST

	// 3. EXECUÇÃO
	// Built-ins rodam no processo pai se forem comandos simples
	if (shell->ast->type == NODE_COMMAND
		&& shell->ast->data.cmd
		&& shell->ast->data.cmd->argv
		&& is_builtin(shell->ast->data.cmd->argv[0]))
	{
		shell->exit_status = exec_builtin(shell->ast->data.cmd, shell);
	}
	else
	{
		// Futuro Executor (fork + execve) entrará aqui
	}

	// 4. LIMPEZA - Libera memória antes do próximo prompt
	free_ast(shell->ast);
	shell->ast = NULL;
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
