/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_heredoc_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 16:24:56 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 16:27:38 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	heredoc_sigint_handler(int sig)
{
	(void)sig;
	g_signal = SIGINT;
}

char	*read_heredoc_line(void)
{
	char	buf[4096];
	int		i;
	char	c;
	int		ret;

	write(STDOUT_FILENO, "heredoc> ", 9);
	i = 0;
	while (1)
	{
		ret = read(STDIN_FILENO, &c, 1);
		if (ret <= 0 || g_signal == SIGINT)
			return (NULL);
		if (c == '\n')
			break ;
		if (i < 4095)
			buf[i++] = c;
	}
	buf[i] = '\0';
	return (ft_strdup(buf));
}

void	read_heredoc_lines(t_redir *redir, t_shell *shell)
{
	char	*line;
	char	*tmp;
	char	*expanded;

	while (1)
	{
		line = read_heredoc_line();
		if (!line)
			break ;
		if (ft_strcmp(line, redir->file) == 0)
		{
			free(line);
			break ;
		}
		if (redir->expand)
			expanded = expand_all_variables(shell, line);
		else
			expanded = ft_strdup(line);
		free(line);
		tmp = ft_strjoin(redir->content, expanded);
		free(redir->content);
		free(expanded);
		redir->content = ft_strjoin(tmp, "\n");
		free(tmp);
	}
}

void	run_heredoc_child(int *pipefd, t_redir *redir, t_shell *shell)
{
	struct sigaction	sa;

	close(pipefd[0]);
	sa.sa_handler = heredoc_sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
	read_heredoc_lines(redir, shell);
	if (g_signal == SIGINT)
	{
		close(pipefd[1]);
		child_exit(shell, 130);
	}
	write(pipefd[1], redir->content, ft_strlen(redir->content));
	close(pipefd[1]);
	child_exit(shell, 0);
}
