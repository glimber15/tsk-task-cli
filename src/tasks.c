#include "tasks.h"
#include <asm-generic/errno-base.h>
#include <sqlite3.h>
#include <stdio.h>
#include <sys/stat.h>


void addTask(sqlite3 *db, const char *title) {
	const char *sql = "INSERT INTO tasks (title) VALUES (?);";
	sqlite3_stmt *stmt;

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return;
	}

	if (sqlite3_bind_text(stmt, 1, title, -1, SQLITE_TRANSIENT) != SQLITE_OK) {
		fprintf(stderr, "Failed to bind title\n");
		return;
	}

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		fprintf(stderr, "Failed to add task: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return;
	}

	sqlite3_finalize(stmt);

	printf("Added task: [%lld] '%s'\n", sqlite3_last_insert_rowid(db), title);
}

void addTaskDesc(sqlite3 *db, const char *desc, int id) {
	const char *sql =
		"UPDATE tasks SET "
		"description = ? "
		"WHERE id = ?;";
	sqlite3_stmt *stmt;

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return;
	}

	if (sqlite3_bind_text(stmt, 1, desc, -1, SQLITE_TRANSIENT) != SQLITE_OK) {
		fprintf(stderr, "Falied to bind description!\n");
		sqlite3_finalize(stmt);
		return;
	}

	if (sqlite3_bind_int(stmt, 2, id) != SQLITE_OK) {
		fprintf(stderr, "Failed to bind task id\n");
		sqlite3_finalize(stmt);
		return;
	}

	int rc = sqlite3_step(stmt);

	if (rc != SQLITE_DONE) {
		fprintf(stderr, "Failed to update task description: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
	}

	printf("description added to task: [%d] '%s'", id, desc);

	sqlite3_finalize(stmt);
}

void displayTasks(sqlite3 *db, int state, int minimal) {
	const char *sql =
		"SELECT * FROM tasks "
		"WHERE state = ? "
		"ORDER BY id;";
	sqlite3_stmt *stmt;

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return;
	}

	if (sqlite3_bind_int(stmt, 1, state) != SQLITE_OK) {
		fprintf(stderr, "Failed to bind state\n");
		sqlite3_finalize(stmt);
		return;
	}

	int rc = sqlite3_step(stmt);

	while (rc == SQLITE_ROW) {
		int id = sqlite3_column_int(stmt, 0);
		const char *title = (char *)sqlite3_column_text(stmt, 1);
		const char *desc = (char *)sqlite3_column_text(stmt, 2);
		int state = sqlite3_column_int(stmt, 3);
		const char *date = (char *)sqlite3_column_text(stmt, 4);
		int priority = sqlite3_column_int(stmt, 5);

		char symbol = state ? '#' : ' ';

		printf("%4d [%c] %s\n\t\033[90m(pry: %d)\033[0m\n", 
			id, symbol, title, priority);

		if (!minimal) {
			if (desc)
				printf(" - %s\n", desc);
			if (date)
				printf("%s\n\n", date);
		}

		rc = sqlite3_step(stmt);
	}

	sqlite3_finalize(stmt);
}

void toggleTask(sqlite3 *db, int id) {
	const char *sql =
		"UPDATE tasks "
		"SET state = NOT state "
		"WHERE id = ? "
		"RETURNING title, state;";
	sqlite3_stmt *stmt;

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return;
	}

	if (sqlite3_bind_int(stmt, 1, id) != SQLITE_OK) {
		fprintf(stderr, "Failed to bind task id\n");
		return;
	}

	int rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW) {
		const char *title = (const char *)sqlite3_column_text(stmt, 0);
	int state = sqlite3_column_int(stmt, 1);

	if (state)
		printf("Task Complete: [%d] '%s'\n", id, title);
	else
		printf("Task Unchecked: [%d] '%s'\n", id, title);

	sqlite3_finalize(stmt);
	return;
	}

	if (rc == SQLITE_DONE)
		fprintf(stderr, "Task [%d] does not exist\n", id);
	else
		fprintf(stderr, "Failed to update task state: %s\n", sqlite3_errmsg(db));

	sqlite3_finalize(stmt);
}

void setPriority(sqlite3 *db, int id, int priority) {
	const char *sql = 
		"UPDATE tasks SET "
		"priority = ? "
		"WHERE id = ?";
	sqlite3_stmt *stmt;

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return;
	}

	if (sqlite3_bind_int(stmt, 1, priority) != SQLITE_OK) {
		fprintf(stderr, "Failed to bind task id\n");
		sqlite3_finalize(stmt);
		return;
	}

	if (sqlite3_bind_int(stmt, 2, id) != SQLITE_OK) {
		fprintf(stderr, "Failed to bind task id\n");
		sqlite3_finalize(stmt);
		return;
	}

	int rc = sqlite3_step(stmt);

	if (rc != SQLITE_DONE) {
		fprintf(stderr, "Failed to update task description: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
	}

	printf("Priority set: [%d] %d\n", id, priority);

	if (rc == SQLITE_DONE)
		fprintf(stderr, "Task [%d] does not exist\n", id);
	else {
		fprintf(stderr, "Failed to delete task: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(stmt);
}

void deleteTask(sqlite3 *db, int id) {
	const char *sql =
		"DELETE FROM tasks "
		"WHERE id = ? "
		"RETURNING title";
	sqlite3_stmt *stmt;

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return;
	}

	if (sqlite3_bind_int(stmt, 1, id) != SQLITE_OK) {
		fprintf(stderr, "Failed to bind task id\n");
		return;
	}

	int rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW) {
		const char *title = (const char *)sqlite3_column_text(stmt, 0);

		printf("Task deleted: [%d] '%s'\n", id, title);

		sqlite3_finalize(stmt);
		return;
	}

	if (rc == SQLITE_DONE)
		fprintf(stderr, "Task [%d] does not exist\n", id);
	else {
		fprintf(stderr, "Failed to delete task: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(stmt);
}

void editTask(sqlite3 *db, int id, const char *new_title) {
	const char *sql =
		"UPDATE tasks "
		"SET title = ? "
		"WHERE id = ?;";
	sqlite3_stmt *stmt;

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		return;
	}

	if (sqlite3_bind_text(stmt, 1, new_title, -1, SQLITE_TRANSIENT) !=SQLITE_OK) {
		fprintf(stderr, "Failed to bind task new title\n");
		sqlite3_finalize(stmt);
		return;
	}

	if (sqlite3_bind_int(stmt, 2, id) != SQLITE_OK) {
		fprintf(stderr, "Failed to bind task id\n");
		sqlite3_finalize(stmt);
		return;
	}

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		fprintf(stderr, "Failed to edit task: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return;
	}

	printf("Task edited: %d - '%s'\n", id, new_title);

	sqlite3_finalize(stmt);
}

void clearData(sqlite3 *db) {
	const char *sql = "DELETE FROM tasks;";
	char *err_msg = NULL;

	if (sqlite3_exec(db, sql, NULL, NULL, &err_msg) != SQLITE_OK) {
		fprintf(stderr, "Failed to clear tasks\n");
		sqlite3_free(err_msg);
		return;
	}

	printf("Tasks data cleared!\n");
}
