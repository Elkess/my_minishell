/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melkess <melkess@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 10:01:34 by melkess           #+#    #+#             */
/*   Updated: 2025/04/22 09:18:21 by melkess          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/executor.h"

int	cd(t_env **envh, t_tree *cmd)
{
	DIR		*dir;
	char	*_dir;
	t_env	*var;

	cmd->cmd++;
	if (!cmd->cmd[0])
	{
		var = search_for_defaults(*envh, "HOME");
		if (!var)
		{
			printf("minishell: HOME not set\n");
			return (1);
		}
		cmd->cmd[0] = var->value;
	}
	_dir = getcwd(NULL, 0);
	if (!_dir)
	{
		printf("bash: cd: getcwd Failed\n");
		return (1);
	}
	dir = opendir(cmd->cmd[0]);
	if (!dir)
	{
		printf("bash: cd: %s : No such file or directory\n", cmd->cmd[0]);
		return (1);
	}
	if (chdir(cmd->cmd[0]) == -1)
	{
		printf("bash: cd: %s : chdir Failed\n", cmd->cmd[0]);
		return (1);
	}
	*envh = edit_env("OLDPWD", _dir, *envh, 0);
	// free(_dir); //??
	_dir = getcwd(NULL, 0);
	if (!_dir)
	{
		printf("bash: cd: getcwd Failed\n");
		return (1);
	}
	*envh = edit_env("PWD", _dir, *envh, 0);
	// free(_dir);
	closedir(dir); // ??
	return (0);
}
