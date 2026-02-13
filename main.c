#include "minishell.h"

volatile sig_atomic_t	g_signal = 0;

int	main(int argc, char **argv, char **envp)
{
	t_shell shell;

	(void)argc;
	(void)argv;
	init_shell(&shell, envp);
	main_loop(&shell);
	cleanup_shell(&shell);
	return (shell.exit_status);
}
