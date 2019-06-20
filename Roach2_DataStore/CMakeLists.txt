cmake_minimum_required(VERSION 3.5)
project(database)

set(CMAKE_CXX_STANDARD 14)

add_executable(database main.cpp lib/serialization.cpp lib/database.cpp lib/data.cpp lib/serialization.cpp lib/sqlite/sqlite3.c)

# link sqlite
# link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/sqlite)
# target_link_libraries (database ${CMAKE_CURRENT_SOURCE_DIR}/lib/sqlite/sqlite3.h)