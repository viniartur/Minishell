/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_free.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 18:00:41 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/10 18:01:13 by tmorais-         ###   ########.fr       */
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
			free(current->file);
		if (current->content)
			free(current->content);
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
				free(cmd->argv[i]);
			i++;
		}
		free(cmd->argv);
	}
	if (cmd->redirs)
		free_redirs(cmd->redirs);
	free(cmd);
}

void	free_ast(t_ast_node *ast)
{
	if (!ast)
		return ;
	if (ast->type == NODE_PIPELINE)
	{
		free_ast(ast->data.pipeline.left);
		free_ast(ast->data.pipeline.right);
	}
	else if (ast->type == NODE_COMMAND)
		free_command(ast->data.cmd);
	free(ast);
}
