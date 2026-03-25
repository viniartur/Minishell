/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:19:07 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 15:26:33 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	process_command(t_shell *shell, char *input)
{
	if (!input || !*input)
		return ;
	if (!handle_tokens(shell, input))
		return ;
	if (!handle_ast(shell))
		return ;
	shell->exit_status = execute_ast(shell->ast, shell);
	free_ast(shell->ast);
	shell->ast = NULL;
	if (shell->tokens)
	{
		free_tokens(shell->tokens);
		shell->tokens = NULL;
	}
}

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

void	handle_input(t_shell *shell, char *input)
{
	if (!input || !*input)
		return ;
	process_command(shell, input);
}
