/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_nav.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 17:42:03 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/10 17:47:49 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_pwd(void)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)))
	{
		printf("%s\n", cwd);
		return (0);
	}
	perror("pwd");
	return (1);
}

int	builtin_echo(t_command *cmd)
{
	int	i;
	int	n_flag;

	i = 1;
	n_flag = 0;
	if (cmd->argc > 1 && ft_strcmp(cmd->argv[1], "-n") == 0)
	{
		n_flag = 1;
		i++;
	}
	while (i < cmd->argc)
	{
		printf("%s", cmd->argv[i]);
		if (i < cmd->argc - 1)
			printf(" ");
		i++;
	}
	if (!n_flag)
		printf("\n");
	return (0);
}

int	builtin_cd(t_command *cmd, t_shell *shell)
{
	char	*target;
	char	cwd[1024];
	char	*oldpwd;
	char	*newpwd;

	if (cmd->argc < 2 || ft_strcmp(cmd->argv[1], "~") == 0)
	{
		target = get_env_value(shell, "HOME");
		if (!target)
		{
			ft_putstr_fd("minishell: cd: HOME not set\n", STDERR_FILENO);
			return (1);
		}
	}
	else if (ft_strcmp(cmd->argv[1], "-") == 0)
	{
		target = get_env_value(shell, "OLDPWD");
		if (!target)
		{
			ft_putstr_fd("minishell: cd: OLDPWD not set\n", STDERR_FILENO);
			return (1);
		}
		printf("%s\n", target);
	}
	else
		target = cmd->argv[1];
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		cwd[0] = '\0';
	if (chdir(target) == -1)
	{
		ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
		ft_putstr_fd(target, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd(strerror(errno), STDERR_FILENO);
		ft_putstr_fd("\n", STDERR_FILENO);
		return (1);
	}
	oldpwd = ft_strjoin("OLDPWD=", cwd);
	builtin_export_single(oldpwd, shell);
	free(oldpwd);
	if (getcwd(cwd, sizeof(cwd)))
	{
		newpwd = ft_strjoin("PWD=", cwd);
		builtin_export_single(newpwd, shell);
		free(newpwd);
	}
	return (0);
}

static int	is_numeric_arg(const char *arg)
{
	int	i;

	i = 0;
	if (arg[i] == '-' || arg[i] == '+')
		i++;
	if (!arg[i])
		return (0);
	while (arg[i])
	{
		if (arg[i] < '0' || arg[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

int	builtin_exit(t_command *cmd, t_shell *shell)
{
	int		i;
	int		code;
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
	code = 0;
	i = (arg[0] == '-' || arg[0] == '+') ? 1 : 0;
	while (arg[i])
		code = code * 10 + (arg[i++] - '0');
	if (arg[0] == '-')
		code = -code;
	code = ((code % 256) + 256) % 256;
	shell->should_exit = 1;
	return (code);
}
