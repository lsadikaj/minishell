/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_read_line_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimpa <jimpa@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 14:15:23 by lsadikaj          #+#    #+#             */
/*   Updated: 2025/05/26 18:56:14 by lsadikaj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

// Initialise les variables de la boucle
/* void print_ast(t_node *node, int level)
{
    if (!node) return;
    for (int i = 0; i < level; i++) printf("  ");
    if (node->type == NODE_CMD) {
        printf("CMD: ");
        for (int i = 0; node->cmd[i]; i++) printf("%s ", node->cmd[i]);
        printf("\n");
    } else {
        printf("NODE: %s\n", (node->type == NODE_AND) ? "AND" : "OR");
        print_ast(node->left, level + 1);
        print_ast(node->right, level + 1);
    }
} */
void	init_loop_vars(t_shell *shell)
{
	// S'assurer que tout est NULL avant de commencer
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
	if (shell->redirections)
	{
		free_redirections(shell->redirections);
		shell->redirections = NULL;
	}
	if (shell->heredocs)
	{
		free_heredocs(shell->heredocs);
		shell->heredocs = NULL;
	}
	
	// Réinitialiser à NULL (au cas où)
	shell->tokens = NULL;
	shell->ast = NULL;
	shell->redirections = NULL;
	shell->heredocs = NULL;
	
	if (g_signal == SIGINT)
	{
		g_signal = 0;
		rl_on_new_line();
	}
	ft_getcwd();
}

// Gère l'AST et l'exécution de la commande
void	handle_ast_execution(t_shell *shell)
{
	t_redirect	red;
	
	init_redirect(&red);
	shell->ast = parse_ast(shell->tokens);
	if (!shell->ast)
	{
		restore_std_fds(&red);
		return ;
	}
	shell->exit_status = execute_ast(shell->ast, shell->envp, shell);
	restore_std_fds(&red);
}
