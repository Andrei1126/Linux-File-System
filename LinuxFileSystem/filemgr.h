
/*Micut Andrei-Ion-Grupa 311CB*/

#ifndef FILEMGR_H_
#define FILEMGR_H_

#define MAX_PATH_LENGTH 4096
#define NO_CHARACTERS 128

typedef struct trie_cell_t {
	int is_file;
	char c;
	struct trie_cell_t *parent;
	struct trie_cell_t *children[NO_CHARACTERS];
} trie_cell_t;

/**
 * Allocate the memory for a new trie cell and initialize it.
 */
trie_cell_t *make_trie_cell(trie_cell_t *parent, char c, int is_file);

/**
 * Erase a cell and all its children.
 */
void erase_trie_cell(trie_cell_t *p_cell);

/**
 * Print the path represented by the given trie cell.
 */
void trie_cell_print(trie_cell_t *p_cell);

/**
 * Go back until the given character is met.
 */
trie_cell_t *trie_cell_go_back(trie_cell_t *p_cell, char c);

/**
 * Go forward on the given path.
 */
trie_cell_t *trie_cell_go_forward(trie_cell_t *p_cell, char *path);

/**
 * Go back in the trie hierarchy until the root is found and return it.
 */
trie_cell_t *trie_cell_get_root(trie_cell_t *p_cell);

/**
 * Insert a new word into the trie.
 */
trie_cell_t *trie_cell_insert(trie_cell_t *p_cell, char *word);

/**
 * Print all the files and directories that are descendants of p_cell.
 */
void trie_cell_recursive_list(char *buffer, int buffer_size,
	trie_cell_t *p_cell, int is_special_print);

/**
 * Check if a cell is empty.
 */
int trie_cell_is_empty(trie_cell_t *p_cell);
#endif
