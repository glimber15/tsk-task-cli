#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include "tasks.h"

// Create or open the DB
bool createOpenDb(sqlite3 **db, const char *db_path) {
	int rc = sqlite3_open(db_path, db);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*db));
		sqlite3_close(*db);
		*db = NULL;

		return false;
	}
	return true;
}

bool createTasksTable(sqlite3 *db) {
	const char *sql =
		"CREATE TABLE IF NOT EXISTS tasks("
		"id INTEGER PRIMARY KEY,"
		"title TEXT NOT NULL,"
		"description TEXT,"
		"state INTEGER NOT NULL DEFAULT 0,"
		"date TEXT DEFAULT CURRENT_TIMESTAMP,"
		"priority INTEGER DEFAULT 0"
		");";

	char *err_msg = NULL;

	if (sqlite3_exec(db, sql, NULL, NULL, &err_msg) != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", err_msg);
		return false;
	}

	sqlite3_free(err_msg);
	return true;
}

void initTasks() {
	if (mkdir(".tsk", 0755) == -1 && errno != EEXIST) {
		perror("Failed to create dir '.tsk'!");
	return;
	}

	printf("tsk initialized successfully!\n");
}
