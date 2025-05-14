/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melkess <melkess@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 10:01:34 by melkess           #+#    #+#             */
/*   Updated: 2025/05/14 14:50:43 by melkess          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/executor.h"
//TODO: errror 
char	*get_home_dir(t_env *envh)
{
	char	*s;

	s = NULL;
	if (search_for_defaults(envh, "HOME"))
	{
		s = search_for_defaults(envh, "HOME")->value;
		if (!s)
		{
			printf("minishell: HOME not set\n");
			return (NULL);
		}
	}
	return (s);
}

int	cd(t_env **envh, t_tree *cmd, char	**pwd_backup)
{
	char	*dir;
	t_env	*var;

	cmd->cmd++;
	if (!cmd->cmd[0])
	{
		cmd->cmd[0] = get_home_dir(*envh);
		if (!cmd->cmd[0])
			return (1);
	}
	dir = getcwd(0, 0);
	if (dir)
		*pwd_backup = dir;
	if (dir && !chdir(cmd->cmd[0]))
	{
		*envh = edit_env("OLDPWD", ft_strdup(*pwd_backup), *envh, 0);
		*pwd_backup = getcwd(0, 0);
	}
	else if (!dir && !chdir(cmd->cmd[0]))
	{
		puts("HERE1");
		*envh = edit_env("OLDPWD", ft_strdup(*pwd_backup), *envh, 0);
		puts("HERE2");
		if (*pwd_backup[ft_strlen(*pwd_backup) -1] != '/')
		{
			
		puts("AASSS");
			*pwd_backup = ft_strjoin(*pwd_backup, ft_strjoin("/", cmd->cmd[0]));
		puts("ZASSS");
		}
		else
		{
		puts("ASSS");
			
			*pwd_backup = ft_strjoin(*pwd_backup, cmd->cmd[0]);
		puts("ZSSS");
		}
		ft_putstr_fd("cd: error retrieving current directory: getcwd:"
		" cannot access parent directories: No such file or directory", 2);
	}
	else
	{
		ft_putstr_fd(ft_strjoin("minishell: cd: ",
			ft_strjoin(cmd->cmd[0], ": No such file or directory\n")), 2);
		return (1);
	}
	*envh = edit_env("PWD", *pwd_backup, *envh, 0);
	return (0);
}


// void	handle_getcwd()
// {
	
// }

// void	change_dir()
// {
	
// }