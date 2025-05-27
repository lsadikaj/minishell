/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsadikaj <lsadikaj@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 18:11:42 by lsadikaj          #+#    #+#             */
/*   Updated: 2025/05/27 13:12:41 by lsadikaj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// Libère la mémoire des redirections
void	free_redirections(t_redir *redirections)
{
	t_redir	*current;
	t_redir	*next;

	current = redirections;
	while (current)
	{
		next = current->next;
		if (current->filename)
			free(current->filename);
		free(current);
		current = next;
	}
}

// Libère la mémoire des heredocs
void	free_heredocs(t_heredoc *heredocs)
{
	t_heredoc	*current;
	t_heredoc	*next;

	current = heredocs;
	while (current)
	{
		next = current->next;
		if (current->delimiter)
			free(current->delimiter);
		if (current->content)
			free(current->content);
		free(current);
		current = next;
	}
}

char	*ft_strjoin_free(char *s1, const char *s2)
{
	char *result;

	result = ft_strjoin(s1, s2);
	free(s1);
	return (result);
}

void	free_token(t_token *token)
{
	t_word_part	*parts;
	t_word_part	*tmp;

	if (!token)
		return ;
	// Libération de la liste parts
	parts = token->parts;
	while (parts)
	{
		tmp = parts->next;
		free(parts->content);
		free(parts);
		parts = tmp;
	}
	// Libération du reste
	free(token->value);
	free(token);
}

t_token	*delete_token(t_token *head, t_token *target)
{
	t_token	*current;
	t_token	*prev;

	if (!head || !target)
		return (head);
	// Cas spécial : suppression de la tête
	if (head == target)
	{
		current = head->next;
		free_token(head);
		return (current);
	}
	// Parcours de la liste
	prev = head;
	current = head->next;
	while (current)
	{
		if (current == target)
		{
			prev->next = current->next;
			free_token(current);
			break ;
		}
		prev = current;
		current = current->next;
	}
	return (head);
}

// Fonctions de débogage temporaires - à supprimer après résolution du problème
static int debug_token_counter = 0;
static int debug_token_freed = 0;

void debug_token_creation(t_token *token, const char *location)
{
    debug_token_counter++;
    printf("DEBUG: Token #%d créé (%s) - valeur: '%s', type: %d\n", 
           debug_token_counter, location, token->value, token->type);
}

void debug_token_destruction(t_token *token, const char *location)
{
    debug_token_freed++;
    printf("DEBUG: Token détruit (%s) - valeur: '%s', compteur libérés: %d\n", 
           location, token->value ? token->value : "NULL", debug_token_freed);
}

void debug_show_token_stats(void)
{
    printf("DEBUG: Bilan tokens - Créés: %d, Libérés: %d, Différence: %d\n", 
           debug_token_counter, debug_token_freed, debug_token_counter - debug_token_freed);
}
