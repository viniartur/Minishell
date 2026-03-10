#include "minishell.h"

int	builtin_env(t_shell *shell)
{
	int	i;

	i = 0;
	while (shell->env && shell->env[i])
	{
		printf("%s\n", shell->env[i]);
		i++;
	}
	return (0);
}

/*
 * env_count — conta o número de variáveis no array env.
 */
static int	env_count(char **env)
{
	int	i;

	i = 0;
	while (env && env[i])
		i++;
	return (i);
}

/*
 * builtin_export_single — adiciona ou atualiza uma variável no env.
 * Aceita strings no formato "NOME=valor" ou apenas "NOME".
 * Usado internamente por cd para atualizar PWD/OLDPWD.
 */
int	builtin_export_single(char *entry, t_shell *shell)
{
	int		i;
	int		len;
	char	*eq;
	char	**new_env;

	if (!entry || !shell)
		return (1);
	eq = ft_strchr(entry, '=');
	len = eq ? (int)(eq - entry) : (int)ft_strlen(entry);
	i = 0;
	while (shell->env && shell->env[i])
	{
		if (ft_strncmp(shell->env[i], entry, len) == 0
			&& (shell->env[i][len] == '=' || shell->env[i][len] == '\0'))
		{
			free(shell->env[i]);
			shell->env[i] = ft_strdup(entry);
			return (0);
		}
		i++;
	}
	/* Não existe — adiciona ao final */
	new_env = malloc(sizeof(char *) * (env_count(shell->env) + 2));
	if (!new_env)
		return (1);
	i = 0;
	while (shell->env && shell->env[i])
	{
		new_env[i] = shell->env[i];
		i++;
	}
	new_env[i] = ft_strdup(entry);
	new_env[i + 1] = NULL;
	free(shell->env);
	shell->env = new_env;
	return (0);
}

/*
 * is_valid_identifier — verifica se o nome da variável é válido.
 * Deve começar com letra ou '_', seguido de letras, dígitos ou '_'.
 */
static int	is_valid_identifier(const char *str)
{
	int	i;

	if (!str || !str[0])
		return (0);
	if (!((str[0] >= 'a' && str[0] <= 'z')
			|| (str[0] >= 'A' && str[0] <= 'Z') || str[0] == '_'))
		return (0);
	i = 1;
	while (str[i] && str[i] != '=')
	{
		if (!((str[i] >= 'a' && str[i] <= 'z')
				|| (str[i] >= 'A' && str[i] <= 'Z')
				|| (str[i] >= '0' && str[i] <= '9') || str[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

/*
 * builtin_export — sem argumentos: imprime env em formato declare -x.
 * Com argumentos: adiciona/atualiza variáveis no env.
 */
int	builtin_export(t_command *cmd, t_shell *shell)
{
	int	i;

	if (cmd->argc < 2)
	{
		i = 0;
		while (shell->env && shell->env[i])
		{
			printf("declare -x %s\n", shell->env[i]);
			i++;
		}
		return (0);
	}
	i = 1;
	while (i < cmd->argc)
	{
		if (!is_valid_identifier(cmd->argv[i]))
		{
			ft_putstr_fd("minishell: export: `", STDERR_FILENO);
			ft_putstr_fd(cmd->argv[i], STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			i++;
			continue ;
		}
		builtin_export_single(cmd->argv[i], shell);
		i++;
	}
	return (0);
}

/*
 * builtin_unset — remove variáveis do env.
 */
int	builtin_unset(t_command *cmd, t_shell *shell)
{
	int		i;
	int		j;
	int		k;
	size_t	len;

	i = 1;
	while (i < cmd->argc)
	{
		len = ft_strlen(cmd->argv[i]);
		j = 0;
		while (shell->env && shell->env[j])
		{
			if (ft_strncmp(shell->env[j], cmd->argv[i], len) == 0
				&& (shell->env[j][len] == '=' || shell->env[j][len] == '\0'))
			{
				free(shell->env[j]);
				k = j;
				while (shell->env[k + 1])
				{
					shell->env[k] = shell->env[k + 1];
					k++;
				}
				shell->env[k] = NULL;
				break ;
			}
			j++;
		}
		i++;
	}
	return (0);
}