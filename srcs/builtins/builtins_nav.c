#include "minishell.h"

int builtin_pwd(void)
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)))
    {
        printf("%s\n", cwd);
        return (0);
    }
    return (1);
}

int builtin_echo(t_command *cmd)
{
    int i = 1;
    int n_flag = 0;
    if (cmd->argc > 1 && ft_strcmp(cmd->argv[1], "-n") == 0)
    {
        n_flag = 1;
        i++;
    }
    while (i < cmd->argc)
    {
        printf("%s", cmd->argv[i]);
        if (i < cmd->argc - 1) printf(" ");
        i++;
    }
    if (!n_flag) printf("\n");
    return (0);
}

int builtin_cd(t_command *cmd, t_shell *shell)
{
    (void)cmd;
    (void)shell;
    return (0);
}

int builtin_exit(t_command *cmd, t_shell *shell)
{
    (void)cmd;
    shell->should_exit = 1;
    return (0);
}