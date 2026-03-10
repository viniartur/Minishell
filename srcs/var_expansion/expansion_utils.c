/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 18:26:59 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/10 18:31:02 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*extract_var_name(const char *str, int start)
{
	int		len;
	char	*name;

	len = 0;
	while (str[start + len] && is_valid_var_char(str[start + len]))
		len++;
	if (len == 0)
		return (NULL);
	name = malloc(sizeof(char) * (len + 1));
	if (!name)
		return (NULL);
	ft_memcpy(name, &str[start], len);
	name[len] = '\0';
	return (name);
}

char	*pid_to_str(int n)
{
	char	buf[20];
	int		len;
	int		tmp;

	if (n == 0)
		return (ft_strdup("0"));
	len = 0;
	tmp = n;
	while (tmp > 0)
	{
		tmp /= 10;
		len++;
	}
	buf[len] = '\0';
	tmp = n;
	while (len > 0)
	{
		len--;
		buf[len] = (tmp % 10) + '0';
		tmp /= 10;
	}
	return (ft_strdup(buf));
}

char	*get_env_value(t_shell *shell, const char *var_name)
{
	int		i;
	size_t	len;

	if (!shell || !shell->env || !var_name)
		return (NULL);
	len = ft_strlen(var_name);
	i = 0;
	while (shell->env[i])
	{
		if (ft_strncmp(shell->env[i], var_name, len) == 0 && \
			shell->env[i][len] == '=')
			return (shell->env[i] + len + 1);
		i++;
	}
	return (NULL);
}
