/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmorais- <tmorais-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 17:40:00 by tmorais-          #+#    #+#             */
/*   Updated: 2026/03/25 14:29:15 by tmorais-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal = 0;

void	close_inherited_fds(void)
{
	DIR				*dir;
	struct dirent	*entry;
	int				fd;
	int				i;

	dir = opendir("/proc/self/fd");
	if (!dir)
		return ;
	entry = readdir(dir);
	while (entry)
	{
		i = 0;
		fd = 0;
		while (entry->d_name[i] >= '0' && entry->d_name[i] <= '9')
			fd = fd * 10 + (entry->d_name[i++] - '0');
		if (fd > 2 && fd != dirfd(dir))
			close(fd);
		entry = readdir(dir);
	}
	closedir(dir);
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;
	int		ret;

	(void)argc;
	(void)argv;
	close_inherited_fds();
	setup_signals();
	init_shell(&shell, envp);
	main_loop(&shell);
	cleanup_shell(&shell);
	rl_clear_history();
	ret = shell.exit_status;
	return (ret);
}
