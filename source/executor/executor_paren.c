/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_paren.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsadikaj <lsadikaj@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 16:12:07 by lsadikaj          #+#    #+#             */
/*   Updated: 2025/06/02 23:12:08 by lsadikaj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static void	child_paren_exec(t_node *node, char ***envp, t_shell *shell)
{
	t_redirect	red;

	init_redirect(&red);
	// Les redirections sont maintenant dans le nœud
	if (node->redirections && !apply_node_redirections(node, &red))
	{
		close_redirect_fds(&red);
		restore_std_fds(&red);
		exit(1);
	}
	// Exécuter le contenu des parenthèses
	exit(execute_ast(node->left, envp, shell));
}

int	execute_paren_node(t_node *node, char ***envp, t_shell *shell)
{
	pid_t	child_pid;
	int		status;

	if (!node || !node->left)
		return (1);
	child_pid = fork();
	if (child_pid == -1)
	{
		perror("minishell: fork");
		return (1);
	}
	if (child_pid == 0)
		child_paren_exec(node, envp, shell);
	waitpid(child_pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}
