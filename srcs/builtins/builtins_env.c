/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_env.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:35:27 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 15:44:24 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_env(t_shell *shell)
{
	int	i;

	i = 0;
	while (shell->env && shell->env[i])
	{
		printf("%s\n", shell->env[i]);
		i++;
	}
	return (0);
}

int	builtin_export_single(char *entry, t_shell *shell)
{
	int		len;
	char	*eq;

	if (!entry || !shell)
		return (1);
	eq = ft_strchr(entry, '=');
	if (eq)
		len = (int)(eq - entry);
	else
		len = (int)ft_strlen(entry);
	if (update_existing_env(shell, entry, len))
		return (0);
	return (add_new_env(shell, entry));
}

int	builtin_export(t_command *cmd, t_shell *shell)
{
	int	i;

	if (cmd->argc < 2)
	{
		print_export_sorted(shell->env);
		return (0);
	}
	i = 1;
	while (i < cmd->argc)
	{
		if (!is_valid_identifier(cmd->argv[i]))
		{
			ft_putstr_fd("minishell: export: `", STDERR_FILENO);
			ft_putstr_fd(cmd->argv[i], STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			i++;
			continue ;
		}
		builtin_export_single(cmd->argv[i], shell);
		i++;
	}
	return (0);
}

void	unset_single(t_shell *shell, char *var)
{
	int		j;
	size_t	len;

	len = ft_strlen(var);
	j = 0;
	while (shell->env && shell->env[j])
	{
		if (ft_strncmp(shell->env[j], var, len) == 0
			&& (shell->env[j][len] == '=' || shell->env[j][len] == '\0'))
		{
			remove_env_entry(shell, j);
			break ;
		}
		j++;
	}
}

int	builtin_unset(t_command *cmd, t_shell *shell)
{
	int	i;

	i = 1;
	while (i < cmd->argc)
	{
		unset_single(shell, cmd->argv[i]);
		i++;
	}
	return (0);
}
