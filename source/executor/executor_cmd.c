/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_cmd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimpa <jimpa@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 14:22:13 by lsadikaj          #+#    #+#             */
/*   Updated: 2025/05/26 18:49:49 by lsadikaj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// Exécute une commande externe en cherchant son chemin dans $PATH
// rajouter controle / gestion de commande sous forme de path expl : "/bin/ls"
static int	exec_external(char **cmd, char **envp)
{
	char	*path;
	int		special;

	if (!cmd || !cmd[0])
		return (1);
	special = handle_special_commands(cmd);
	if (special)
		return (special);
	path = handle_direct_path(cmd[0]);
	if (!path)
		path = ft_path_finder(cmd[0], &envp);
	if (!path)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd[0], 2);
		ft_putendl_fd(": command not found", 2);
		return (127);
	}
	return (execute_with_path(path, cmd, envp));
}

// Forke et exécute une commande dans un processus enfant
static int	exec_forked(t_node *node, char **envp)
{
	pid_t	pid;
	int		status;
	int		ret;

	pid = fork();
	if (pid == -1)
	{
		perror("minishell: fork");
		return (1);
	}
	else if (pid == 0)
	{
		ret = exec_external(node->cmd, envp);
		exit(ret);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

// Crée des tokens temporaires à partir des arguments de la commande
static t_token	*create_tokens_from_cmd(char **cmd, t_shell *shell)
{
	t_token	*tokens;
	t_token	*current;
	int		i;

	tokens = NULL;
	i = 0;
	while (cmd[i])
	{
		if (tokens == NULL)
		{
			tokens = malloc(sizeof(t_token));
			current = tokens;
		}
		else
		{
			current->next = malloc(sizeof(t_token));
			current = current->next;
		}
		if (!current)
			return (free_tokens(tokens), NULL);
		current->value = ft_strdup(cmd[i]);
		current->type = TOKEN_WORD;
		current->parts = NULL;
		if(shell->tokens->parts && shell->tokens->parts->type == QUOTE_SINGLE)
		current->next = NULL;
		i++;
	}
	return (tokens);
}

// Met à jour les arguments de la commande avec les valeurs expansées
static void	update_cmd_from_tokens(char **cmd, t_token *tokens)
{
	t_token	*current;
	int		i;

	current = tokens;
	i = 0;
	while (current && cmd[i])
	{
		free(cmd[i]);
		cmd[i] = ft_strdup(current->value);
		current = current->next;
		i++;
	}
}

// Exécute un nœud de type commande (builtin ou externe)
int	execute_cmd_node(t_node *node, char ***envp, t_shell *shell)
{
	t_token	*original_tokens;
	t_token	*temp_tokens;
	int		result;

	if (!node || !node->cmd || !node->cmd[0])
		return (0);
	// Sauvegarder les tokens originaux du shell
	original_tokens = shell->tokens;
	// Créer des tokens temporaires à partir des arguments
	temp_tokens = create_tokens_from_cmd(node->cmd, shell);
	if (!temp_tokens)
		return (1);
	// Assigner les tokens temporaires au shell
	shell->tokens = temp_tokens;
	scan_envar(shell);
	// Mettre à jour les arguments de la commande
	update_cmd_from_tokens(node->cmd, shell->tokens);
	// Restaurer les tokens originaux et nettoyer
	free_tokens(shell->tokens);
	shell->tokens = original_tokens;
	// Effectuer l'expansion des variables (Je l'ai deplacé et ça pose peut-etre probleme)
	// Exécuter la commande
	if (ft_is_builtin(node->cmd, envp))
		result = execute_builtin(node->cmd, envp);
	else
		result = exec_forked(node, *envp);
	return (result);
}
