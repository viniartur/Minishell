#include "minishell.h"

int builtin_env(t_shell *shell)
{
    int i = 0;
    while (shell->env && shell->env[i])
    {
        printf("%s\n", shell->env[i]);
        i++;
    }
    return (0);
}

int builtin_unset(t_command *cmd, t_shell *shell)
{
    (void)cmd;
    (void)shell;
    return (0);
}

int builtin_export(t_command *cmd, t_shell *shell)
{
    (void)cmd;
    (void)shell;
    return (0);
}