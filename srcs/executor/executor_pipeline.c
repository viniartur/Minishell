/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 16:15:49 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 18:25:33 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	child_left(t_ast_node *node, t_shell *shell, int *fd)
{
	close(fd[0]);
	dup2(fd[1], STDOUT_FILENO);
	close(fd[1]);
	exit(execute_ast(node->data.pipeline.left, shell));
}

void	child_right(t_ast_node *node, t_shell *shell, int *fd)
{
	close(fd[1]);
	dup2(fd[0], STDIN_FILENO);
	close(fd[0]);
	exit(execute_ast(node->data.pipeline.right, shell));
}

int	handle_pipeline_status(int sl, int sr, t_shell *shell)
{
	(void)sl;
	return (handle_exit_status(sr, shell));
}

char	**dup_argv(char **argv, int argc)
{
	char	**copy;
	int		i;

	copy = malloc(sizeof(char *) * (argc + 1));
	if (!copy)
		return (NULL);
	i = 0;
	while (i < argc)
	{
		copy[i] = ft_strdup(argv[i]);
		if (!copy[i])
		{
			while (i-- > 0)
				free(copy[i]);
			free(copy);
			return (NULL);
		}
		i++;
	}
	copy[i] = NULL;
	return (copy);
}

void	free_argv(char **argv)
{
	int	i;

	if (!argv)
		return ;
	i = 0;
	while (argv[i])
	{
		free(argv[i]);
		i++;
	}
	free(argv);
}
