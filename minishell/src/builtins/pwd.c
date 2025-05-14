/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melkess <melkess@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 13:23:47 by melkess           #+#    #+#             */
/*   Updated: 2025/05/14 13:38:42 by melkess          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/executor.h"
// TODO: she is going to read from the hiden var  
int	pwd(char **pwd_backup, int print)
{
	char		*pwd;

	pwd = getcwd(NULL, 0);
	if (!pwd)
		pwd  = *pwd_backup;
	if (print)
		printf("%s\n", pwd);
	return (0);
}
