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
		if (argc != 2) {
			printHelp();
			return 1;
		}
		initTasks();
		return 0;
	}

	// Open db
	sqlite3 *db = NULL;
	char root[PATH_MAX];
	char db_path[PATH_MAX];

	if (!findProjectDir(root, sizeof(root))) {
		fprintf(stderr, "Not a tsk project!\n");
		return 1;
	}

	snprintf(db_path, sizeof(db_path), "%s/.tsk/tasks.db", root);

	if (!createOpenDb(&db, db_path))
		return 1;

	if (!createTasksTable(db))
		return 1;

	//
	// Add Task
	//
	if (strcmp(argv[1], "add") == 0) {
		if (argc != 3) {
			printHelp();
			return 1;
		}
		addTask(db, argv[2]);
		return 0;
	}
	//
	// Add description
	//
	else if (strcmp(argv[1], "desc") == 0) {
		if (argc != 4) {
			printHelp();
			return 1;
		}
		float id;
		if (parseToNum(argv[2], &id)) {
			addTaskDesc(db, argv[3], id);
			return 0;
		}
		printHelp();
	}
	//
	// Show tasks
	//
	else if (strcmp(argv[1], "show") == 0) {
		int state = 0; // Completed or not
		int mode = 1; // Detailed or not

		if (argc > 2) {
			if (strcmp(argv[2], "-c") == 0)
				state = 1;
			else if (strcmp(argv[2], "-d") == 0)
				mode = 0;
			else if (strcmp(argv[2], "-cd") == 0) {
				state = 1;
				mode = 0;
			}
			else {
				printHelp();
				return 1;
			}
		}

		if (getTaskCount(db, state) <= 0) {
			if (state == 0)
				printf("No pending tasks!\n");
			else
				printf("No completed tasks!\n");
		}
		else {
			if (state == 0) {
				printf("Tasks\n");
				displayTasks(db, state, mode);
			}
			else {
				printf("Completed\n");
				displayTasks(db, state, mode);
				state = 0;
				printf("Tasks\n");
				displayTasks(db, state, mode);
			}
		}

		return 0;
	}
	//
	// Toggle task
	//
	else if (strcmp(argv[1], "tick") == 0) {
		if (argc != 3) {
			printHelp();
			return 1;
		}

		float id;
		if (parseToNum(argv[2], &id)) {
			toggleTask(db, id);
			return 0;
		}
		printHelp();
	}
	// 
	// Priority Setup
	//
	else if (strcmp(argv[1], "pry") == 0) {
		if (argc != 4) {
			printHelp();
			return 1;
		}

		float id;
		float priority;
		if (parseToNum(argv[2], &id) && parseToNum(argv[3], &priority)) {
			setPriority(db, id, priority);
			return 0;
		}
		printHelp();
	}
	//
	// Delete task
	//
	else if (strcmp(argv[1], "del") == 0) {
		if (argc != 3) {
			printHelp();
			return 1;
		}

		float id;
		if (parseToNum(argv[2], &id)) {
			deleteTask(db, id);
			return 0;
		}
		printHelp();
	}
	//
	// Edit task
	//
	else if (strcmp(argv[1], "edit") == 0) {
		if (argc != 4) {
			printHelp();
			return 1;
		}

		float id;
		if (parseToNum(argv[2], &id)) {
			editTask(db, id, argv[3]);
			return 0;
		}
		printHelp();
	}
	//
	// Clear all task data
	//
	else if (strcmp(argv[1], "kaboom") == 0) {
		if (argc != 2) {
			printHelp();
			return 1;
		}
		if (confirm("Clear all Data: ")) {
			clearData(db);
			return 0;
		}

	}
	//
	// Print help
	//
	else if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "h") == 0) {
		if (argc != 2) {
			printHelp();
			return 1;
		}
		printHelp();
		return 0;
	}

	else
	printHelp();

	sqlite3_close(db);

	return 0;
}
