/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 17:40:00 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/19 18:19:41 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal = 0;
t_shell					*g_shell_ptr = NULL;

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;
	int		ret;

	(void)argc;
	(void)argv;
	g_shell_ptr = &shell;
	setup_signals();
	init_shell(&shell, envp);
	main_loop(&shell);
	cleanup_shell(&shell);
	rl_clear_history();
	ret = shell.exit_status;
	return (ret);
}
