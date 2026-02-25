#include "minishell.h"

static char	*find_path_variable(char **env)
{
	int	i;

	i = 0;
	while (env && env[i])
	{
		if (ft_strncmp(env[i], "PATH=", 5) == 0)
			return (env[i] + 5);
		i++;
	}
	return (NULL);
}

static char	*check_access(char **paths, char *cmd)
{
	char	*tmp;
	char	*full_path;
	int		i;

	i = 0;
	while (paths && paths[i])
	{
		tmp = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(tmp, cmd);
		free(tmp);
		if (access(full_path, X_OK) == 0)
			return (full_path);
		free(full_path);
		i++;
	}
	return (NULL);
}

char	*get_command_path(char *cmd, char **env)
{
	char	**paths;
	char	*path_var;
	char	*resolved_path;
	int		i;

	if (!cmd || !*cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	path_var = find_path_variable(env);
	if (!path_var)
		return (NULL);
	paths = ft_split(path_var, ':');
	resolved_path = check_access(paths, cmd);
	i = 0;
	while (paths && paths[i])
		free(paths[i++]);
	free(paths);
	return (resolved_path);
}