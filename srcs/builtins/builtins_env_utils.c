/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_env_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:36:41 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 15:38:36 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	env_count(char **env)
{
	int	i;

	i = 0;
	while (env && env[i])
		i++;
	return (i);
}

int	update_existing_env(t_shell *shell, char *entry, int len)
{
	int	i;

	i = 0;
	while (shell->env && shell->env[i])
	{
		if (ft_strncmp(shell->env[i], entry, len) == 0
			&& (shell->env[i][len] == '=' || shell->env[i][len] == '\0'))
		{
			free(shell->env[i]);
			shell->env[i] = ft_strdup(entry);
			return (1);
		}
		i++;
	}
	return (0);
}

int	add_new_env(t_shell *shell, char *entry)
{
	int		i;
	char	**new_env;

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

int	is_valid_identifier(const char *str)
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

int	export_strcmp(const char *a, const char *b)
{
	int	ia;
	int	ib;
	int	min;

	ia = 0;
	ib = 0;
	while (a[ia] && a[ia] != '=')
		ia++;
	while (b[ib] && b[ib] != '=')
		ib++;
	min = ia;
	if (ib < min)
		min = ib;
	if (ft_strncmp(a, b, min) != 0)
		return (ft_strncmp(a, b, min));
	if (ia != ib)
		return (ia - ib);
	return (0);
}
