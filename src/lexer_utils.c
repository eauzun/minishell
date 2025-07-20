/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emuzun <emuzun@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 18:38:14 by hialpagu          #+#    #+#             */
/*   Updated: 2025/07/20 12:11:38 by emuzun           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

int	is_special_char(char c)
{
	return (c == '|' || c == '<' || c == '>' || c == '\'' || c == '"');
}

t_token	*token_word(const char *line, size_t *i)
{
	size_t	start;
	t_token	*tok;

	start = *i;
	while (line[*i] && !is_whitespace(line[*i]) && !is_special_char(line[*i]))
		(*i)++;
	if (*i == start)
		return (NULL);
	tok = new_token(T_WORD, &line[start], *i - start);
	if (tok && tok->str && ft_strchr(tok->str, '$'))
		tok->expand = 1;
	return (tok);
}

t_token	*new_token(t_token_type type, const char *start, size_t len)
{
	t_token	*tok;

	if (!start || len == 0)
		return (NULL);
	tok = malloc(sizeof(t_token));
	if (!tok)
		return (NULL);
	tok->str = ft_substr(start, 0, len);
	if (!tok->str)
	{
		free(tok);
		return (NULL);
	}
	tok->type = type;
	tok->expand = 0;
	tok->next = NULL;
	return (tok);
}

void	add_token(t_token **list, t_token *tok)
{
	t_token	*current;

	if (!list || !tok)
		return ;
	if (!*list)
	{
		*list = tok;
		return ;
	}
	current = *list;
	while (current->next)
		current = current->next;
	current->next = tok;
}

void	free_token(t_token *list)
{
	t_token	*tmp;

	while (list)
	{
		tmp = list->next;
		if (list->str)
			free(list->str);
		free(list);
		list = tmp;
	}
}

int	is_valid_token_sequence(t_token *tokens)
{
	t_token	*current;

	if (!tokens)
		return (1);
	current = tokens;
	if (current->type == T_PIPE)
	{
		write(2, "minishell: syntax error near unexpected token '|'\n", 51);
		return (0);
	}
	while (current)
	{
		if (current->type == T_PIPE || current->type == T_REDIR_IN
			|| current->type == T_REDIR_OUT || current->type == T_REDIR_APPEND
			|| current->type == T_HEREDOC)
		{
			if (!current->next)
			{
				write(2, "minishell: syntax error near unexpected token 'newline'\n", 57);
				return (0);
			}
			if (current->next->type != T_WORD)
			{
				write(2, "minishell: syntax error near unexpected token\n", 47);
				return (0);
			}
		}
		current = current->next;
	}
	return (1);
}
