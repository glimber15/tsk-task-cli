#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "tasks.h"

#define PATH_MAX 4096

int main(int argc, char **argv) {

	if (argc < 2) {
		printHelp();
		return 1;
	}

	//
	// Initialize/create tasks .tsk dir
	//
	if (strcmp(argv[1], "init") == 0) {
		if (argc < 2 || argc > 2) {
			printHelp();
			return 1;
		}
		initTasks();
		return 0;
	}

	// Open db
	sqlite3 *task_db = NULL;
	char root[PATH_MAX];
	char db_path[PATH_MAX];

	if (!findProjectDir(root, sizeof(root))) {
		fprintf(stderr, "Not a tsk project!\n");
		return 1;
	}

	snprintf(db_path, sizeof(db_path), "%s/.tsk/tasks.db", root);

	if (!createOpenDb(&task_db, db_path))
		return 1;

	if (!createTasksTable(task_db))
		return 1;

	//
	// Add Task
	//
	if (strcmp(argv[1], "add") == 0) {
		if (argc < 3 || argc > 3) {
			printHelp();
			return 1;
		}
		addTask(task_db, argv[2]);
		return 0;
	}
	//
	// Add description
	//
	else if (strcmp(argv[1], "desc") == 0) {
		if (argc < 4 || argc > 4) {
			printHelp();
			return 1;
		}
		float id;
		if (parseToNum(argv[2], &id)) {
			addTaskDesc(task_db, argv[3], id);
			return 0;
		}
		printHelp();
	}
	//
	// Show tasks
	//
	else if (strcmp(argv[1], "show") == 0) {
		displayTasks(task_db, 0, "Tasks");
		if (argc > 2 || argc < 2) {
			if (strcmp(argv[2], "-c") == 0)
				displayTasks(task_db, 1, "Completed");
			else if (strcmp(argv[2], "-d") == 0)
				// TODO: dont show descriptions
				return 1;
			else
				printHelp();
		}
		return 0;
	}
	//
	// Toggle task
	//
	else if (strcmp(argv[1], "tick") == 0) {
		if (argc < 3 || argc > 3) {
			printHelp();
			return 1;
		}

		float id;
		if (parseToNum(argv[2], &id)) {
			toggleTask(task_db, id);
			return 0;
		}
		printHelp();
	}
	//
	// Delete task
	//
	else if (strcmp(argv[1], "del") == 0) {
		if (argc < 3 || argc > 3) {
			printHelp();
			return 1;
		}

		float id;
		if (parseToNum(argv[2], &id)) {
			deleteTask(task_db, id);
			return 0;
		}
		printHelp();
	}
	//
	// Edit task
	//
	else if (strcmp(argv[1], "edit") == 0) {
		if (argc < 4 || argc > 4) {
			printHelp();
			return 1;
		}

		float id;
		if (parseToNum(argv[2], &id)) {
			editTask(task_db, id, argv[3]);
			return 0;
		}
		printHelp();
	}
	//
	// Clear all task data
	//
	else if (strcmp(argv[1], "kaboom") == 0) {
		if (confirm("Clear all Data: ")) {
			clearData(task_db);
			return 0;
		}

	}
	//
	// Print help
	//
	else if (strcmp(argv[1], "help") == 0) {
		printHelp();
		return 0;
	}

	else
	printHelp();

	sqlite3_close(task_db);

	return 0;
}
