cmake_minimum_required(VERSION 3.10)

project(tsk C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SQLite3 REQUIRED)

add_executable(tsk
    src/main.c
    src/database.c
    src/tasks.c
    src/helper.c
)

target_include_directories(tsk PRIVATE
    include
)

target_link_libraries(tsk PRIVATE
	SQLite3::SQLite3
)
install(TARGETS tsk
	RUNTIME DESTINATION bin
)
