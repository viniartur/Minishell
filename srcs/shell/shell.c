/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 18:31:35 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/19 18:50:32 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	init_shell(t_shell *shell, char **envp)
{
	shell->exit_status = EXIT_SUCCESS;
	shell->should_exit = 0;
	shell->in_child = 0;
	shell->tokens = NULL;
	shell->ast = NULL;
	shell->env = ft_copy_env(envp);
	g_signal = 0;
}

void	process_command(t_shell *shell, char *input)
{
	if (!input || !*input)
		return ;
	shell->tokens = tokenize(input);
	if (!shell->tokens || shell->tokens->type == TOKEN_EOF)
	{
		if (shell->tokens)
		{
			free_tokens(shell->tokens);
			shell->tokens = NULL;
		}
		return ;
	}
	shell->ast = parse(shell->tokens, shell);
	if (!shell->ast)
	{
		free_tokens(shell->tokens);
		shell->tokens = NULL;
		return ;
	}
	if (prepare_ast_heredocs(shell->ast, shell) == -1)
	{
		free_ast(shell->ast);
		shell->ast = NULL;
		free_tokens(shell->tokens);
		shell->tokens = NULL;
		g_signal = 0;
		return ;
	}
	shell->exit_status = execute_ast(shell->ast, shell);
	free_ast(shell->ast);
	shell->ast = NULL;
	if (shell->tokens)
	{
		free_tokens(shell->tokens);
		shell->tokens = NULL;
	}
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
		if (input == NULL)
		{
			write(STDOUT_FILENO, "exit\n", 5);
			break ;
		}
		if (!*input)
		{
			free(input);
			continue;
		}
		if (g_signal == SIGINT)
		{
			shell->exit_status = 130;
			g_signal = 0;
			if (shell->ast)
			{
				free_ast(shell->ast);
				shell->ast = NULL;
			}
			if (shell->tokens)
			{
				free_tokens(shell->tokens);
				shell->tokens = NULL;
			}
			free(input);
			continue;
		}
		add_to_history(input);
		handle_input(shell, input);
		if (g_signal == SIGINT)
		{
			write(STDOUT_FILENO, "\n", 1);
			g_signal = 0;
			shell->exit_status = 130;
		}
		free(input);
		input = NULL;
		if (shell->should_exit)
			break ;
	}
}

void	cleanup_shell(t_shell *shell)
{
	int	i;

	if (shell->tokens)
	{
		free_tokens(shell->tokens);
		shell->tokens = NULL;
	}
	if (shell->ast)
	{
		free_ast(shell->ast);
		shell->ast = NULL;
	}
	if (shell->env)
	{
		i = 0;
		while (shell->env[i])
		{
			free(shell->env[i]);
			shell->env[i] = NULL;
			i++;
		}
		free(shell->env);
		shell->env = NULL;
	}
}
