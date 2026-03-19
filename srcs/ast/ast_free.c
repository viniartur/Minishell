/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_free.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 18:00:41 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/19 17:38:40 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_redirs(t_redir *redirs)
{
	t_redir	*current;
	t_redir	*next;

	current = redirs;
	while (current)
	{
		next = current->next;
		if (current->file)
		{
			free(current->file);
			current->file = NULL;
		}
		if (current->content)
		{
			free(current->content);
			current->content = NULL;
		}
		free(current);
		current = next;
	}
}

void	free_command(t_command *cmd)
{
	int	i;

	if (!cmd)
		return ;
	if (cmd->argv)
	{
		i = 0;
		while (i < cmd->argc)
		{
			if (cmd->argv[i])
			{
				free(cmd->argv[i]);
				cmd->argv[i] = NULL;
			}
			i++;
		}
		free(cmd->argv);
		cmd->argv = NULL;
	}
	if (cmd->redirs)
	{
		free_redirs(cmd->redirs);
		cmd->redirs = NULL;
	}
	free(cmd);
}

void	free_ast(t_ast_node *ast)
{
	if (!ast)
		return ;
	if (ast->type == NODE_PIPELINE)
	{
		if (ast->data.pipeline.left)
		{
			free_ast(ast->data.pipeline.left);
			ast->data.pipeline.left = NULL;
		}
		if (ast->data.pipeline.right)
		{
			free_ast(ast->data.pipeline.right);
			ast->data.pipeline.right = NULL;
		}
	}
	else if (ast->type == NODE_COMMAND)
	{
		if (ast->data.cmd)
		{
			free_command(ast->data.cmd);
			ast->data.cmd = NULL;
		}
	}
	free(ast);
}
