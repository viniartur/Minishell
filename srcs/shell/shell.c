/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:20:37 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 15:21:32 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	handle_tokens(t_shell *shell, char *input)
{
	shell->tokens = tokenize(input);
	if (!shell->tokens || shell->tokens->type == TOKEN_EOF)
	{
		if (shell->tokens)
		{
			free_tokens(shell->tokens);
			shell->tokens = NULL;
		}
		return (0);
	}
	return (1);
}

int	handle_ast(t_shell *shell)
{
	shell->ast = parse(shell->tokens, shell);
	if (!shell->ast)
	{
		free_tokens(shell->tokens);
		shell->tokens = NULL;
		return (0);
	}
	if (prepare_ast_heredocs(shell->ast, shell) == -1)
	{
		free_ast(shell->ast);
		shell->ast = NULL;
		free_tokens(shell->tokens);
		shell->tokens = NULL;
		g_signal = 0;
		return (0);
	}
	return (1);
}

static int	handle_signal_interrupt(t_shell *shell, char *input)
{
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
		return (1);
	}
	return (0);
}

static void	handle_post_execution(t_shell *shell)
{
	if (g_signal == SIGINT)
	{
		write(STDOUT_FILENO, "\n", 1);
		g_signal = 0;
		shell->exit_status = 130;
	}
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
			continue ;
		}
		if (handle_signal_interrupt(shell, input))
			continue ;
		add_to_history(input);
		handle_input(shell, input);
		handle_post_execution(shell);
		free(input);
		if (shell->should_exit)
			break ;
	}
}
