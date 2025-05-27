/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsadikaj <lsadikaj@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 23:42:00 by lsadikaj          #+#    #+#             */
/*   Updated: 2025/05/27 13:15:40 by lsadikaj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// Libère un tableau de chaînes terminé par NULL
void	free_array(char **array)
{
	int	i;

	if (!array)
		return ;
	i = 0;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}

// Libère un tableau de chaînes (alias de free_array)
void	ft_free_split(char **split_array)
{
	free_array(split_array);
}

// Nettoyage d urgence pour les tokens partiellement traites
void	cleanup_partial_expansion(t_shell *shell, t_token *failed_token)
{
	if (!shell || !failed_token)
		return ;
	if (failed_token->parts)
	{
		free_word_parts(failed_token->parts);
		failed_token->parts = NULL;
	}
	if (!failed_token->value)
	{
		failed_token->value = ft_strdup("");
		if (!failed_token->value)
			shell->tokens = delete_token(shell->tokens, failed_token);
	}
}

// Libère la mémoire de la liste chaînée de tokens
void	free_tokens(t_token *tokens)
{
	t_token	*current;
	t_token	*next;

	current = tokens;
	while (current)
	{
		next = current->next;

		// Debug spécifique pour les opérateurs
        if (current->type == TOKEN_REDIRECT_OUT)
        {
            printf("DEBUG SPECIAL: Libération opérateur '>' à l'adresse %p\n", (void*)current);
            printf("DEBUG SPECIAL: Libération sa valeur à l'adresse %p\n", (void*)current->value);
        }
		 // Debug temporaire
        debug_token_destruction(current, "free_tokens");
		
		if (current->parts)
			free_word_parts(current->parts);
		if (current->value)
			free(current->value);
		free(current);
		current = next;
	}
}

// Libère la mémoire de l'arbre (AST) de façon récursive
void	free_ast(t_node *node)
{
	int	i;

	if (!node)
		return ;
	free_ast(node->left);
	free_ast(node->right);
	if (node->cmd)
	{
		i = 0;
		while (node->cmd[i])
		{
			free(node->cmd[i]);
			i++;
		}
		free(node->cmd);
	}
	free(node);
}
