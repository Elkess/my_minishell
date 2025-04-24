/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melkess <melkess@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 18:33:52 by melkess           #+#    #+#             */
/*   Updated: 2025/04/22 09:21:33 by melkess          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/executor.h"

int	env(t_env *envh)
{
	while (envh)
	{
		if (envh->value)
			printf("%s=%s\n", envh->key, envh->value);
		envh = envh->next;
	}
	return (0);
}