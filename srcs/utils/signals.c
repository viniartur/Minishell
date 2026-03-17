#include "minishell.h"

// O manipulador de sinal para o modo interativo
void	handle_signals(int sig)
{
	if (sig == SIGINT)
	{
		g_signal = sig; // Atualiza a variável global permitida [cite: 105]
		write(STDOUT_FILENO, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

// Configura os sinais para o loop principal do shell
void	setup_signals(void)
{
	signal(SIGINT, handle_signals);  // Ctrl+C -> Nova linha no prompt 
	signal(SIGQUIT, SIG_IGN);       // Ctrl+\ -> Ignorar 
	signal(SIGTSTP, SIG_IGN);       // Ctrl+Z -> Ignorar
}