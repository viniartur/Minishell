#include "minishell.h"

void	display_prompt(void)
{
	if (isatty(STDIN_FILENO))
		write(STDOUT_FILENO, PROMPT, sizeof(PROMPT) - 1);
}

static char	*get_non_interactive_input(void)
{
	char	buffer[4096];
	char	*input;

	if (fgets(buffer, sizeof(buffer), stdin) == NULL)
		return (NULL);
	buffer[ft_strcspn(buffer, "\n")] = '\0';
	input = ft_strdup(buffer);
	return (input);
}

char	*get_user_input(void)
{
	char	*input;

	if (isatty(STDIN_FILENO))
		input = readline(PROMPT);
	else
		input = get_non_interactive_input();
	return (input);
}

static int	has_content(const char *input)
{
	int	i;

	i = 0;
	while (input[i])
	{
		if (!ft_isspace(input[i]))
			return (1);
		i++;
	}
	return (0);
}

void	add_to_history(char *input)
{
	if (!input || !*input)
		return ;
	if (!has_content(input))
		return ;
	add_history(input);
}
