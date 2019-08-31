/* @file main.cpp
 * @date 20.06.2019
 * @brief Example for database usage
 *
 * @author Christian Mollière
 * @copyright KSat Stuttgart e.V. Roach2 software team
 * */
#ifdef NOT_USED
#include "lib/database.h"
#include "lib/data.h"
#include <vector>
#include <string>
#include <iostream>
#endif

int main(int argc, char** argv) {
}

#ifdef NOT_USED

int main2() {
    /* Hello guys.
     * This is a short example on how to use the ROACH2 database.
     * Please look at the source for more detailed descriptions of the functions.
     *
     * Keep in mind that a Data object always represents one set of measurements at a given time.
     * To handle multiple data objects simply use a std::vector<Data>.
     *
     * Also the id string of each data object will be used to setup a table. Therefore, equal data sources should
     * always generate Data objects with equal (case sensitive!) id strings.
     *
     * The image saving is not yet implemented but would need to be placed in the database.h/database.cpp files.
     * */

    /*
     * To create a new database call
     * */
    Database db;

    /*
     * Now we will need to open a database or create a new one.
     * Therefore, we will first define a location for our new database.
     * */
    std::string fname = "/Users/chris_mac/Desktop/example.db";

    /*
     * To open the database, simply call.
     * If SQLite3 does not find a file in this location, it will simply create a new empty one.
     * */
    db.open(fname);

    /*
     * Now lets save some data. For this we first create some dummy data,
     * using the Data Class.
     * */
    Data dummy_data;

    /*
     * Lets create some dummy entries
     * (this is kind of ugly)
     * */
    std::string id = "SENSOR_IMU_EXAMPLE";
    unsigned long time = 0;
    std::vector<std::string> columnNames;
    columnNames.push_back("ACC_X");
    columnNames.push_back("ACC_Y");
    columnNames.push_back("ACC_Z");
    std::vector<double> values;
    values.push_back(-123);
    values.push_back(0);
    values.push_back(0.123);

    /*
     * To save these into the data object use
     * */
    dummy_data.setId(id);
    dummy_data.setTime(time);
    dummy_data.setColumnNames(columnNames);
    dummy_data.setValues(values);

    /*
     * We could also use the second constructor given by the data object
     * */
    Data dummy_data2 (id,time,columnNames,values);

    /*
     * Now lets save these data objects to our database
     * */
    db.saveData(dummy_data);
    db.saveData(dummy_data2);

    /*
     * Now to get all entries in the database using the id "SENSOR_IMU_EXAMPLE"
     * we can use the following function
     * */
    std::vector<Data> all_entries = db.loadData("SENSOR_IMU_EXAMPLE");

    /*
     * If we only want the last entry we can call
     * */
    Data last_entry = db.loadLastData("SENSOR_IMU_EXAMPLE");

    /*
     * Lets use the inbuilt print function to have a look at the data
     * */
    std::cout << "Printing last entry of SENSOR_IMU_TABLE" << std::endl;
    last_entry.print();
    std::cout << std::endl;

    /*
     * Finally when we are done with the database, we can simply close it
     * */
    db.close();

    /*
     * Additionally, we can serialize any Data object for transmission encoding.
     * To do this, call any Data object using
     * */
    std::string serialized_data = dummy_data.serialize();

    /*
     * To reconstruct the data from the string use
     * */
    Data dummy_data3;
    dummy_data3.deserialize(&serialized_data);

    /*
     * We can always perform a quick check if the data seems valid.
     * Which is true when the id is not empty and the columnNames have the same length as the values.
     * */
    std::cout << "dummy_data3 sucessfully serialized? " << dummy_data3.is_valid() << std::endl;

    /*
     * You can also serialize multiple objects into the same string
     * and deserialize them afterwards. Just be sure to deserialize them in the inverse
     * order as you serialized them.
     * */

    return 0;
}
#endif