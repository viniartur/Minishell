/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:56:13 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 16:08:45 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	handle_redirections(t_redir *redir)
{
	while (redir)
	{
		if (redir->type == TOKEN_REDIR_OUT
			&& handle_output_redir(redir) == -1)
			return (-1);
		else if (redir->type == TOKEN_APPEND
			&& handle_append_redir(redir) == -1)
			return (-1);
		else if (redir->type == TOKEN_REDIR_IN
			&& handle_input_redir(redir) == -1)
			return (-1);
		else if (redir->type == TOKEN_HEREDOC
			&& handle_heredoc_redir(redir) == -1)
			return (-1);
		redir = redir->next;
	}
	return (0);
}

int	execute_ast(t_ast_node *node, t_shell *shell)
{
	if (!node)
		return (EXIT_SUCCESS);
	if (node->type == NODE_COMMAND)
		return (execute_command(node->data.cmd, shell));
	if (node->type == NODE_PIPELINE)
		return (execute_pipeline(node, shell));
	return (EXIT_FAILURE);
}

int	execute_builtin_with_redir(t_command *cmd, t_shell *shell)
{
	int	in;
	int	out;
	int	status;

	in = dup(STDIN_FILENO);
	out = dup(STDOUT_FILENO);
	if (in == -1 || out == -1)
		return (EXIT_FAILURE);
	if (handle_redirections(cmd->redirs) == -1)
		return (restore_fds(in, out), EXIT_FAILURE);
	status = EXIT_SUCCESS;
	if (cmd->argv && cmd->argv[0])
		status = exec_builtin(cmd, shell);
	restore_fds(in, out);
	return (status);
}

int	execute_command(t_command *cmd, t_shell *shell)
{
	pid_t	pid;
	int		status;

	if (!cmd)
		return (EXIT_SUCCESS);
	if (!cmd->argv || !cmd->argv[0])
	{
		if (cmd->redirs)
			return (execute_builtin_with_redir(cmd, shell));
		return (EXIT_SUCCESS);
	}
	if (is_builtin(cmd->argv[0]))
		return (execute_builtin_with_redir(cmd, shell));
	pid = fork();
	if (pid == -1)
		return (perror("fork"), EXIT_FAILURE);
	if (pid == 0)
		exec_child(cmd, shell);
	if (!shell->in_child)
		signal(SIGINT, SIG_IGN);
	waitpid(pid, &status, 0);
	if (!shell->in_child)
		setup_signals();
	return (handle_exit_status(status, shell));
}

int	execute_pipeline(t_ast_node *node, t_shell *shell)
{
	int		fd[2];
	pid_t	l;
	pid_t	r;
	int		sl;
	int		sr;

	if (pipe(fd) == -1)
		return (perror("pipe"), EXIT_FAILURE);
	l = fork();
	if (l == 0)
		child_left(node, shell, fd);
	r = fork();
	if (r == 0)
		child_right(node, shell, fd);
	close(fd[0]);
	close(fd[1]);
	waitpid(l, &sl, 0);
	waitpid(r, &sr, 0);
	return (handle_pipeline_status(sl, sr, shell));
}
