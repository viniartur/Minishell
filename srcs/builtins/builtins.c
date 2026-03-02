#include "minishell.h"

int is_builtin(char *cmd)
{
    if (!cmd) return (0);
    if (ft_strcmp(cmd, "echo") == 0 || ft_strcmp(cmd, "cd") == 0 ||
        ft_strcmp(cmd, "pwd") == 0 || ft_strcmp(cmd, "export") == 0 ||
        ft_strcmp(cmd, "unset") == 0 || ft_strcmp(cmd, "env") == 0 ||
        ft_strcmp(cmd, "exit") == 0)
        return (1);
    return (0);
}

int exec_builtin(t_command *cmd, t_shell *shell)
{
    char *name = cmd->argv[0];

    if (ft_strcmp(name, "echo") == 0)
        return (builtin_echo(cmd));
    if (ft_strcmp(name, "cd") == 0)
        return (builtin_cd(cmd, shell));
    if (ft_strcmp(name, "pwd") == 0)
        return (builtin_pwd());
    if (ft_strcmp(name, "env") == 0)
        return (builtin_env(shell));
    if (ft_strcmp(name, "export") == 0)
        return (builtin_export(cmd, shell));
    if (ft_strcmp(name, "unset") == 0)
        return (builtin_unset(cmd, shell));
    if (ft_strcmp(name, "exit") == 0)
        return (builtin_exit(cmd, shell));
    return (1);
}