/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsadikaj <lsadikaj@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 15:13:42 by lsadikaj          #+#    #+#             */
/*   Updated: 2025/05/27 13:10:23 by lsadikaj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// Initialise la structure principale du shell
void	*ft_realloc(void *ptr, size_t old_size, size_t new_size)
{
	void	*new_ptr;
	size_t	copy_size;
	size_t	i;

	if (new_size == 0)
	{
		free(ptr);
		return (NULL);
	}
	new_ptr = malloc(new_size);
	if (!new_ptr)
		return (NULL);
	if (ptr)
	{
		if (old_size < new_size)
			copy_size = old_size;
		else
			copy_size = new_size;
		i = 0;
		while (i < copy_size)
		{
			((char *)new_ptr)[i] = ((char *)ptr)[i];
			i++;
		}
		free(ptr);
	}
	return (new_ptr);
}

t_shell	*init_shell(char **envp)
{
	t_shell	*shell;

	shell = ft_calloc(1, sizeof(t_shell));
	if (!shell)
		return (NULL);
	shell->envp = ft_wrap_array(envp);
	if (!shell->envp)
	{
		free(shell);
		return (NULL);
	}
	shell->tokens = NULL;
	shell->ast = NULL;
	shell->exit_status = 0;
	shell->current_dir = getcwd(NULL, 0);
	shell->heredocs = NULL;
	shell->redirections = NULL;
	if (!shell->current_dir)
	{
		free(shell->envp);
		free(shell);
		return (NULL);
	}
	return (shell);
}

// Libère proprement la structure principale du shell
void	free_shell(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->envp && *shell->envp)
	{
		free_array(*shell->envp);
		free(shell->envp);
		shell->envp = NULL;
	}
	if (shell->tokens)
	{
		free_tokens(shell->tokens);
		shell->tokens = NULL;
	}
	if (shell->ast)
	{
		free_ast(shell->ast);
		shell->ast = NULL;
	}
	if (shell->current_dir)
	{
		free(shell->current_dir);
		shell->current_dir = NULL;
	}
	if (shell->heredocs)
	{
		free_heredocs(shell->heredocs);
		shell->heredocs = NULL;
	}
	if (shell->redirections)
	{
		free_redirections(shell->redirections);
		shell->redirections = NULL;
	}
	free(shell);
}


// Fonction pour nettoyer après chaque commande
void	cleanup_shell_iteration(t_shell *shell)
{
	if (!shell)
		return ;
	// Nettoyer les tokens de l'itération précédente
	if (shell->tokens)
	{
		free_tokens(shell->tokens);
		shell->tokens = NULL;
	}
	
	// Nettoyer l'AST de l'itération précédente
	if (shell->ast)
	{
		free_ast(shell->ast);
		shell->ast = NULL;
	}
	
	// Nettoyer les redirections de l'itération précédente
	if (shell->redirections)
	{
		free_redirections(shell->redirections);
		shell->redirections = NULL;
	}
	
	// Nettoyer les heredocs de l'itération précédente
	if (shell->heredocs)
	{
		free_heredocs(shell->heredocs);
		shell->heredocs = NULL;
	}
	shell->tokens = NULL;
	debug_show_token_stats();
}
