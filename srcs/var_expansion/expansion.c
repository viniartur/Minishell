#include "minishell.h"

// Percorre o array shell->env procurando por "NOME=valor"
char	*get_env_value(t_shell *shell, const char *var_name)
{
	int		i;
	size_t	len;

	if (!shell || !shell->env || !var_name)
		return (NULL);
	len = ft_strlen(var_name);
	i = 0;
	while (shell->env[i])
	{
		if (ft_strncmp(shell->env[i], var_name, len) == 0 &&
		shell->env[i][len] == '=')
			return (shell->env[i] + len + 1); // retorna valor após '='
		i++;
	}
	return (NULL); // se variável não existir
}

// Expande $? para o status de saída (como string)
char	*expand_exit_status(t_shell *shell)
{
	char	*status_str;
	int		status;
	int		temp;
	int		digits;

	if (!shell)
		return (ft_strdup("0"));
	status = shell->exit_status;
	// Garante que status nunca seja negativo (ex: sinal matou processo)
	if (status < 0)
		status = 255;
	digits = 1;
	temp = status;
	while (temp >= 10)
	{
		temp /= 10;
		digits++;
	}
	status_str = malloc(sizeof(char) * (digits + 1));
	if (!status_str)
		return (NULL);
	status_str[digits] = '\0';
	temp = status;
	while (digits > 0)
	{
		digits--;
		status_str[digits] = (temp % 10) + '0';
		temp /= 10;
	}
	return (status_str);
}

char	*expand_variable(t_shell *shell, const char *str, int *i)
{
	char	*var_name;
	char	*value;
	char	*result;

	if (str[*i] == '?')
	{
		(*i)++;
		return (expand_exit_status(shell)); // CORRIGIDO: passou shell, não (str, *i)
	}
	var_name = extract_var_name(str, *i);
	if (!var_name)
		return (ft_strdup("$")); // sozinho ou seguido de caractere inválido
	*i += ft_strlen(var_name); // avança até o final do nome da variável
	value = get_env_value(shell, var_name);
	free(var_name);
	if (value)
		result = ft_strdup(value);
	else
		result = ft_strdup(""); // para string vazia / variável não definida
	return (result);
}

/*
Função principal: expande todas as variáveis na string.

NOTA SOBRE ASPAS:
Esta função recebe o VALOR já extraído pelo lexer, sem as aspas delimitadoras.
O controle de aspas (expandir ou não) é responsabilidade do parser:
  - TOKEN_QUOTE  (aspas simples): o parser NÃO chama esta função, usa ft_strdup direto
  - TOKEN_DQUOTE (aspas duplas):  o parser CHAMA esta função — expande $VAR
  - TOKEN_WORD   (sem aspas):     o parser CHAMA esta função — expande $VAR

Portanto, toda string que chega aqui DEVE ter suas variáveis expandidas.
O código de controle de aspas abaixo foi removido pois causava o bug:
  echo "$HOME"  -> expandia corretamente   ✓
  echo '$HOME'  -> também expandia (errado) ✗
*/
char	*expand_all_variables(t_shell *shell, const char *str)
{
	int		i;
	char	*result;
	char	*expanded;

	if (!str)
		return (NULL);
	result = ft_strdup("");
	if (!result)
		return (NULL);
	i = 0;
	while (str[i])
	{
		if (str[i] == '$')
		{
			i++;
			expanded = expand_variable(shell, str, &i);
			if (expanded)
			{
				result = join_strings(result, expanded);
				free(expanded);
			}
			continue ;
		}
		expanded = malloc(sizeof(char) * 2);
		if (expanded)
		{
			expanded[0] = str[i];
			expanded[1] = '\0';
			result = join_strings(result, expanded);
			free(expanded);
		}
		i++;
	}
	return (result);
}
