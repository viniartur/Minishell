/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils3.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 16:07:29 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 16:09:07 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	handle_append_redir(t_redir *r)
{
	int	fd;

	fd = open(r->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
		return (perror(r->file), -1);
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

int	handle_input_redir(t_redir *r)
{
	int	fd;

	fd = open(r->file, O_RDONLY);
	if (fd == -1)
		return (perror(r->file), -1);
	dup2(fd, STDIN_FILENO);
	close(fd);
	return (0);
}
