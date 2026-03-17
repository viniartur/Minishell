/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_heredoc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 18:51:08 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/10 18:51:16 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	collect_heredoc(t_redir *redir, t_shell *shell)
{
	char	*line;
	char	*tmp;
	char	*expanded;
	int		saved_stdin;
	int		tty_fd;

	redir->content = ft_strdup("");
	if (!redir->content)
		return (-1);
	saved_stdin = dup(STDIN_FILENO);
	tty_fd = open("/dev/tty", O_RDONLY);
	if (tty_fd != -1)
	{
		dup2(tty_fd, STDIN_FILENO);
		close(tty_fd);
	}
	while (1)
	{
		line = readline("heredoc> ");
		if (!line)
		{
			ft_putstr_fd("minishell: warning: heredoc delimited by EOF\n",
				STDERR_FILENO);
			break ;
		}
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
	dup2(saved_stdin, STDIN_FILENO);
	close(saved_stdin);
	return (0);
}

int	prepare_heredocs(t_redir *redir, t_shell *shell)
{
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
		{
			if (collect_heredoc(redir, shell) == -1)
				return (-1);
		}
		redir = redir->next;
	}
	return (0);
}
