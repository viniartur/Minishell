/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_env.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 17:50:09 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/10 17:50:15 by tmorais-         ###   ########.fr       */
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

static int	env_count(char **env)
{
	int	i;

	i = 0;
	while (env && env[i])
		i++;
	return (i);
}

int	builtin_export_single(char *entry, t_shell *shell)
{
	int		i;
	int		len;
	char	*eq;
	char	**new_env;

	if (!entry || !shell)
		return (1);
	eq = ft_strchr(entry, '=');
	len = eq ? (int)(eq - entry) : (int)ft_strlen(entry);
	i = 0;
	while (shell->env && shell->env[i])
	{
		if (ft_strncmp(shell->env[i], entry, len) == 0
			&& (shell->env[i][len] == '=' || shell->env[i][len] == '\0'))
		{
			free(shell->env[i]);
			shell->env[i] = ft_strdup(entry);
			return (0);
		}
		i++;
	}
	new_env = malloc(sizeof(char *) * (env_count(shell->env) + 2));
	if (!new_env)
		return (1);
	i = 0;
	while (shell->env && shell->env[i])
	{
		new_env[i] = shell->env[i];
		i++;
	}
	new_env[i] = ft_strdup(entry);
	new_env[i + 1] = NULL;
	free(shell->env);
	shell->env = new_env;
	return (0);
}

static int	is_valid_identifier(const char *str)
{
	int	i;

	if (!str || !str[0])
		return (0);
	if (!((str[0] >= 'a' && str[0] <= 'z')
			|| (str[0] >= 'A' && str[0] <= 'Z') || str[0] == '_'))
		return (0);
	i = 1;
	while (str[i] && str[i] != '=')
	{
		if (!((str[i] >= 'a' && str[i] <= 'z')
				|| (str[i] >= 'A' && str[i] <= 'Z')
				|| (str[i] >= '0' && str[i] <= '9') || str[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

int	builtin_export(t_command *cmd, t_shell *shell)
{
	int	i;

	if (cmd->argc < 2)
	{
		i = 0;
		while (shell->env && shell->env[i])
		{
			printf("declare -x %s\n", shell->env[i]);
			i++;
		}
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

int	builtin_unset(t_command *cmd, t_shell *shell)
{
	int		i;
	int		j;
	int		k;
	size_t	len;

	i = 1;
	while (i < cmd->argc)
	{
		len = ft_strlen(cmd->argv[i]);
		j = 0;
		while (shell->env && shell->env[j])
		{
			if (ft_strncmp(shell->env[j], cmd->argv[i], len) == 0
				&& (shell->env[j][len] == '=' || shell->env[j][len] == '\0'))
			{
				free(shell->env[j]);
				k = j;
				while (shell->env[k + 1])
				{
					shell->env[k] = shell->env[k + 1];
					k++;
				}
				shell->env[k] = NULL;
				break ;
			}
			j++;
		}
		i++;
	}
	return (0);
}
