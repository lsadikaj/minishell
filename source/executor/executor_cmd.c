/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_cmd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsadikaj <lsadikaj@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 14:22:13 by lsadikaj          #+#    #+#             */
/*   Updated: 2025/04/15 17:21:03 by lsadikaj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// Execute la commande dans le processus enfant
static void	exec_child_process(t_node *node, char **envp)
{
	char	*path;

	// Restaurer les gestionnaires de signaux par défaut
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	path = ft_path_finder(node->cmd[0]);
	if (!path)
	{
		ft_printf("minishell: %s: command not found\n", node->cmd[0]);
		exit(127);
	}
	execve(path, node->cmd, envp);
	perror("minishell");
	free(path);
	exit(1);
}

// Traite le status de sortie du processus enfant
static int	handle_child_status(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

// Execute une commande simple (sans pipe ni redirection)
int	execute_cmd_node(t_node *node, char **envp)
{
	int		status;
	pid_t	pid;

	status = 0;
	if (!node || !node->cmd || !node->cmd[0])
		return (0);
	if (ft_is_builtin(node->cmd, &envp))
		return (0);
	pid = fork();
	if (pid == 0)
		exec_child_process(node, envp);
	else if (pid > 0)
	{
		waitpid(pid, &status, 0);
		status = handle_child_status(status);
	}
	else
	{
		perror("minishell: fork");
		status = 1;
	}
	return (status);
}
