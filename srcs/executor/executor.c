/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 17:52:57 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/10 17:58:10 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

int	handle_redirections(t_redir *redir)
{
	int	fd;
	int	pipe_fd[2];

	while (redir)
	{
		if (redir->type == TOKEN_REDIR_OUT)
		{
			fd = open(redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd == -1)
				return (perror(redir->file), -1);
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (redir->type == TOKEN_APPEND)
		{
			fd = open(redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd == -1)
				return (perror(redir->file), -1);
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (redir->type == TOKEN_REDIR_IN)
		{
			fd = open(redir->file, O_RDONLY);
			if (fd == -1)
				return (perror(redir->file), -1);
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		else if (redir->type == TOKEN_HEREDOC)
		{
			if (pipe(pipe_fd) == -1)
				return (perror("pipe"), -1);
			write(pipe_fd[1], redir->content, ft_strlen(redir->content));
			close(pipe_fd[1]);
			dup2(pipe_fd[0], STDIN_FILENO);
			close(pipe_fd[0]);
		}
		redir = redir->next;
	}
	return (0);
}

int	execute_builtin_with_redir(t_command *cmd, t_shell *shell)
{
	int	saved_stdin;
	int	saved_stdout;
	int	status;

	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	if (saved_stdin == -1 || saved_stdout == -1)
		return (EXIT_FAILURE);
	if (handle_redirections(cmd->redirs) == -1)
	{
		dup2(saved_stdin, STDIN_FILENO);
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdin);
		close(saved_stdout);
		return (EXIT_FAILURE);
	}
	status = exec_builtin(cmd, shell);
	dup2(saved_stdin, STDIN_FILENO);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdin);
	close(saved_stdout);
	return (status);
}

int	execute_command(t_command *cmd, t_shell *shell)
{
	pid_t	pid;
	int		status;
	char	*path;

	if (!cmd || !cmd->argv || !cmd->argv[0])
		return (EXIT_SUCCESS);
	if (prepare_heredocs(cmd->redirs, shell) == -1)
		return (EXIT_FAILURE);
	if (is_builtin(cmd->argv[0]))
		return (execute_builtin_with_redir(cmd, shell));
	pid = fork();
	if (pid == -1)
		return (perror("fork"), EXIT_FAILURE);
	if (pid == 0)
	{
		if (handle_redirections(cmd->redirs) == -1)
			exit(EXIT_FAILURE);
		path = get_command_path(cmd->argv[0], shell->env);
		if (!path)
		{
			ft_putstr_fd("minishell: ", STDERR_FILENO);
			ft_putstr_fd(cmd->argv[0], STDERR_FILENO);
			ft_putstr_fd(": command not found\n", STDERR_FILENO);
			exit(127);
		}
		execve(path, cmd->argv, shell->env);
		perror(cmd->argv[0]);
		free(path);
		exit(126);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (EXIT_FAILURE);
}

int	execute_pipeline(t_ast_node *node, t_shell *shell)
{
	int		fd[2];
	pid_t	pid_left;
	pid_t	pid_right;
	int		status_right;

	if (pipe(fd) == -1)
		return (perror("pipe"), EXIT_FAILURE);
	pid_left = fork();
	if (pid_left == -1)
		return (perror("fork"), close(fd[0]), close(fd[1]), EXIT_FAILURE);
	if (pid_left == 0)
	{
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
		shell->should_exit = 0;
		exit(execute_ast(node->data.pipeline.left, shell));
	}
	pid_right = fork();
	if (pid_right == -1)
	{
		close(fd[0]);
		close(fd[1]);
		waitpid(pid_left, NULL, 0);
		return (perror("fork"), EXIT_FAILURE);
	}
	if (pid_right == 0)
	{
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);
		shell->should_exit = 0;
		exit(execute_ast(node->data.pipeline.right, shell));
	}
	close(fd[0]);
	close(fd[1]);
	waitpid(pid_left, NULL, 0);
	waitpid(pid_right, &status_right, 0);
	if (WIFEXITED(status_right))
		return (WEXITSTATUS(status_right));
	if (WIFSIGNALED(status_right))
		return (128 + WTERMSIG(status_right));
	return (EXIT_FAILURE);
}
