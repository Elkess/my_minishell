/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_tes.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melkess <melkess@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 15:21:18 by sgmih             #+#    #+#             */
/*   Updated: 2025/05/14 16:08:57 by melkess          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <readline/readline.h>
#include "../include/executor.h"
#include <errno.h>
//cc -lreadline src/main_tes.c src/builtins/*.c src/executor/*.c src/env/*.c  library/libft/*.c -o minishell && env -i ./minishell

static char	*ft_get_prompt(int exit_status)
{
	char	*prompt;

	prompt = readline("minishell$> ");
	if (prompt && prompt[0])
		add_history(prompt);
// if (!prompt) {
//     // readline returns NULL on Ctrl+D (EOF) or real error
//     if (errno != 0) {
//         perror("readline");  // Only print if errno is set
//         exit(1);
//     } else {
//         // Probably just Ctrl+D (EOF)
//         printf("exit\n");
//         exit(0);
//     }
// }

	return (prompt);
}
static t_tree *create_command_node(char **cmd)
{
    t_tree *node = malloc(sizeof(t_tree));
    if (!node)
        return NULL;
    node->type = NODE_COMMAND;
    node->cmd = cmd;
    node->redirs = NULL;
    node->is_ambiguous = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Helper to create a redirection node
static t_redir *create_redir(t_redir_type type, char *file, size_t index)
{
    t_redir *redir = malloc(sizeof(t_redir));
    if (!redir)
        return NULL;
    redir->type = type;
    redir->file = file;
    redir->index = index;
    redir->fd = -1;
    redir->next = NULL;
    return redir;
}

// Parse a command and extract redirections
static t_tree *parse_command(char **tokens, size_t start, size_t end)
{
    t_tree *node;
    char **cmd;
    size_t i, j;
    t_redir *redir_list = NULL;
    t_redir *last_redir = NULL;

    cmd = malloc(sizeof(char *) * (end - start + 1));
    if (!cmd)
        return NULL;
    i = start;
    j = 0;
    while (i < end)
    {
        if (strcmp(tokens[i], "<") == 0 && i + 1 < end)
        {
            t_redir *redir = create_redir(REDIR_IN, strdup(tokens[i + 1]), i);
            if (!redir)
                return NULL;
            if (!redir_list)
                redir_list = redir;
            else
                last_redir->next = redir;
            last_redir = redir;
            i += 2;
        }
        else if (strcmp(tokens[i], ">") == 0 && i + 1 < end)
        {
            t_redir *redir = create_redir(REDIR_OUT, strdup(tokens[i + 1]), i);
            if (!redir)
                return NULL;
            if (!redir_list)
                redir_list = redir;
            else
                last_redir->next = redir;
            last_redir = redir;
            i += 2;
        }
        else if (strcmp(tokens[i], ">>") == 0 && i + 1 < end)
        {
            t_redir *redir = create_redir(REDIR_APPEND, strdup(tokens[i + 1]), i);
            if (!redir)
                return NULL;
            if (!redir_list)
                redir_list = redir;
            else
                last_redir->next = redir;
            last_redir = redir;
            i += 2;
        }
        else if (strcmp(tokens[i], "<<") == 0 && i + 1 < end)
        {
            t_redir *redir = create_redir(REDIR_HEREDOC, strdup(tokens[i + 1]), i);
            if (!redir)
                return NULL;
            if (!redir_list)
                redir_list = redir;
            else
                last_redir->next = redir;
            last_redir = redir;
            i += 2;
        }
        else
            cmd[j++] = strdup(tokens[i++]);
    }
    cmd[j] = NULL;
    node = create_command_node(cmd);
    if (!node)
        return NULL;
    node->redirs = redir_list;
    return node;
}

// Main recursive parser
static t_tree *parse_tokens(char **tokens, size_t start, size_t end)
{
    size_t i;
    t_tree *node;

    i = start;
    while (i < end)
    {
        if (strcmp(tokens[i], "|") == 0 || strcmp(tokens[i], "&&") == 0 || strcmp(tokens[i], "||") == 0)
        {
            node = malloc(sizeof(t_tree));
            if (!node)
                return NULL;
            if (strcmp(tokens[i], "|") == 0)
                node->type = NODE_PIPE;
            else if (strcmp(tokens[i], "&&") == 0)
                node->type = NODE_AND;
            else
                node->type = NODE_OR;
            node->is_ambiguous = 0;
            node->cmd = NULL;
            node->redirs = NULL;
            node->left = parse_tokens(tokens, start, i);
            node->right = parse_tokens(tokens, i + 1, end);
            return node;
        }
        i++;
    }
    return parse_command(tokens, start, end);
}
size_t	ft_arrlen(char **arr)
{
	size_t	i = 0;
	while (arr && arr[i])
		i++;
	return (i);
}

// TEMP parser entry point
t_tree *temporary_parse(char *line)
{
    char **tokens;

    tokens = ft_split(line, ' '); // Simple split by space
    if (!tokens)
        return NULL;
    return parse_tokens(tokens, 0, (size_t)ft_arrlen(tokens));
}

static void print_redirs(t_redir *redir)
{
    while (redir)
    {
        if (redir->type == REDIR_IN)
            printf("  [<] Input: %s\n", redir->file);
        else if (redir->type == REDIR_OUT)
            printf("  [>] Output: %s\n", redir->file);
        else if (redir->type == REDIR_APPEND)
            printf("  [>>] Append: %s\n", redir->file);
        else if (redir->type == REDIR_HEREDOC)
            printf("  [<<] Here-doc: %s\n", redir->file);
        redir = redir->next;
    }
}

static void print_indent(int level)
{
    for (int i = 0; i < level; i++)
        printf("    "); // 4 spaces per level
}

void print_tree(t_tree *tree, int level)
{
    if (!tree)
        return;
    print_indent(level);
    if (tree->type == NODE_COMMAND)
    {
        printf("NODE_COMMAND:\n");
        if (tree->cmd)
        {
            print_indent(level + 1);
            printf("CMD:");
            for (int i = 0; tree->cmd[i]; i++)
                printf(" %s", tree->cmd[i]);
            printf("\n");
        }
        if (tree->redirs)
        {
            print_indent(level + 1);
            printf("REDIRS:\n");
            print_redirs(tree->redirs);
        }
    }
    else if (tree->type == NODE_PIPE)
        printf("NODE_PIPE:\n");
    else if (tree->type == NODE_AND)
        printf("NODE_AND:\n");
    else if (tree->type == NODE_OR)
        printf("NODE_OR:\n");
    if (tree->left)
    {
        print_indent(level);
        printf("LEFT:\n");
        print_tree(tree->left, level + 1);
    }
    if (tree->right)
    {
        print_indent(level);
        printf("RIGHT:\n");
        print_tree(tree->right, level + 1);
    }
}

void	handle_herdocs(t_tree *tree)
{
	if (!tree)
		return ;
	here_docs(tree->redirs);
	handle_herdocs(tree->left);
	handle_herdocs(tree->right);
}

void leaks(){system("leaks minishell");}

int	main(int ac, char **av, char **env)
{
	// atexit(leaks);
	(void)ac;
	(void)av;
	char	*line;
	t_tree	*tree;
	t_env	*envh;

	// printf("{%d}", getpid());
	envh = fill_env(env);
	while (1)
	{
	//TODO :handle herdoc properly
    // TODO: change all the eroors from printf to perror
		line = ft_get_prompt(0);
		// tree = malloc(sizeof(t_tree));
		// tree->type = NODE_COMMAND;
		// tree->cmd = ft_split(line, ' ');
		tree = temporary_parse(line);
		// print_tree(tree, 0);
		handle_herdocs(tree);
		// tree->redirs = NULL; // Deactivate the reds
        // puts(line);
		if (line && *line)
			printf("exit status: %d\n", execute_tree(tree, envh));
	}
	// TODO: Freeeeeeeeeeeeee;
	return (0);
}
