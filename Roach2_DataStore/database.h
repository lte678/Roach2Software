/* @file database.h
 * @date 16.01.2019
 * @brief Implementation of the SQLite3 database for the ROACH2 experiment
 *
 * @author Christian Mollière
 * @copyright KSat Stuttgart e.V. Roach2 software team
 * */

#ifndef DATABASE_H
#define DATABASE_H

#include "sqlite/sqlite3.h"
#include "data.h"
#include <cstdio>
#include <string>
#include <vector>
#include <dirent.h>

/* ############################################################################# */
/* 1. constants */

const std::string DATABASE_FILE_PREFIX = "run";
const std::string DATABASE_FILE_EXTENSION = ".db";
const bool DATABASE_DEBUG = false;


/* ############################################################################# */
/* 2. filesystem functions */

std::vector<std::string> findFiles(const std::string pname, const std::string ext = "");
int detectFileVersion(std::string file);


/* ############################################################################# */
/* 3. data type functions */

std::string guessType(const std::string value);


/* ############################################################################# */
/* 4. database */

class SQLAnswer {
    /* this class is needed to obtain and process the SQL answers of the database */
public:
    std::vector<std::string> columns;
    std::vector<std::string> values;

    SQLAnswer(int argc, char **argv, char **azColName);
};


class Database {
    /* the database class contains all needed functionality to read and write data into a SQLITE3 database */
private:
    // database essentials
    sqlite3 *db;
    char *zErrMsg = nullptr;
    const char *data = "Callback function called";
    // query
    std::vector<SQLAnswer> queryResults;
    // database informations
    std::string path = "";
    int not_connected = 1;

public:
    // constructors
    Database();
    Database(std::string pname, bool createNewDatabaseVersion); // this is a experimental versioning constructor

    // callback to catch queryResults
    static int callback(void *data, int argc, char **argv, char **azColName);

    // basic database operations
    int open(std::string fname);
    void close();
    int execute(std::string sql_statement);

    // specific database operations
    std::vector<Data> loadData(std::string id);
    Data loadLastData(std::string id);
    int saveData(Data data);
    int saveImage(); // TODO !

};

#endif
