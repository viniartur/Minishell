/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 18:01:27 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/10 18:01:29 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_ast_node	*create_ast_node(t_node_type type)
{
	t_ast_node	*node;

	node = malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = type;
	if (type == NODE_PIPELINE)
	{
		node->data.pipeline.left = NULL;
		node->data.pipeline.right = NULL;
	}
	else if (type == NODE_COMMAND)
		node->data.cmd = NULL;
	return (node);
}

t_ast_node	*create_command_node(t_command *cmd)
{
	t_ast_node	*node;

	node = create_ast_node(NODE_COMMAND);
	if (!node)
		return (NULL);
	node->data.cmd = cmd;
	return (node);
}

t_ast_node	*create_pipeline_node(t_ast_node *left, t_ast_node *right)
{
	t_ast_node	*node;

	node = create_ast_node(NODE_PIPELINE);
	if (!node)
		return (NULL);
	node->data.pipeline.left = left;
	node->data.pipeline.right = right;
	return (node);
}
