/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 18:30:04 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 17:07:35 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*int_to_str(int n, int digits)
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

static char	*expand_dollar_var(t_shell *shell, const char *str, int *i)
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

static char	*append_char(char *result, char c)
{
	char	tmp[2];

	tmp[0] = c;
	tmp[1] = '\0';
	return (join_strings(result, tmp));
}

char	*expand_all_variables(t_shell *shell, const char *str)
{
	int		i;
	char	*result;
	char	*expanded;

	if (!str)
		return (NULL);
	result = ft_strdup("");
	if (!result)
		return (NULL);
	i = 0;
	while (str[i])
	{
		if (str[i] == '$')
		{
			expanded = expand_dollar_var(shell, str, &i);
			result = join_strings(result, expanded);
			free(expanded);
		}
		else
			result = append_char(result, str[i++]);
	}
	return (result);
}
