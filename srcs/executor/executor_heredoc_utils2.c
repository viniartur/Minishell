/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_heredoc_utils2.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 16:26:58 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 16:27:44 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	init_heredoc(t_redir *redir, int *pipefd)
{
	redir->content = ft_strdup("");
	if (!redir->content)
		return (-1);
	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		free(redir->content);
		return (-1);
	}
	return (0);
}

int	fork_heredoc(int *pipefd, t_redir *redir, t_shell *shell)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		close(pipefd[0]);
		close(pipefd[1]);
		free(redir->content);
		return (-1);
	}
	if (pid == 0)
		run_heredoc_child(pipefd, redir, shell);
	return (pid);
}

int	handle_heredoc_status(int status, t_shell *shell,
	int *pipefd, t_redir *redir)
{
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		return (heredoc_fail(shell, pipefd, redir));
	if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
		return (heredoc_fail(shell, pipefd, redir));
	return (0);
}

int	heredoc_fail(t_shell *shell, int *pipefd, t_redir *redir)
{
	write(STDOUT_FILENO, "\r\n", 2);
	shell->exit_status = 130;
	g_signal = SIGINT;
	close(pipefd[0]);
	free(redir->content);
	redir->content = NULL;
	return (-1);
}

void	read_pipe_content(int fd, t_redir *redir)
{
	char	chunk[4096];
	ssize_t	bytes;
	char	*tmp;

	free(redir->content);
	redir->content = NULL;
	while (1)
	{
		bytes = read(fd, chunk, sizeof(chunk) - 1);
		if (bytes <= 0)
			break ;
		chunk[bytes] = '\0';
		if (!redir->content)
			redir->content = ft_strdup(chunk);
		else
		{
			tmp = ft_strjoin(redir->content, chunk);
			free(redir->content);
			redir->content = tmp;
		}
	}
}
