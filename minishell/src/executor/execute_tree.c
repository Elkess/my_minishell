/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_tree.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melkess <melkess@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 08:39:55 by melkess           #+#    #+#             */
/*   Updated: 2025/04/24 14:22:43 by melkess          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/executor.h"

int	execute_pipes(t_tree *tree, t_env *envh)
{
	int		pipefd[2];
	pid_t	left_pid;
	pid_t	right_pid;
	int		status;

	if (tree->type != NODE_PIPE)
		return (-1);
	if (pipe(pipefd) == -1){perror("Pipe failed");return (-1);}
	left_pid = fork();
	if (left_pid == -1)	{perror("Fork failed");return (-1);}
	if (left_pid == 0)
	{
		close(pipefd[0]);
		dup2(pipefd[1], 1);
		close(pipefd[1]);
		exit(execute_tree(tree->left, envh));
	}
	right_pid = fork();
	if (right_pid == -1){perror("Fork failed");return (-1);}
	if (right_pid == 0)
	{
		close(pipefd[1]);
		dup2(pipefd[0], 0);
		close(pipefd[0]);
		exit(execute_tree(tree->right, envh));
	}
	(close(pipefd[0]), close(pipefd[1]));
	(waitpid(left_pid, NULL, 0), waitpid(right_pid, &status, 0));
	return (WEXITSTATUS(status));
}

int	execute_tree(t_tree *tree, t_env *envh)
{
	int	status;

	status = 1;
	if (!tree)
		return (status);
	if (tree->type == NODE_COMMAND)
		return (executor(tree, envh));
	if (tree->type == NODE_PIPE)
		return (execute_pipes(tree, envh));
	if (tree->type == NODE_AND)
	{
		status = execute_tree(tree->left, envh);
		if (status == 0)
			return (execute_tree(tree->right, envh));
		return (status);
	}
	if (tree->type == NODE_OR)
	{
		status = execute_tree(tree->left, envh);
		if (status != 0)
			return (execute_tree(tree->right, envh));
		return (status);
	}
	return (status);
}
