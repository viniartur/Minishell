/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 18:10:00 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/19 19:15:53 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Usando a variável global que já existe */
extern volatile sig_atomic_t	g_signal;

void	handle_sigint(int sig)
{
	(void)sig;
	g_signal = SIGINT;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	handle_sigquit(int sig)
{
	(void)sig;
	/* Ignora SIGQUIT */
	return ;
}

void	setup_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;
	struct sigaction	sa_tstp;

	/* Configurar SIGINT (Ctrl+C) */
	sa_int.sa_handler = handle_sigint;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa_int, NULL);

	/* Configurar SIGQUIT (Ctrl+\) */
	sa_quit.sa_handler = SIG_IGN;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = SA_RESTART;
	sigaction(SIGQUIT, &sa_quit, NULL);

	/* Configurar SIGTSTP (Ctrl+Z) - ignorar */
	sa_tstp.sa_handler = SIG_IGN;
	sigemptyset(&sa_tstp.sa_mask);
	sa_tstp.sa_flags = SA_RESTART;
	sigaction(SIGTSTP, &sa_tstp, NULL);
}
