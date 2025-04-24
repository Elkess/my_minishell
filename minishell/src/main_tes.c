/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_tes.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melkess <melkess@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 15:21:18 by sgmih             #+#    #+#             */
/*   Updated: 2025/04/24 16:23:45 by melkess          ###   ########.fr       */
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

void leaks(){system("leaks minishell");}

int	main(int ac, char **av, char **env)
{
	(void)ac;
	(void)av;
	char	*line;
	t_tree	*tree;
	t_env	*envh;

	// printf("{%d}", getpid());
	envh = fill_env(env);
	while (1)
	{//TODO: Handle SHLVL, 
	//TODO:leaks, nrminette,o export a="111" && echo $a , change from pipes to fd in herdoc
		line = ft_get_prompt(0);
		tree = malloc(sizeof(t_tree));
		tree->type = NODE_COMMAND;
		tree->cmd = ft_split(line, ' ');

		if (tree)
			tree->redirs = NULL; // Deactivate the reds
		if (line && *line)
			execute_tree(tree, envh);
	}
	// TODO: Freeeeeeeeeeeeee;

	return (0);
}
