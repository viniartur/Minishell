/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_heredoc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 18:51:08 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/19 18:44:20 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern t_shell	*g_shell_ptr;

static volatile sig_atomic_t	g_heredoc_pipefd = -1;

static void	heredoc_sigint_handler(int sig)
{
	(void)sig;
	if (g_heredoc_pipefd != -1)
	{
		close(g_heredoc_pipefd);
		g_heredoc_pipefd = -1;
	}
	if (g_shell_ptr)
		child_exit(g_shell_ptr, 130);
	exit(130);
}

static void	read_heredoc_lines(t_redir *redir, t_shell *shell)
{
	char	*line;
	char	*tmp;
	char	*expanded;

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
}

int	collect_heredoc(t_redir *redir, t_shell *shell)
{
	pid_t	pid;
	int		status;
	int		pipefd[2];
	char	chunk[4096];
	ssize_t	bytes;
	char	*tmp;

	redir->content = ft_strdup("");
	if (!redir->content)
		return (-1);
	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		free(redir->content);
		return (-1);
	}
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
	{
		close(pipefd[0]);
		g_heredoc_pipefd = pipefd[1];
		signal(SIGINT, heredoc_sigint_handler);
		signal(SIGQUIT, SIG_IGN);
		read_heredoc_lines(redir, shell);
		write(pipefd[1], redir->content, ft_strlen(redir->content));
		close(pipefd[1]);
		child_exit(shell, 0);
	}
	close(pipefd[1]);
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		write(STDOUT_FILENO, "\r\n", 2);
		shell->exit_status = 130;
		g_signal = SIGINT;
		close(pipefd[0]);
		free(redir->content);
		redir->content = NULL;
		return (-1);
	}
	if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
	{
		write(STDOUT_FILENO, "\r\n", 2);
		shell->exit_status = 130;
		g_signal = SIGINT;
		close(pipefd[0]);
		free(redir->content);
		redir->content = NULL;
		return (-1);
	}
	free(redir->content);
	redir->content = NULL;
	while (1)
	{
		bytes = read(pipefd[0], chunk, sizeof(chunk) - 1);
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
	close(pipefd[0]);
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

int	prepare_ast_heredocs(t_ast_node *node, t_shell *shell)
{
	if (!node)
		return (0);
	if (node->type == NODE_COMMAND)
		return (prepare_heredocs(node->data.cmd->redirs, shell));
	if (node->type == NODE_PIPELINE)
	{
		if (prepare_ast_heredocs(node->data.pipeline.left, shell) == -1)
			return (-1);
		return (prepare_ast_heredocs(node->data.pipeline.right, shell));
	}
	return (0);
}
