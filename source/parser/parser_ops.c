/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_ops.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimpa <jimpa@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 17:03:25 by lsadikaj          #+#    #+#             */
/*   Updated: 2025/05/27 14:30:23 by lsadikaj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// Coupe la liste de tokens en deux autour du token opérateur
static void	prepare_op_parts(t_token *tokens, t_token *op,
							t_token **right_part)
{
	t_token	*tmp;

	if (!op || !right_part)
	{
		if (right_part)
			*right_part = NULL;
		return ;
	}
	*right_part = op->next;
	tmp = tokens;
	while (tmp && tmp->next != op)
		tmp = tmp->next;
	if (tmp)
		tmp->next = NULL;
}

// Initialise un nœud opérateur avec ses branches gauche et droite
static t_node	*setup_operator_node(t_node *node, t_token *tokens,
									t_token *right_part)
{
	node->left = parse_ast(tokens);
	node->right = parse_ast(right_part);
	return (node);
}

static t_node	*create_redirect_left_right(t_token *tokens, t_token_type type)
{
	t_node	*node;
	t_token	*cmd_tokens;

	node = malloc(sizeof(t_node));
	if (!node)
		return (NULL);
	node->type = token_to_node_type(type);
	node->cmd = NULL;
	node->right = create_redirect_right(tokens->next);
	cmd_tokens = tokens->next->next;
	if (cmd_tokens)
		node->left = parse_ast(cmd_tokens);
	else
		node->left = NULL;
	return (node);
}

// Initialise un nœud de redirection complet
t_node	*init_redirect_node(t_token *tokens, t_token *right_part,
			t_token_type type)
{
	t_node	*node;

	printf("DEBUG REDIRECT: Initialisation nœud de redirection pour type %d\n", type);
	if (tokens->type == type)
		return (create_redirect_left_right(tokens, type));
	node = malloc(sizeof(t_node));
	if (!node)
		return (NULL);
	node->type = token_to_node_type(type);
	node->cmd = NULL;
	node->right = create_redirect_right(right_part);
	if (!tokens)
		node->left = NULL;
	else
		node->left = setup_redirect_left(tokens);
	printf("DEBUG REDIRECT: Nœud de redirection créé avec succès\n");
	return (node);
}

// Crée un nœud opérateur ou redirection à partir d’un token opérateur
t_node	*create_op_node(t_token *tokens, t_token *op)
{
	t_token			*right_part;
	t_node			*node;
	t_node			*result;
	
	prepare_op_parts(tokens, op, &right_part);
	if (is_redirection(op->type) && right_part
		&& right_part->type == TOKEN_WORD)
	{
		result = init_redirect_node(tokens, right_part, op->type);
		free_token(op);
		return (result);
	}
	node = malloc(sizeof(t_node));
	if (!node)
	{
		free_token(op);
		return (NULL);
	}
	node->type = token_to_node_type(op->type);
	node->cmd = NULL;
	result = setup_operator_node(node, tokens, right_part);
	return (result);
}
