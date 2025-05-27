/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_paren.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimpa <jimpa@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 16:12:07 by lsadikaj          #+#    #+#             */
/*   Updated: 2025/05/26 18:43:29 by lsadikaj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// Applique les redirections et exécute le nœud entre parenthèses
static void	child_paren_exec(t_node *node, char ***envp, t_shell *shell)
{
	t_redirect	red;
	int			status;

	init_redirect(&red);
	process_heredocs(shell);
	if (!apply_all_redirections(shell, &red))
	{
		close_redirect_fds(&red);
		restore_std_fds(&red);
		free_redirections(shell->redirections);
		free_heredocs(shell->heredocs);
		exit(1);
	}
	// Exécuter le contenu des parenthèses dans le processus enfant
	// Les changements d'environnement (export) seront limités à ce sous-shell
	status = execute_ast(node->left, envp, shell);
	free_redirections(shell->redirections);
	free_heredocs(shell->heredocs);
	exit(status);
}

// Exécute un nœud de parenthèses dans un sous-processus isolé
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
