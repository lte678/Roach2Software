/* @file database.h
 * @date 16.01.2019
 * @brief Implementation of the SQLite3 database for the ROACH2 experiment
 *
 * @author Christian Mollière
 * @copyright KSat Stuttgart e.V. Roach2 software team
 * */

#include "database.h"


/*
 * 1. filesystem functions
 * */

std::vector<std::string> findFiles(const std::string pname, const std::string ext) {
    /* FINDFILES returns a vector of all filenames as strings in a given directory path */
    DIR *dir;
    struct dirent *ent;
    std::vector<std::string> files;

    if ((dir = opendir(pname.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            files.emplace_back(ent->d_name);
        }
        closedir(dir);
    }

    /* filter files for extensions */
    if (ext.empty()) {
        return files;
    } else {
        std::vector<std::string> filtered_files;
        for (int i = 0; i < files.size(); i++) {
            int found = (int) files[i].rfind(ext);
            if ((found != -1) && (files[i].size() - found == ext.size())) {
                filtered_files.emplace_back(files[i]);
            }
        }
        return filtered_files;
    }
}

int detectFileVersion(std::string file) {
    /* detects file version by reading the last number in the file name */
    int found_ext = (int) file.find_last_of(".");
    file = file.substr(0, found_ext);
    size_t last_index = file.find_last_not_of("0123456789");
    if (last_index + 1 != file.size()) {
        return (int) std::stol(file.substr(last_index + 1));
    } else {
        return 0;
    }
}


/*
 * 2. data type functions
 * */
std::string guessType(const std::string value) {
    /* guessType guesses the data type of a value converted to a string */
    if ((int) value.find_first_not_of("0123456789") == -1) {
        return "INTEGER";
    } else if (((int) value.find_first_not_of("0123456789.") == -1) &&
               (value.find_first_of(".") == value.find_last_of("."))) {
        return "REAL";
    } else {
        return "TEXT";
    }
}


/*
 * 3. SQLAnswer Constructor
 * */

SQLAnswer::SQLAnswer(int argc, char **argv, char **azColName) {
    // clear vectors
    columns.clear();
    values.clear();
    // get query data
    for (int i = 0; i < argc; i++) {
        columns.emplace_back(azColName[i]);
        values.emplace_back(argv[i] ? argv[i] : "NULL");
    }
}


/*
 * 4. Database Constructors
 * */

Database::Database(){};

Database::Database(std::string pname, bool createNewDatabase) {
    /* @brief automatically creates database versions */
    // list all files in pname
    std::vector<std::string> files = findFiles(pname, DATABASE_FILE_EXTENSION);
    // detect newest db file
    int newest = 0;
    std::string fname = "";
    for (int i = 0; i < files.size(); i++) {
        int version = detectFileVersion(files[i]);
        if (version > newest) {
            newest = version;
            fname = files[i];

        }
        // strip pname of / if needed
        char lastCharacterOfPname = pname[pname.size() - 1];
        if ((lastCharacterOfPname == '/') || (lastCharacterOfPname == '\\')) {
            pname = pname.substr(0, pname.size() - 1);
        }
        // create new database file or load the latest
        if (createNewDatabase || (newest == 0)) {
            fname = pname + "/" + DATABASE_FILE_PREFIX + std::to_string(newest + 1) + DATABASE_FILE_EXTENSION;
        } else {
            fname = pname + "/" + fname;
        }
        // open database
        open(fname);
    }
}


/*
 * 5. Database Callback
 * */

int Database::callback(void *data, int argc, char **argv, char **azColName) {
    /* @ brief static callback to catch queryResults from database
     * */
    // print query results
    if (DATABASE_DEBUG) {
        for (int i = 0; i < argc; i++) {
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }
        printf("\n");
    }
    // save query results to data base buffer
    auto database = (Database *) (data);
    SQLAnswer result(argc, argv, azColName);
    database->queryResults.emplace_back(result);

    return 0;
}


/*
 * 4. Database Basic Operations
 * */

int Database::open(std::string fname) {
    /* @brief opens a database or creates a new one if fname does not exist
     * */
    this->not_connected = sqlite3_open(fname.c_str(), &db);
    if (!this->not_connected){
        // save path to database
        this->path = fname;
        if(DATABASE_DEBUG) fprintf(stderr, "Opened database successfully\n");
    }else{
        if(DATABASE_DEBUG) fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    return this->not_connected;
}

int Database::execute(std::string sql_statement) {
    /* @brief executes a given sql statement
     * */
    // check if database is open
    if(this->not_connected){
        return 0;
    }
    queryResults.clear();
    // execute statement
    int rc = sqlite3_exec(db, sql_statement.c_str(), callback, (void *) this, &zErrMsg);
    if (DATABASE_DEBUG) {
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
    }
    return rc;
}

void Database::close() {
    /* closes the current database */
    sqlite3_close(db);
    this->not_connected = 1;
    this->path = "";
    if (DATABASE_DEBUG) {
        fprintf(stderr, "Database closed\n");
    }
}


/*
 * 6. Database Specific Operations
 * */

int Database::saveData(Data data){
    /* @brief Saves data object to currently opened database
     * @param Data data, instance of data class containing data to save
     * @return int, successfully saved (1) or not (0)
     * */
    // check if data is valid and database is there
    if(!data.is_valid() || this->not_connected){
        return 0;
    }
    // fetch columnNames and values
    std::vector<std::string> columnNames = data.getColumnNames();
    columnNames.insert(columnNames.begin(),"time");
    std::vector<std::string> values;
    values.push_back(std::to_string(data.getTime()));
    for(int i = 0; i<data.getValues().size(); i++) values.push_back(std::to_string(data.getValues()[i]));

    // create table if not exists
    std::string sql;
    sql = "CREATE TABLE IF NOT EXISTS ";
    sql += data.getId();
    sql += "(id INTEGER PRIMARY KEY AUTOINCREMENT);";
    execute(sql);
    // read all columns
    sql = "PRAGMA table_info(";
    sql += data.getId();
    sql += ");";
    execute(sql);
    // create a column if not available
    sql = "";
    for (int i = 0; i < columnNames.size(); i++) {
        bool columnExists = false;
        for (int j = 0; j < queryResults.size(); j++) {
            // queryResults[j].values[1] == column name
            if (columnNames[i] == queryResults[j].values[1]) {
                columnExists = true;
            }
        }
        if (!columnExists) {
            sql += "ALTER TABLE ";
            sql += data.getId();
            sql += " ADD COLUMN ";
            sql += columnNames[i];
            sql += " ";
            sql += guessType(values[i]);
            sql += ";";
        }
    }
    execute(sql);
    // insert values
    // "INSERT INTO position (id,x,y,z) VALUES (2,1,0,0), (3,1,1,0), (4,1,1,1);"
    sql = "INSERT INTO ";
    sql += data.getId();
    sql += " (";
    sql += columnNames[0];
    for (int i = 1; i < columnNames.size(); i++) {
        sql += "," + columnNames[i];
    }
    sql += ") VALUES (";
    if (guessType(values[0]) == "TEXT") sql += "\"";
    sql += values[0];
    if (guessType(values[0]) == "TEXT") sql += "\"";
    for (int i = 1; i < values.size(); i++) {
        sql += ",";
        if (guessType(values[i]) == "TEXT") sql += "\"";
        sql += values[i];
        if (guessType(values[i]) == "TEXT") sql += "\"";
    }
    sql += ");";
    execute(sql);
    // print status
    if (DATABASE_DEBUG) {
        fprintf(stderr, "Data written successfully to database!\n");
    }
    return 1;
}

std::vector<Data> Database::loadData(std::string id) {
    /* @brief loads all data from the table named id in the currently opened database
     * @param std::string id, table name
     * @return std::vector<Data>. a vector containing Data objects, empty if table was empty or non existent
     * */
    // init return vector
    std::vector<Data> data;

    // check if database is there
    if(this->not_connected){
        return data;
    }

    // poll database
    std::string sql = "SELECT * FROM ";
    sql += id;
    sql += ";";
    execute(sql);

    // reformat SQLAnswers in Data objects
    for(int i = 0; i < this->queryResults.size(); i++){
        // read time
        unsigned long time_ = std::stoul(this->queryResults[i].values[1]);
        // read columnNames and values
        std::vector<std::string> columnNames_;
        std::vector<double> values_;
        for(int j = 2; j < this->queryResults[i].columns.size(); j++) {
            columnNames_.push_back(this->queryResults[i].columns[j]);
            values_.push_back(std::stod(this->queryResults[i].values[j]));
        }
        // assemble temporary data instance
        Data temp(id,time_,columnNames_,values_);
        // push back into return vector
        data.push_back(temp);
    }

    return data;
}

Data Database::loadLastData(std::string id) {
    /* @brief loads last data entry from the table named id in the currently opened database
     * @param std::string id, table name
     * @return Data, a data object, empty if table was empty or non existent
     * */
    // check if database is there
    if(this->not_connected){
        Data data;
        return data;
    }

    // poll database
    std::string sql = "SELECT * FROM ";
    sql += id;
    sql += ";";
    execute(sql);

    // pick up last index
    unsigned long idx = this->queryResults.size()-1;

    // reformat SQLAnswers in Data objects
    if(this->queryResults.size()){
        // read time
        unsigned long time_ = std::stoul(this->queryResults[idx].values[1]);
        // read columnNames and values
        std::vector<std::string> columnNames_;
        std::vector<double> values_;
        for(int j = 2; j < this->queryResults[idx].columns.size(); j++) {
            columnNames_.push_back(this->queryResults[idx].columns[j]);
            values_.push_back(std::stod(this->queryResults[idx].values[j]));
        }
        // assemble temporary data instance
        Data data(id,time_,columnNames_,values_);
        return data;
    }else{
        // return empty if no table or empty table
        Data data;
        return data;
    }
}

int Database::saveImage(){
    /* TODO implement image saving when image container is ready
     * use this->path to place images next to database
     * */
    return 0;
}