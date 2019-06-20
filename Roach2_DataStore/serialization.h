/* @file serialization.h
 * @date 14.06.2019
 * @brief Serialization methods implemented for data frame serialization
 * @author Christian Mollière
 * @copyright KSat Stuttgart e.V. Roach2 software team
 * */

#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <string>
#include <vector>

template <typename T> std::string serializeInteger(const T integer);
template <typename T> T deserializeInteger(std::string * archive);

std::string serializeDouble(const double num);
double deserializeDouble(std::string * archive);

std::string serializeString(const std::string str);
std::string deserializeString(std::string * archive);

std::string serializeStringVector(const std::vector<std::string> stringVector);
std::vector<std::string> deserializeStringVector(std::string * archive);

std::string serializeDoubleVector(std::vector<double> doubleVector);
std::vector<double> deserializeDoubleVector(std::string * archive);

#endif
