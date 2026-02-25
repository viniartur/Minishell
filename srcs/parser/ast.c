#include "minishell.h"

t_ast_node	*create_ast_node(t_node_type type)
{
	t_ast_node	*node;

	node = malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = type;
	// initiate union based on type
	if (type == NODE_PIPELINE)
	{
		node->data.pipeline.left = NULL;
		node->data.pipeline.right = NULL;
	}
	else if (type == NODE_COMMAND)
		node->data.cmd = NULL;
	return (node);
}

//	creates a node for command
t_ast_node	*create_command_node(t_command *cmd)
{
	t_ast_node	*node;

	node = create_ast_node(NODE_COMMAND);
	if (!node)
		return (NULL);
	node->data.cmd = cmd;
	return (node);
}

// creates a node for pipeline
t_ast_node	*create_pipeline_node(t_ast_node *left, t_ast_node *right)
{
	t_ast_node	*node;

	node = create_ast_node(NODE_PIPELINE);
	if (!node)
		return (NULL);
	node->data.pipeline.left = left;
	node->data.pipeline.right = right;
	return (node);
}

void	free_redirs(t_redir *redirs)
{
	t_redir	*current;
	t_redir	*next;

	current = redirs;
	while (current)
	{
		next = current->next;
		if (current->file)
			free(current->file);
		free(current);
		current = next;
	}
}

void	free_command(t_command *cmd)
{
	int	i;

	if (!cmd)
		return ;
	if (cmd->argv)
	{
		i = 0;
		while (i < cmd->argc)
		{
			if (cmd->argv[i])
				free(cmd->argv[i]);
			i++;
		}
		free(cmd->argv);
	}
	if (cmd->redirs)
		free_redirs(cmd->redirs);
	free(cmd);
}

void	free_ast(t_ast_node *ast)
{
	if (!ast)
		return ;
	// using recursion for cleaning ast
	if (ast->type == NODE_PIPELINE)
	{
		free_ast(ast->data.pipeline.left);
		free_ast(ast->data.pipeline.right);
	}
	else if (ast->type == NODE_COMMAND)
		free_command(ast->data.cmd);
	free(ast);
}

// DEBUG: prints ast with identation
void	print_ast(t_ast_node *ast, int level)
{
	int	i;

	if (!ast)
		return ;
	// identation
	i = 0;
	while (i < level * 2)
	{
		printf(" ");
		i++;
	}
	if (ast->type == NODE_PIPELINE)
	{
		printf("PIPELINE:\n");
		print_ast(ast->data.pipeline.left, level + 1);
		print_ast(ast->data.pipeline.right, level + 1);
	}
	else if (ast->type == NODE_COMMAND)
	{
		t_command* cmd = ast->data.cmd;
		t_redir* redir;

		printf("COMMAND: ");
		if (cmd->argv && cmd->argc > 0)
		{
			printf("[");
			i = 0;
			while (i < cmd->argc)
			{
				printf("'%s'", cmd->argv[i]);
				if (i < cmd->argc - 1)
					printf(", ");
				i++;
			}
			printf("]");
		}
		printf("\n");
		// print redirs
		redir = cmd->redirs;
		while (redir)
		{
			i = 0;
			while (i < (level + 1) * 2)
			{
				printf(" ");
				i++;
			}
			printf("REDIR: ");
			if (redir->type == TOKEN_REDIR_IN)
				printf("< '%s'", redir->file);
			else if (redir->type == TOKEN_REDIR_OUT)
				printf("> '%s'", redir->file);
			else if (redir->type == TOKEN_APPEND)
				printf(">> '%s'", redir->file);
			else if (redir->type == TOKEN_HEREDOC)
				printf("<< '%s'", redir->file);
			printf("\n");

			redir = redir->next;
		}
	}
}
