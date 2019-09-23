
/*Micut Andrei-Ion-Grupa 311CB*/

#include <stdlib.h>
#include <stdio.h>

#include "filemgr.h"

trie_cell_t *make_trie_cell(trie_cell_t *parent, char c, int is_file)
{
	trie_cell_t *p = malloc(sizeof(trie_cell_t));
	if (p == NULL)
		return NULL;
	
	// Add all the characters in the tree
	p->parent = parent;
	for (int i = 0; i < NO_CHARACTERS; ++i)
		p->children[i] = NULL;
	p->is_file = is_file;
	p->c = c;

	return p;
}

void erase_trie_cell(trie_cell_t *p_cell)
{
	for (int i = 0; i < NO_CHARACTERS; ++i) {
		if (p_cell->children[i] != NULL)
			erase_trie_cell(p_cell->children[i]);
	}
	free(p_cell);
}

void trie_cell_print(trie_cell_t *p_cell)
{
	if (p_cell == NULL)
		return;
	trie_cell_print(p_cell->parent);
	printf("%c", p_cell->c);
}

trie_cell_t *trie_cell_go_back(trie_cell_t *p_cell, char c)
{
	if (p_cell == NULL)
		return NULL;
	if (p_cell->c == c)
		return p_cell;
	return trie_cell_go_back(p_cell->parent, c);
}

trie_cell_t *trie_cell_go_forward(trie_cell_t *p_cell, char *path)
{
	if (path[0] == '\0')
		return p_cell;
	if (p_cell->children[(int)path[0]] == NULL)
		return NULL;
	return trie_cell_go_forward(p_cell->children[(int)path[0]], path + 1);
}


trie_cell_t *trie_cell_get_root(trie_cell_t *p_cell)
{
	if (p_cell == NULL)
		return NULL;
	while (p_cell->parent != NULL)
		p_cell = p_cell->parent;
	return p_cell;
}

trie_cell_t *trie_cell_insert(trie_cell_t *p_cell, char *word)
{
	for (int i = 0; word[i] != '\0'; ++i) {
		if (p_cell->children[(int)word[i]] == NULL) {
			p_cell->children[(int)word[i]] = make_trie_cell(p_cell, word[i], 0);
			if (p_cell->children[(int)word[i]] == NULL)
				return NULL;
		}
		p_cell = p_cell->children[(int)word[i]];
	}

	return p_cell;
}

void trie_cell_recursive_list(char *buffer, int buffer_size,
	trie_cell_t *p_cell, int is_special_print)
{
	if (buffer_size > 0 && buffer[buffer_size - 1] == '/') {
		
		// Print directory
		if (is_special_print) {
			printf("%s ", buffer);
		} else {
			buffer[buffer_size - 1] = '\0';
			printf("%s ", buffer);
			buffer[buffer_size - 1] = '/';
		}
		return;
	} else if (p_cell->is_file) {
		
		// Print file
		if (is_special_print)
			printf("%s* ", buffer);
		else
			printf("%s ", buffer);
	}

	// Traversate all the children
	for (int i = 0; i < NO_CHARACTERS; ++i) {
		if (p_cell->children[i] == NULL)
			continue;
		buffer[buffer_size] = (char)i;
		buffer[buffer_size + 1] = '\0';
		trie_cell_recursive_list(buffer, buffer_size + 1,
			p_cell->children[i], is_special_print);
	}
}

int trie_cell_is_empty(trie_cell_t *p_cell)
{
	for (int i = 0; i < NO_CHARACTERS; ++i) {
		if (p_cell->children[i] != NULL)
			return 0;
	}
	return 1;
}
