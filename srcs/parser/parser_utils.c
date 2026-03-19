/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 18:00:00 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/19 15:10:52 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*get_next_token(t_token **tokens)
{
	t_token	*current;

	if (!tokens || !*tokens)
		return (NULL);
	current = *tokens;
	*tokens = (*tokens)->next;
	return (current);
}

int	match(t_token **tokens, t_token_type type)
{
	if (!tokens || !*tokens)
		return (FALSE);
	return ((*tokens)->type == type);
}

void	expect(t_token **tokens, t_token_type type)
{
	if (!tokens || !*tokens)
	{
		printf("minishell: syntax error: unexpected EOF\n");
		return ;
	}
	if ((*tokens)->type != type)
	{
		printf("minishell: syntax error near unexpected token\n");
		return ;
	}
	*tokens = (*tokens)->next;
}

t_command	*create_command(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->argv = NULL;
	cmd->argc = 0;
	cmd->redirs = NULL;
	return (cmd);
}

/* ===== FUNÇÃO CORRIGIDA - RECEBE STRING JÁ ALOCADA ===== */
void	add_argument(t_command *cmd, char *arg)
{
	char	**new_argv;
	int		i;

	if (!cmd || !arg)
	{
		if (arg)
			free(arg);
		return ;
	}
	new_argv = malloc(sizeof(char *) * (cmd->argc + 2));
	if (!new_argv)
	{
		free(arg);
		return ;
	}
	i = 0;
	while (i < cmd->argc)
	{
		new_argv[i] = cmd->argv[i];
		i++;
	}
	new_argv[i] = arg;
	new_argv[i + 1] = NULL;
	free(cmd->argv);
	cmd->argv = new_argv;
	cmd->argc++;
}

/* ===== FUNÇÃO CORRIGIDA - RECEBE FILE JÁ ALOCADO ===== */
t_redir	*create_redirection(int type, char *file, int expand)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
	{
		if (file)
			free(file);
		return (NULL);
	}
	redir->type = type;
	redir->file = file;
	redir->content = NULL;
	redir->expand = expand;
	redir->fd = -1;
	redir->next = NULL;
	return (redir);
}

void	add_redirection(t_command *cmd, t_redir *redir)
{
	t_redir *last;

	if (!cmd || !redir)
		return ;
	if (!cmd->redirs)
	{
		cmd->redirs = redir;
		return ;
	}
	last = cmd->redirs;
	while (last->next)
		last = last->next;
	last->next = redir;
}
