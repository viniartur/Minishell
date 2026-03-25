/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_env_utils2.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 15:39:06 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 15:41:10 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	**copy_env(char **env, int count)
{
	char	**copy;
	int		i;

	copy = malloc(sizeof(char *) * (count + 1));
	if (!copy)
		return (NULL);
	i = 0;
	while (i < count)
	{
		copy[i] = env[i];
		i++;
	}
	copy[count] = NULL;
	return (copy);
}

void	sort_env(char **env, int count)
{
	int		i;
	int		j;
	char	*tmp;

	i = 0;
	while (i < count - 1)
	{
		j = i + 1;
		while (j < count)
		{
			if (export_strcmp(env[i], env[j]) > 0)
			{
				tmp = env[i];
				env[i] = env[j];
				env[j] = tmp;
			}
			j++;
		}
		i++;
	}
}

void	print_sorted_env(char **env)
{
	int	i;

	i = 0;
	while (env[i])
	{
		printf("declare -x %s\n", env[i]);
		i++;
	}
}

void	print_export_sorted(char **env)
{
	int		count;
	char	**sorted;

	count = env_count(env);
	sorted = copy_env(env, count);
	if (!sorted)
		return ;
	sort_env(sorted, count);
	print_sorted_env(sorted);
	free(sorted);
}

void	remove_env_entry(t_shell *shell, int index)
{
	int	k;

	free(shell->env[index]);
	k = index;
	while (shell->env[k + 1])
	{
		shell->env[k] = shell->env[k + 1];
		k++;
	}
	shell->env[k] = NULL;
}
