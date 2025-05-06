/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melkess <melkess@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 18:33:52 by melkess           #+#    #+#             */
/*   Updated: 2025/05/05 10:01:24 by melkess          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/executor.h"

int	env(t_env *envh)
{
	char	*env_join;
	char	**env;
	size_t	i;

	i = 0;
	env_join = NULL;
	env = struct_to_darr(envh);
	while (env && env[i])
	{
		env_join = ft_strjoin(env_join, env[i]); // leaks
		env_join = ft_strjoin(env_join, "\n");
		i++;
	}
	ft_putstr_fd(env_join, 1);
	return (0);
}