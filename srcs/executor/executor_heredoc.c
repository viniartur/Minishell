/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_heredoc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 16:24:36 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 16:24:39 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	collect_heredoc(t_redir *redir, t_shell *shell)
{
	int		pipefd[2];
	int		status;
	pid_t	pid;

	if (init_heredoc(redir, pipefd) == -1)
		return (-1);
	pid = fork_heredoc(pipefd, redir, shell);
	if (pid == -1)
		return (-1);
	close(pipefd[1]);
	signal(SIGINT, SIG_IGN);
	waitpid(pid, &status, 0);
	setup_signals();
	if (handle_heredoc_status(status, shell, pipefd, redir) == -1)
		return (-1);
	read_pipe_content(pipefd[0], redir);
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
