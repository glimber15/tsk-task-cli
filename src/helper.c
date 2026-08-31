#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "tasks.h"

bool findProjectDir(char *root, size_t size) {
	if (getcwd(root, size) == NULL) {
		perror("getcwd");
		return false;
	}

	while (1) {
		char task_dir[size];
		snprintf(task_dir, sizeof(task_dir), "%s/.tsk", root);

		struct stat st;

		if (stat(task_dir, &st) == 0 && S_ISDIR(st.st_mode))
			return true;

		char*slash =strrchr(root, '/');
		if (slash == NULL)
			return false;

		if (slash == root) {
			root[1] = '\0';
			snprintf(task_dir, sizeof(task_dir), "%s/.tsk", root);

			if (stat(task_dir, &st) == 0 && S_ISDIR(st.st_mode))
				return true;

			return false;
		}
		*slash = '\0';
	}
}

void printHelp() {
	printf(
		"usage                      tsk <action> <params...>\n\n"
		"actions:\n"
		"  init                     Initialize tasks in project dir\n"
		"  add 'task title'         Add a task\n"
		"  desc id 'description'    Add description to a task\n"
		"  show                     Show tasks pending\n"
		"  show -c                  Show completed tasks also\n"
		"  tick id                  Complete/toggle a task\n"
		"  edit id 'new title'      Edit a task title\n"
		"  del id                   Delete a task\n"
		"  help                     Show this again\n"
		"  kaboom                   Uninitaialize tasks\n"
	);
}

bool parseToNum(const char *str, float *result) {
	char *end;
	*result = strtod(str, &end);
	return *end == '\0';
}

bool confirm(const char *dialog) {
	char input;
	printf("%s", dialog);
	printf("(y/N)?");
	scanf("%c", &input);
	getchar();
	if (input == 'Y' || input == 'y')
		return true;
	else if (input == 'N' || input == 'n')
		return false;
	else {
		printHelp();
		return false;
	}
}
