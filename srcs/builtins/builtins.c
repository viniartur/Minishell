/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 17:43:42 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 15:46:29 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strcmp(cmd, "echo") == 0 || ft_strcmp(cmd, "cd") == 0 || \
		ft_strcmp(cmd, "pwd") == 0 || ft_strcmp(cmd, "export") == 0 || \
		ft_strcmp(cmd, "unset") == 0 || ft_strcmp(cmd, "env") == 0 || \
		ft_strcmp(cmd, "exit") == 0)
		return (1);
	return (0);
}

int	exec_builtin(t_command *cmd, t_shell *shell)
{
	char	*name;

	name = cmd->argv[0];
	if (ft_strcmp(name, "echo") == 0)
		return (builtin_echo(cmd));
	if (ft_strcmp(name, "cd") == 0)
		return (builtin_cd(cmd, shell));
	if (ft_strcmp(name, "pwd") == 0)
		return (builtin_pwd());
	if (ft_strcmp(name, "env") == 0)
		return (builtin_env(shell));
	if (ft_strcmp(name, "export") == 0)
		return (builtin_export(cmd, shell));
	if (ft_strcmp(name, "unset") == 0)
		return (builtin_unset(cmd, shell));
	if (ft_strcmp(name, "exit") == 0)
		return (builtin_exit(cmd, shell));
	return (1);
}

char	*resolve_cd_target(t_command *cmd, t_shell *shell)
{
	char	*target;

	if (cmd->argc < 2 || ft_strcmp(cmd->argv[1], "~") == 0)
	{
		target = get_env_value(shell, "HOME");
		if (!target)
			ft_putstr_fd("minishell: cd: HOME not set\n", STDERR_FILENO);
	}
	else if (ft_strcmp(cmd->argv[1], "-") == 0)
	{
		target = get_env_value(shell, "OLDPWD");
		if (!target)
			ft_putstr_fd("minishell: cd: OLDPWD not set\n", STDERR_FILENO);
		else
			printf("%s\n", target);
	}
	else
		target = cmd->argv[1];
	return (target);
}

void	update_pwd(t_shell *shell, char *oldcwd)
{
	char	cwd[1024];
	char	*oldpwd;
	char	*newpwd;

	oldpwd = ft_strjoin("OLDPWD=", oldcwd);
	builtin_export_single(oldpwd, shell);
	free(oldpwd);
	if (getcwd(cwd, sizeof(cwd)))
	{
		newpwd = ft_strjoin("PWD=", cwd);
		builtin_export_single(newpwd, shell);
		free(newpwd);
	}
}

int	builtin_exit(t_command *cmd, t_shell *shell)
{
	char	*arg;

	printf("exit\n");
	if (cmd->argc < 2)
	{
		shell->should_exit = 1;
		return (shell->exit_status);
	}
	arg = cmd->argv[1];
	if (!is_numeric_arg(arg))
	{
		ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
		shell->should_exit = 1;
		return (2);
	}
	if (cmd->argc > 2)
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", STDERR_FILENO);
		return (1);
	}
	shell->should_exit = 1;
	return (parse_exit_code(arg));
}
