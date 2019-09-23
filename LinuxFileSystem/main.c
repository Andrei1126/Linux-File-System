
/*Micut Andrei-Ion-Grupa 311CB*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "filemgr.h"

#define INPUT_LINE_SIZE 4096

/**
 * Change the current directory.
 */
trie_cell_t *do_cd(trie_cell_t *p_cell, char *path)
{
	char buffer[MAX_PATH_LENGTH];
	strcpy(buffer, path);
	path = buffer;

	// Check if it's an absolute path
	if (path[0] == '/') {
		p_cell = trie_cell_get_root(p_cell);
		++path;
	}
	char *token = strtok(path, "/");

	// Advance in the hierarchy, directory by directory
	while (token != NULL) {

		// The parent of the current directory
		if (strcmp(token, "..") == 0) {
			if (p_cell->c == '/') p_cell = p_cell->parent;
			p_cell = trie_cell_go_back(p_cell, '/');
			if (p_cell == NULL)
				return NULL;

			// Current directory
		} else if (strcmp(token, ".") == 0) {
			token = strtok(NULL, "/");
			continue;
		} else {
			p_cell = trie_cell_go_forward(p_cell, token);
			if (p_cell == NULL)
				return NULL;
		}
		if (p_cell != NULL && p_cell->c != '/'
			&& p_cell->children[(int)'/'] != NULL)

			// Trate '/' as an int, not a char
			p_cell = p_cell->children[(int)'/'];
		token = strtok(NULL, "/");
	}

	return p_cell;
}

/**
 * List all the files and directories that are in the current directory.
 */
void do_ls(trie_cell_t *p_cell, int is_special_print)
{
	char buffer[MAX_PATH_LENGTH];
	buffer[0] = '\0';
	trie_cell_recursive_list(buffer, 0, p_cell, is_special_print);
	printf("\n");
}

trie_cell_t *go_to_last_directory(trie_cell_t *p_cell, char *name, char **file)
{
	int is_absolute = 0;
	char tmp[INPUT_LINE_SIZE];
	if (name[0] == '/') {
		++name;
		is_absolute = 1;
		p_cell = trie_cell_get_root(p_cell);
	}

	// Count the number of slashes
	int no_slashes = 0;
	for (int i = 0; name[i] != '\0'; ++i) {
		if (name[i] == '/')
			++no_slashes;
	}
	char *p = name;
	while (no_slashes > 0) {
		if (*p == '/') --no_slashes;
		++p;
	}

	if (p != name) {
		*(p - 1) = '\0';
		strcpy(tmp, name);
		p_cell = do_cd(p_cell, tmp);
		if (p_cell == NULL) {
			*(p - 1) = '/';
			if (is_absolute)
				fprintf(stderr, "/");

			// If the path wasn't find
			fprintf(stderr, "%s: No such file or directory\n", name);
			return NULL;
		} else if (p_cell->c != '/' && p_cell->children[(int)'/'] == NULL) {
			*(p - 1) = '/';
			if (is_absolute)
				fprintf(stderr, "/");

			// If instead of a directory, we have the path of a file
			fprintf(stderr, "%s: Not a directory\n", name);
			return NULL;
		}
	}

	// Go to the last directory
	*file = p;
	*(p - 1) = '/';
	return p_cell;
}

/**
 * Make a new directory (name variable is not changed)
 */
void do_mkdir(trie_cell_t *p_cell, char *name)
{
	char *p;
	p_cell = go_to_last_directory(p_cell, name, &p);
	if (p_cell == NULL)
		return;

	trie_cell_t *tmp;

	// Check if the directory already exists
	if ((tmp = do_cd(p_cell, p)) != NULL && (tmp->is_file ||
		tmp->c == '/' || tmp->children[(int)'/'] != NULL)) {
		fprintf(stderr, "%s: already exists\n", name);
		return;
	}
	p_cell = trie_cell_insert(p_cell, p);
	if (p != name)
		*(p - 1) = '/';
	if (p_cell == NULL)
		return;
	if (p_cell->c != '/') {

		// Allocate the memory for a new trie cell
		p_cell->children[(int)'/'] = make_trie_cell(p_cell, '/', 0);
		p_cell = p_cell->children[(int)'/'];
		if (p_cell == NULL)
		 	return;
	}
}

/**
 * Make a new file. (argument name is not modified)
 */
void do_touch(trie_cell_t *p_cell, char *name)
{
	char *p;
	p_cell = go_to_last_directory(p_cell, name, &p);
	if (p_cell == NULL)
		return;

	trie_cell_t *tmp;

	// Check if the file already exists
	if ((tmp = do_cd(p_cell, p)) != NULL && (tmp->is_file ||
		tmp->c == '/' || tmp->children[(int)'/'] != NULL)) {
		fprintf(stderr, "%s: already exists\n", name);
		return;
	}
	p_cell = trie_cell_insert(p_cell, p);
	if (p != name)
		*(p - 1) = '/';
	if (p_cell == NULL)
		return;
	p_cell->is_file = 1;
}

/**
 * Delete an empty directory.
 */
void do_rmdir(trie_cell_t *p_cell, char *name)
{
	char *p;
	trie_cell_t *tmp;
	p_cell = go_to_last_directory(p_cell, name, &p);
	if (p_cell == NULL)
		return;

	tmp = do_cd(p_cell, p);

	// Check if i could move on this directory
	if (tmp == NULL) {
		fprintf(stderr, "%s: No such file or directory\n", name);
		return;
	}

	// If instead of a directory, we have the path
	if (tmp->c != '/' && tmp->children[(int)'/']) {
		fprintf(stderr, "%s: Not a directory\n", name);
		return;
	}

	// Check if a cell is empty and then erase the cell and its children
	if (trie_cell_is_empty(tmp))
		erase_trie_cell(tmp);
	else

		// If the directory we want to delete, it isn't empty
		fprintf(stderr, "%s: Directory not empty\n", name);
}

/**
 * Delete recursively files and directories.
 */
void do_rm(trie_cell_t *p_cell, char *name)
{
	char *p;
	trie_cell_t *tmp;
	p_cell = go_to_last_directory(p_cell, name, &p);
	if (p_cell == NULL)
		return;

	tmp = do_cd(p_cell, p);

	// Check if i could move on this directory
	if (tmp == NULL) {
		fprintf(stderr, "%s: No such file or directory\n", name);
		return;
	}
	if (tmp->c == '/' || tmp->children[(int)'/'] != NULL) {
		if (tmp->c != '/')
			tmp = tmp->children[(int)'/'];
		erase_trie_cell(tmp);
	} else if (tmp->is_file) {
		tmp->is_file = 0;

		// If the path doesn't exists
	} else {
		fprintf(stderr, "%s: No such file or directory\n", name);
	}
}

int main(void)
{
	int n_lines;
	char line[INPUT_LINE_SIZE];

	trie_cell_t *p_root = make_trie_cell(NULL, '/', 0);
	if (p_root == NULL)
		return -1;

	trie_cell_t *p_current = p_root;

	// Check if we have more than one line or 0
	if (scanf("%d\n", &n_lines) != 1)
		return -1;

	while (n_lines--) {
		memset(line, 0, INPUT_LINE_SIZE);
		fgets(line, INPUT_LINE_SIZE, stdin);
		char *command = strtok(line, "\n ");
		char *args = command + strlen(command) + 1;
		char args_copy[INPUT_LINE_SIZE];
		strcpy(args_copy, args);
		if (args_copy[strlen(args_copy) - 1] == '\n')
			args_copy[strlen(args_copy) - 1] = '\0';

		if (strcmp(command, "pwd") == 0) {

			// Check if a command it's called with too many arguments
			if (args[0] != '\0') {
				fprintf(stderr, "pwd: too many arguments\n");
				continue;
			}

			if (p_current->c == '/' && p_current->parent != NULL)

				// Print the trie
				trie_cell_print(p_current->parent);
			else

				// We are locate in the root and print
				trie_cell_print(p_current);
			printf("\n");

			// Verify if the command inserted is "ls"
		} else if (strcmp(command, "ls") == 0) {
			int is_special_print = 0;
			char *arg = strtok(args, "\n ");
			if (arg == NULL) {
				do_ls(p_current, 0);
				continue;

				// Verify if the argument inserted is "-F"
			} else if (strcmp(arg, "-F") == 0) {
				is_special_print = 1;
				arg = strtok(NULL, "\n ");
				if (arg == NULL) {
					do_ls(p_current, 1);
					continue;
				}
			}
			if (strtok(NULL, "\n ") != NULL) {

				// Check if a command it's called with too many arguments
				fprintf(stderr, "ls %s: too many arguments\n", args_copy);
				continue;
			}
			trie_cell_t *p;
			int len = strlen(arg);

			// Check if the first argument is root
			if (arg[0] == '/')
				p = do_cd(p_root, arg + 1);
			else
				p = do_cd(p_current, arg);
			do_ls(p, is_special_print);

			// Use this form since it might infere with the used functions
			arg = strtok(arg + len + 1, "\n ");

			// Verify if the command inserted is "cd"
		} else if (strcmp(command, "cd") == 0) {
			trie_cell_t *p;
			char *arg0 = strtok(args, "\n ");

			// Check if the command is called with too few operands
			if (args == NULL) {
				fprintf(stderr, "cd: missing operand\n");
				continue;
			} else {
				char *arg1 = strtok(NULL, "\n ");

				// Check if the command is called with too many operands
				if (arg1 != NULL) {
					fprintf(stderr, "cd: too many arguments\n");
					continue;
				}
				p = do_cd(p_current, arg0);
			}

			// Check if the directory exists
			if (p == NULL)
				fprintf(stderr, "%s: No such file or directory\n", args_copy);

			// If instead of a directory, we have the path
			else if (p->c != '/' && p->children[(int)'/'] == NULL)
				fprintf(stderr, "%s: Not a directory\n", args_copy);
			else
				p_current = p;

			//Verify if the command inserted is "mkdir"
		} else if (strcmp(command, "mkdir") == 0) {
			char *arg = strtok(args, "\n ");

			// Check if the command is called with too few operands
			if (arg == NULL) {
				fprintf(stderr, "mkdir: missing operand");
				continue;
			}
			while (arg != NULL) {
				int len = strlen(arg);
				do_mkdir(p_current, arg);

				// Use this form since it might infere with the used functions
				arg = strtok(arg + len + 1, "\n ");
			}

			//Verify if the command inserted is "touch"
		} else if (strcmp(command, "touch") == 0) {
			char *arg = strtok(args, "\n ");

			// Check if the command is called with too few operands
			if (arg == NULL) {
				fprintf(stderr, "touch: missing operand");
				continue;
			}
			while (arg != NULL) {
				int len = strlen(arg);
				do_touch(p_current, arg);

				// Use this form since it might infere with the used functions
				arg = strtok(arg + len + 1, "\n ");
			}

			//Verify if the command inserted is "rmdir"
		} else if (strcmp(command, "rmdir") == 0) {
			char *arg = strtok(args, "\n ");

			// Check if the command is called with too few operands
			if (arg == NULL) {
				fprintf(stderr, "rmdir: missing operand");
				continue;
			}
			while (arg != NULL) {
				int len = strlen(arg);
				do_rmdir(p_current, arg);

				// Use this form since it might infere with the used functions
				arg = strtok(arg + len + 1, "\n ");
			}

			//Verify if the command inserted is "rm"
		} else if (strcmp(command, "rm") == 0) {
			char *arg = strtok(args, "\n ");

			// Check if the command is called with too few operands
			if (arg == NULL) {
				fprintf(stderr, "rm: missing operand");
				continue;
			}
			while (arg != NULL) {
				int len = strlen(arg);
				do_rm(p_current, arg);

				// Use this form since it might infere with the used functions
				arg = strtok(arg + len + 1, "\n ");
			}

			//Verify if the command inserted is "mv"
		} else if (strcmp(command, "mv") == 0) {
			char *to_path = args + strlen(args) - 1;
			char *to_file, *arg_end;
			while (*to_path != ' ' && to_path != args)
				--to_path;

			// Check if the command is called with too few operands
			if (to_path == args) {
				args[strlen(args) - 1] = '\0';
				fprintf(stderr, "mv %s: missing operand\n", args);
				continue;
			}
			trie_cell_t *to, *tmp;
			*to_path = '\0';
			++to_path;
			arg_end = to_path;
			if (strcmp(to_path, "/") != 0 && strcmp(to_path, "/\n") != 0) {
				to_path = strtok(to_path, "/\n ");
				to_file = strtok(NULL, "/\n ");
				if (to_file == NULL) {
					to_file = to_path;
					to_path = ".";
				}
				to = do_cd(p_current, to_path);

				// Check if the path exists
				if (to == NULL) {
					fprintf(stderr, "%s/%s: No such file or directory\n",
						to_path, to_file);
				}
				tmp = do_cd(to, to_file);
				if (tmp != NULL && (tmp->c == '/' ||
					tmp->children[(int)'/'] != NULL)) {
					to = tmp;
					if (to->c != '/')
						to = to->children[(int)'/'];
					if (to_path == NULL)
						to_path = to_file;
					to_file = NULL;
				}
			} else {
				to_file = NULL;
				to = trie_cell_get_root(p_current);
			}
			char *arg = strtok(args, "\n ");
			while (arg != NULL && arg != arg_end) {
				int len = strlen(arg);
				char *p;
				trie_cell_t *p_cell = go_to_last_directory(p_current, arg, &p);
				trie_cell_t *from;
				if (p_cell == NULL) {
					arg = strtok(arg + len + 1, "\n ");
					continue;
				}
				from = do_cd(p_cell, p);

				// Check if the path exists
				if (from == NULL || !from->is_file) {
					fprintf(stderr, "%s: No such file or directory\n", arg);
					arg = strtok(arg + len + 1, "\n ");
					continue;
				}
				if (to_file == NULL) {
					trie_cell_insert(to, p)->is_file = 1;
					from->is_file = 0;

					// If instead of a directory, we have the path
				} else if (tmp != NULL && tmp->is_file) {
					fprintf(stderr, "%s: Not a directory\n", to_file);

					// Check if the path exists
				} else {
					fprintf(stderr, "%s: No such file or directory\n", to_file);
					// trie_cell_insert(to, to_file)->is_file = 1;
				}
				// Use this form since it might infere with the used functions
				arg = strtok(arg + len + 1, "\n ");
			}

			//Verify if the command inserted is "cp"
		} else if (strcmp(command, "cp") == 0) {
			char *to_path = args + strlen(args) - 1;
			char *to_file, *arg_end;
			while (*to_path != ' ' && to_path != args)
				--to_path;

			// Check if the command is called with too few operands
			if (to_path == args) {
				args[strlen(args) - 1] = '\0';
				fprintf(stderr, "cp %s: missing operand\n", args);
				continue;
			}
			trie_cell_t *to, *tmp;
			*to_path = '\0';
			++to_path;
			arg_end = to_path;
			if (strcmp(to_path, "/") != 0 && strcmp(to_path, "/\n") != 0) {
				to_path = strtok(to_path, "/\n ");
				to_file = strtok(NULL, "/\n ");
				if (to_file == NULL) {
					to_file = to_path;
					to_path = ".";
				}
				to = do_cd(p_current, to_path);

				// Check if the path exists
				if (to == NULL) {
					fprintf(stderr, "%s/%s: No such file or directory\n",
						to_path, to_file);
				}
				tmp = do_cd(to, to_file);
				if (tmp != NULL && (tmp->c == '/' ||
					tmp->children[(int)'/'] != NULL)) {
					to = tmp;
					if (to->c != '/')
						to = to->children[(int)'/'];
					if (to_path == NULL)
						to_path = to_file;
					to_file = NULL;
				}
			} else {
				to_file = NULL;
				to = trie_cell_get_root(p_current);
			}
			char *arg = strtok(args, "\n ");
			while (arg != NULL && arg != arg_end) {
				int len = strlen(arg);
				char *p;
				trie_cell_t *p_cell = go_to_last_directory(p_current, arg, &p);
				trie_cell_t *from;
				if (p_cell == NULL) {
					arg = strtok(arg + len + 1, "\n ");
					continue;
				}
				from = do_cd(p_cell, p);

				// Check if the path exists
				if (from == NULL || !from->is_file) {
					fprintf(stderr, "%s: No such file or directory\n", arg);
					arg = strtok(arg + len + 1, "\n ");
					continue;
				}
				if (to_file == NULL) {
					trie_cell_insert(to, p)->is_file = 1;

					// If instead of a directory, we have the path
				} else if (tmp != NULL && tmp->is_file) {
					fprintf(stderr, "%s: Not a directory\n", to_file);

					// Check if the path exists
				} else {
					fprintf(stderr, "%s: No such file or directory\n", to_file);
					// trie_cell_insert(to, to_file)->is_file = 1;
				}

				// Use this form since it might infere with the used functions
				arg = strtok(arg + len + 1, "\n ");
			}

			// If an invalide command is called
		} else {
			fprintf(stderr, "%s: command not found\n", command);
		}
	}

// Erase a cell and all its children
	erase_trie_cell(p_root);
	return 0;
}
