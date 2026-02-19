#include "minishell.h"

void	init_shell(t_shell *shell, char **envp)
{
	shell->exit_status = EXIT_SUCCESS;
	shell->tokens = NULL;
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
	shell->tokens = tokenize(input);
	if (!shell->tokens)
	{
		shell->exit_status = 1;
		return ;
	}
	if (is_builtin_exit(shell->tokens))
	{
		shell->exit_status = 0;
		free_tokens(shell->tokens);
		shell->tokens = NULL;
		return ;
	}
	free_tokens(shell->tokens);
	shell->tokens = NULL;
}

/*
static void	process_command(t_shell *shell, char *input)
{
	char	*path;

	if (!input || !*input)
		return ;
	shell->tokens = tokenize(input);
	if (!shell->tokens)
		return ;
	
	// TESTE DO MOTOR:
	// Se o primeiro token for uma palavra, tentamos achar o path dela
	if (shell->tokens->type == TOKEN_WORD)
	{
		path = get_command_path(shell->tokens->value, shell->env);
		if (path)
		{
			printf("Motor encontrou o caminho: %s\n", path);
			free(path); // O motor aloca memória com strdup/strjoin, precisa de free!
		}
		else
			printf("Motor: comando '%s' não encontrado no PATH\n", shell->tokens->value);
	}

	// ... resto do código (free_tokens, etc)
	free_tokens(shell->tokens);
	shell->tokens = NULL;
}
*/

void	handle_input(t_shell *shell, char *input)
{
	if (!input || !*input)
		return ;
	process_command(shell, input);
}

static int	should_exit(t_shell *shell)
{
	if (shell->exit_status == 0 && shell->tokens == NULL)
		return (1);
	return (0);
}

void	main_loop(t_shell *shell)
{
	char	*input;

	while (1)
	{
		input = get_user_input();
		if (input == NULL)
		{
			write(STDOUT_FILENO, "exit\n", 5);
			break ;
		}
		if (*input)
			add_to_history(input);
		handle_input(shell, input);
		if (should_exit(shell))
		{
			free(input);
			break ;
		}
		free(input);
	}
}

void	cleanup_shell(t_shell *shell)
{
	int	i;

	if (shell->tokens)
		free_tokens(shell->tokens);
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
