/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_nav.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:32:53 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 20:10:12 by tmorais-         ###   ########.fr       */
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
	while (i < cmd->argc && is_n_flag(cmd->argv[i]))
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

	target = resolve_cd_target(cmd, shell);
	if (!target)
		return (1);
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
	update_pwd(shell, cwd);
	return (0);
}

int	is_numeric_arg(const char *arg)
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

int	parse_exit_code(char *arg)
{
	int	i;
	int	code;

	code = 0;
	if (arg[0] == '-' || arg[0] == '+')
		i = 1;
	else
		i = 0;
	while (arg[i])
		code = code * 10 + (arg[i++] - '0');
	if (arg[0] == '-')
		code = -code;
	return (((code % 256) + 256) % 256);
}
