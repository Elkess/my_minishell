/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melkess <melkess@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 18:51:01 by melkess           #+#    #+#             */
/*   Updated: 2025/05/03 10:54:34 by melkess          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/executor.h"

char	**struct_to_darr(t_env *envh)
{
	size_t	len;
	t_env	*tmp;
	char	**env;

	len = ft_envlen(envh);
	env = malloc((len +1) * sizeof(char *));
	if (!env)
		exit (1);
	len = 0;
	while (envh)
	{
		if (envh->value)
		{
			env[len] = ft_strjoin(envh->key, "=");
			env[len] = ft_strjoin(env[len], envh->value);
			len++;
		}
		envh = envh->next;
	}
	return (env[len] = NULL, env);
}

void	is_dir(char **p, char *path)
{
	struct stat	s;

	if (!stat(path, &s) && S_ISDIR(s.st_mode))
	{
		if (p && !ft_strcmp(path, ".."))
		{
			printf("minishell1: %s: command not found\n", path);
			exit (127);
		}
		else
		{
			printf("minishell2: %s: is a directory\n", path);
			exit (126);
		}
	}
}

void	exec_helper(char **cmd, char **env, t_env *envh, char **path)
{
	size_t		i;

	i = 0;
	while (path && path[i])
	{
		path[i] = ft_strjoin(path[i], "/");
		path[i] = ft_strjoin(path[i], cmd[0]);
		if (!access(path[i], X_OK))
		{
			is_dir(path, path[i]);
			if (execve(path[i], cmd, env) == -1)
				(perror("Execve2 Failed:"), exit(1));
		}
		i++;
	}
}

void	execute_one(t_tree *cmd, t_env *envh)
{
	pid_t		fd = fork();
	char		**env;
	char		**path;

	path = NULL;
	if (search_for_defaults(envh, "PATH"))
		path = ft_split(search_for_defaults(envh, "PATH")->value, ':');
	env = struct_to_darr(envh);
	if (fd == -1)
		(perror("Fork Failed:"), exit(1));
	if (fd == 0)
	{
		if (!access(cmd->cmd[0], X_OK))
		{
			is_dir(path, cmd->cmd[0]);
			if (execve(cmd->cmd[0], cmd->cmd, env) == -1)
				(perror("Execve1 Failed:"), exit(1));
		}
		exec_helper(cmd->cmd, env, envh, path);
		if (!path || ft_strchr(cmd->cmd[0], '/'))
			(printf("minishell3: %s: No such file or directory\n", cmd->cmd[0]), exit(127));
		else
		{
			write(2, "not found\n", 11);
			(printf("minishell: %s:command not found \n", cmd->cmd[0]), exit(127));
		}
	}
}

void	ft_dup(int *io, int flag)
{
	if (flag)
	{
		io[0] = dup(0);
		io[1] = dup(1);
		if (io[0] == -1 || io[1] == -1)
			(close(io[0]), close(io[1]));
	}
	else
	{
		if (dup2(io[0], 0) == -1 || dup2(io[1], 1) == -1)
			(close(io[0]), close(io[1]));
		(close(io[0]), close(io[1]));
	}
}

t_redir	*find_lastredir(t_redir *redirs, t_redir_type type)
{
	t_redir	*last;

	last = NULL;
	if (type == REDIR_OUT || type == REDIR_APPEND)
	{
		while (redirs)
		{
			if (redirs->type == REDIR_OUT || redirs->type == REDIR_APPEND)
				last = redirs;
			redirs = redirs->next;
		}
	}
	if (type == REDIR_IN || type == REDIR_HEREDOC)
	{
		while (redirs)
		{
			if (redirs->type == REDIR_IN || redirs->type == REDIR_HEREDOC)
				last = redirs;
			redirs = redirs->next;
		}
	}
	return (last);
}

int	handle_lastredir(t_redir *redirs)
{
	t_redir	*lastin;
	t_redir	*lastout;
	lastin = find_lastredir(redirs, REDIR_IN);
	lastout = find_lastredir(redirs, REDIR_OUT);

	if (lastin && lastin->type != REDIR_HEREDOC && (!lastout || (lastout && lastin->index < lastout->index)))
	{
		lastin->fd = open(lastin->file, O_RDONLY);
		if (lastin->fd == -1)
			return (perror(ft_strjoin("minishell5: ", lastin->file)), 1);
	}
	if (lastout && lastout->type == REDIR_OUT)
		lastout->fd = open(lastout->file, O_CREAT | O_TRUNC | O_WRONLY, 0644);
	else if (lastout && lastout->type == REDIR_APPEND)
		lastout->fd = open(lastout->file, O_CREAT | O_APPEND | O_WRONLY, 0644);
	if (lastout && lastout->fd == -1)
		return (perror(ft_strjoin("minishell6: ", lastout->file) ), 1);
	if (lastin && lastin->type != REDIR_HEREDOC && lastout && lastin->index > lastout->index)
	{
		lastin->fd = open(lastin->file, O_RDONLY);
		if (lastin->fd == -1)
			return (perror(ft_strjoin("minishell7: ", lastin->file)), 1);
	}
	if (lastin)
		dup2(lastin->fd, 0);
	if (lastout) 
		dup2(lastout->fd, 1);
	return (0);
}

int	ft_redir(t_tree *tree)
{
	t_redir	*red;

	red = tree->redirs;
	while (red)
	{
		// if (red != find_lastredir(tree->redirs, red->type) && red->type != REDIR_HEREDOC)
		// {
			if (red->type == REDIR_IN)
				red->fd = open(red->file, O_RDONLY);
			else if (red->type == REDIR_OUT)
				red->fd = open(red->file, O_CREAT | O_TRUNC | O_WRONLY, 0644);
			else if (red->type == REDIR_APPEND)
				red->fd = open(red->file, O_CREAT | O_APPEND | O_WRONLY, 0644);
			if (red->fd == -1)
			{
				perror(ft_strjoin("minishell8: ", red->file));
				return (1);
			}
		// }
		if (red != find_lastredir(tree->redirs, REDIR_OUT) &&
			red != find_lastredir(tree->redirs, REDIR_IN))
			close(red->fd);
		red = red->next;
	}
	return (handle_lastredir(tree->redirs));
}

char	*generate_file(t_redir *red)
{
	char	*str;
	size_t	i;

	str = NULL;
	i = 0;
	while (1)
	{
		str = ft_strjoin("/tmp/.here_doc", ft_itoa(i)); //leaks
		if (access(str, F_OK) != 0)
		{
			red->fd = open(str, O_CREAT | O_RDWR | O_APPEND);
			if (red->fd == -1)
				(perror("Open failed in << :"), exit (1));
			break ;
		}
		i++;
	}
	return (str);
}

void	here_doc(t_redir *red)
{
	char	*line;
	char	*file;

	line = NULL;
	while (red)
	{
		if (red->type == REDIR_HEREDOC)
		{
			file = generate_file(red);
			while (1)
			{//TODO: 1 handle heredocs with unlink and fd
				line = readline("> ");
				if (!line)
					exit(1);
				if (!ft_strcmp(line, red->file))
					break ;
				line = ft_strjoin(line, "\n");
				write(red->fd, line, ft_strlen(line)); //TODO :write all at once
			}
			close(red->fd);
			red->fd = open(file, O_CREAT | O_RDWR | O_APPEND);
			if (red->fd == -1)
				(perror("Open failed in << :"), exit (1));
			unlink(file);
		}
		red = red->next;
	}
}

t_redir	*find_lasthd(t_redir *redirs)
{
	t_redir	*last;

	last = NULL;
	while (redirs)
	{
		if (redirs->type == REDIR_HEREDOC)
			last = redirs;
		redirs = redirs->next;
	}
	return (last);
}

int	execute_cmd(t_tree *tree, t_env *envh, int status)
{
	char	*cmd;

	cmd = tree->cmd[0];
	if (!cmd)
		return (0);
	else if (!ft_strcmp(cmd, "echo"))
		status = echo(tree);
	else if (!ft_strcmp(cmd, "cd"))
		status = cd(&envh, tree);
	else if (!ft_strcmp(cmd, "export"))
		status = ft_export(&envh, tree);
	else if (!ft_strcmp(cmd, "unset"))
		status = unset(&envh, tree->cmd);
	else if (!ft_strcmp(cmd, "env"))
		status = env(envh);
	else if (!ft_strcmp(cmd, "pwd"))
		status = pwd(1);
	else if (!ft_strcmp(cmd, "exit"))
		ft_exit(tree, 0);
	else
	{
		(execute_one(tree, envh), waitpid(0, &status, 0));
		status = WEXITSTATUS(status);
	}
	return (status);
}

int	executor(t_tree *tree, t_env *envh)
{
	int		fds[2];
	int		redir_status = 0;
	int		status;
	t_redir	lasthd;

	if (!tree || tree->type != NODE_COMMAND)
		return (1);
	ft_dup(fds, 1);
	if (tree->redirs)
	{
		// lasthd = find_lasthd(tree->redirs);
		// TODO: 2 handle herdoc properly this is wrong
		// if (lasthd)
		// 	here_doc(tree->redirs);
		redir_status = ft_redir(tree);
	}
	if (!redir_status)
		status = execute_cmd(tree, envh, status);
	ft_dup(fds, 0);
	return (status);
}
