/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_special.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emuzun <emuzun@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 18:39:48 by hialpagu          #+#    #+#             */
/*   Updated: 2025/07/20 12:02:39 by emuzun           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*single_quote(const char *line, size_t *i)
{
	t_token	*tok;
	size_t	start;

	if (line[*i] != '\'')
		return (NULL);
	start = ++(*i);
	while (line[*i] && line[*i] != '\'')
		(*i)++;
	if (line[*i] != '\'')
	{
		write(2, "minishell: syntax error: unclosed single quote\n", 48);
		return (NULL);
	}
	tok = new_token(T_WORD, &line[start], *i - start);
	if (tok)
		tok->expand = 0; // single quote'da expansion yok
	(*i)++;
	return (tok);
}

t_token	*double_quote(const char *line, size_t *i)
{
	t_token	*tok;
	size_t	start;

	if (line[*i] != '"')
		return (NULL);
	start = ++(*i);
	while (line[*i] && line[*i] != '"')
		(*i)++;
	if (line[*i] != '"')
	{
		write(2, "minishell: syntax error: unclosed double quote\n", 48);
		return (NULL);
	}
	tok = new_token(T_WORD, &line[start], *i - start);
	if (tok && ft_strchr(tok->str, '$'))
		tok->expand = 1; // double quote'da $ expansion var
	(*i)++;
	return (tok);
}

t_token	*redirections(const char *line, size_t *i)
{
	t_token_type	type;
	size_t			start;
	size_t			len;

	if (!line || !i)
		return (NULL);
	start = *i;
	if (line[*i] == '<')
	{
		if (line[*i + 1] == '<')
		{
			type = T_HEREDOC;
			len = 2;
		}
		else
		{
			type = T_REDIR_IN;
			len = 1;
		}
	}
	else if (line[*i] == '>')
	{
		if (line[*i + 1] == '>')
		{
			type = T_REDIR_APPEND;
			len = 2;
		}
		else
		{
			type = T_REDIR_OUT;
			len = 1;
		}
	}
	else
		return (NULL);
	*i += len;
	return (new_token(type, &line[start], len));
}

t_token	*is_pipe(const char *line, size_t *i)
{
	if (!line || !i || line[*i] != '|')
		return (NULL);
	if (line[*i + 1] == '|')
	{
		write(2, "minishell: syntax error: '||' not supported\n", 45);
		return (NULL);
	}
	(*i)++;
	return (new_token(T_PIPE, "|", 1));
}