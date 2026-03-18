/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 18:30:04 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/10 18:51:29 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*int_to_str(int n, int digits)
{
	char	*str;
	int		i;

	str = malloc(sizeof(char) * (digits + 1));
	if (!str)
		return (NULL);
	str[digits] = '\0';
	i = digits;
	while (i > 0)
	{
		i--;
		str[i] = (n % 10) + '0';
		n /= 10;
	}
	return (str);
}

char	*expand_exit_status(t_shell *shell)
{
	int	status;
	int	temp;
	int	digits;

	if (!shell)
		return (ft_strdup("0"));
	status = shell->exit_status;
	if (status < 0)
		status = 255;
	digits = 1;
	temp = status;
	while (temp >= 10)
	{
		temp /= 10;
		digits++;
	}
	return (int_to_str(status, digits));
}

char	*expand_variable(t_shell *shell, const char *str, int *i)
{
	char	*var_name;
	char	*value;
	char	*result;

	if (str[*i] == '?')
	{
		(*i)++;
		return (expand_exit_status(shell));
	}
	if (str[*i] == '$')
	{
		(*i)++;
		return (pid_to_str(getpid()));
	}
	var_name = extract_var_name(str, *i);
	if (!var_name)
		return (ft_strdup("$"));
	*i += ft_strlen(var_name);
	value = get_env_value(shell, var_name);
	free(var_name);
	if (value)
		result = ft_strdup(value);
	else
		result = ft_strdup("");
	return (result);
}

static char	*expand_dollar(t_shell *shell, const char *str, int *i)
{
	char	*expanded;
	char	*result;

	(*i)++;
	expanded = expand_variable(shell, str, i);
	if (!expanded)
		return (ft_strdup(""));
	result = ft_strdup(expanded);
	free(expanded);
	return (result);
}

char	*expand_all_variables(t_shell *shell, const char *str)
{
	int		i;
	char	*result;
	char	tmp[2];

	if (!str)
		return (NULL);
	result = ft_strdup("");
	if (!result)
		return (NULL);
	i = 0;
	while (str[i])
	{
		if (str[i] == '$')
			result = join_strings(result, expand_dollar(shell, str, &i));
		else
		{
			tmp[0] = str[i++];
			tmp[1] = '\0';
			result = join_strings(result, tmp);
		}
	}
	return (result);
}
