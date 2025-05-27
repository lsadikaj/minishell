/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipe.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimpa <jimpa@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 16:35:18 by jimpa             #+#    #+#             */
/*   Updated: 2025/05/27 12:42:28 by lsadikaj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// Crée une copie filtrée des redirections (seulement les redirections d'entrée)
static t_redir	*filter_input_redirections(t_redir *all_redirections)
{
	t_redir	*filtered;
	t_redir	*current;

	filtered = NULL;
	current = all_redirections;
	while (current)
	{
		if (current->type == TOKEN_REDIRECT_IN\
		|| current->type == TOKEN_HEREDOC)
			add_redirection(&filtered, current->type, current->filename);
		current = current->next;
	}
	return (filtered);
}

// Crée une copie filtrée des redirections (seulement les redirections de sortie)
static t_redir	*filter_output_redirections(t_redir *all_redirections)
{
	t_redir	*filtered;
	t_redir	*current;

	filtered = NULL;
	current = all_redirections;
	while (current)
	{
		if (current->type == TOKEN_REDIRECT_OUT\
|| current->type == TOKEN_APPEND)
			add_redirection(&filtered, current->type, current->filename);
		current = current->next;
	}
	return (filtered);
}

// Exécute le côté droit du pipe avec les redirections filtrées
int	execute_pipe_node_right(t_node *node,\
	char ***envp, int pipefd[2], pid_t pid_left, t_shell *shell)
{
	int		status;
	pid_t	pid_right;
	t_redir	*original_redirections;
	t_redir	*filtered_redirections;

	pid_right = fork();
	if (pid_right == 0)
	{
		// Filtrer les redirections pour ne garder que les sorties
		original_redirections = shell->redirections;
		filtered_redirections = filter_output_redirections\
(original_redirections);
		shell->redirections = filtered_redirections;
		close(pipefd[1]);
		if (dup2(pipefd[0], STDIN_FILENO) == -1)
		{
			perror("minishell: dup2");
			free_redirections(shell->redirections);
			free_heredocs(shell->heredocs);
			exit(1);
		}
		close(pipefd[0]);
		status = execute_node_by_type(node->right, envp, shell);
		free_redirections(shell->redirections);
		free_heredocs(shell->heredocs);
		exit(status);
	}
	close(pipefd[0]);
	close(pipefd[1]);
	waitpid(pid_left, NULL, 0);  // Ignorer le statut du process gauche
	waitpid(pid_right, &status, 0);  // Utiliser le statut du dernier process
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

// Ne libere que si c est une copie filtree, pas l original
static void	cleanup_filtered_redirections(t_redir *filtered, t_redir *original)
{
	if (filtered && filtered != original)
		free_redirections(filtered);
}

// Exécute un pipe en filtrant les redirections pour chaque côté
int	execute_pipe_node(t_node *node, char ***envp, t_shell *shell)
{
	int		status;
	int		pipefd[2];
	pid_t	pid_left;
	t_redir	*original_redirections;
	t_redir	*filtered_redirections;

	if (pipe(pipefd) == -1)
		return (perror("minishell: pipe"), -1);

	// Sauvegarder les redirections originales
	original_redirections = shell->redirections;
	filtered_redirections = NULL;
	// Processus gauche (écriture)
	pid_left = fork();
	if (pid_left == 0)
	{
		// Filtrer les redirections pour ne garder que les entrées
		filtered_redirections = filter_input_redirections\
(original_redirections);
		shell->redirections = filtered_redirections;
		close(pipefd[0]);
		if (dup2(pipefd[1], STDOUT_FILENO) == -1)
		{
			perror("minishell: dup2");
			cleanup_filtered_redirections(filtered_redirections, original_redirections);
			free_heredocs(shell->heredocs);
			exit(EXIT_FAILURE);
		}
		close(pipefd[1]);
		status = execute_node_by_type(node->left, envp, shell);
		cleanup_filtered_redirections(filtered_redirections, original_redirections);
		free_heredocs(shell->heredocs);
		exit(status);
	}
	// Restaurer les redirections originales pour le processus parent
	shell->redirections = original_redirections;
	status = execute_pipe_node_right(node, envp, pipefd, pid_left, shell);
	cleanup_filtered_redirections(filtered_redirections, original_redirections);
	
	return (status);
}
