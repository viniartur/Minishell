/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 16:02:36 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 16:20:30 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	handle_exit_status(int status, t_shell *shell)
{
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		write(STDOUT_FILENO, "\r\n", 2);
		shell->exit_status = 130;
		g_signal = SIGINT;
		return (130);
	}
	if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
	{
		shell->exit_status = 130;
		g_signal = SIGINT;
		return (130);
	}
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (EXIT_FAILURE);
}

void	exec_child(t_command *cmd, t_shell *shell)
{
	signal(SIGINT, child_sigint_handler);
	signal(SIGQUIT, SIG_DFL);
	if (handle_redirections(cmd->redirs) == -1)
		child_exit(shell, EXIT_FAILURE);
	if (g_signal == SIGINT)
		child_exit(shell, 130);
	execve_wrapper(cmd, shell);
}

void	execve_wrapper(t_command *cmd, t_shell *shell)
{
	char	*path;
	char	**argv_copy;

	path = get_command_path(cmd->argv[0], shell->env);
	if (!path)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(cmd->argv[0], STDERR_FILENO);
		ft_putstr_fd(": command not found\n", STDERR_FILENO);
		child_exit(shell, 127);
	}
	argv_copy = dup_argv(cmd->argv, cmd->argc);
	if (!argv_copy)
	{
		free(path);
		child_exit(shell, EXIT_FAILURE);
	}
	free_child_memory(shell);
	execve(path, argv_copy, shell->env);
	perror(argv_copy[0]);
	free(path);
	free_argv(argv_copy);
	free_shell(shell);
	exit(126);
}

void	restore_fds(int in, int out)
{
	dup2(in, STDIN_FILENO);
	dup2(out, STDOUT_FILENO);
	close(in);
	close(out);
}

int	handle_heredoc_redir(t_redir *r)
{
	int	pipe_fd[2];

	if (pipe(pipe_fd) == -1)
		return (perror("pipe"), -1);
	write(pipe_fd[1], r->content, ft_strlen(r->content));
	close(pipe_fd[1]);
	dup2(pipe_fd[0], STDIN_FILENO);
	close(pipe_fd[0]);
	return (0);
}
