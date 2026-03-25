/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_expansion.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 16:46:51 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 16:47:35 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*expand_token(t_parser *parser, t_token *tok)
{
	if (!tok || !tok->value)
		return (ft_strdup(""));
	if (tok->type == TOKEN_QUOTE)
		return (ft_strdup(tok->value));
	return (expand_all_variables(parser->shell, tok->value));
}

char	*expand_exit_code(t_parser *parser, t_token *tok)
{
	if (tok->type == TOKEN_WORD && tok->value
		&& tok->value[0] == '?' && tok->value[1] == '\0')
	{
		advance_parser(parser);
		return (expand_exit_status(parser->shell));
	}
	return (NULL);
}

char	*expand_env_var(t_parser *parser, t_token *tok)
{
	char	*value;

	if (tok->type == TOKEN_WORD && tok->value)
	{
		value = get_env_value(parser->shell, tok->value);
		advance_parser(parser);
		if (value)
			return (ft_strdup(value));
		return (ft_strdup(""));
	}
	return (NULL);
}

char	*expand_dollar(t_parser *parser)
{
	t_token	*tok;
	char	*res;

	tok = current_token(parser);
	if (!tok || tok->type == TOKEN_EOF)
		return (ft_strdup("$"));
	if (tok->type == TOKEN_DOLLAR)
	{
		advance_parser(parser);
		return (pid_to_str(getpid()));
	}
	res = expand_exit_code(parser, tok);
	if (res)
		return (res);
	res = expand_env_var(parser, tok);
	if (res)
		return (res);
	return (ft_strdup("$"));
}
