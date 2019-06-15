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
#include <iostream>

template <typename T>
std::string serializeInteger(const T integer) {
	/* @brief serializes an integer of type T
	 * @param integer to serialize
	 * @return string containing serialized data
	 * */
	std::string str;
	for (int i = 0; i < sizeof(T); i++) {
		str += static_cast<uint8_t>(integer >> (i * 8)); // 8 bits per byte
	}
	return str;
}

template <typename T>
T deserializeInteger(std::string * archive) {
	/* @brief deserializes integer from string archive
	 * @param archive string which contains serialized data
	 * @return integer of type T
	 * */
	T integer = 0;
	for (int i = sizeof(T) - 1; i >= 0; i--) {
		// i dont know why but the cast only works if done in separate steps
		auto c = static_cast<uint8_t>(archive->back());
		auto c2 = static_cast<T>(c);
		uint64_t c3 = c2 << (i * 8);
		integer += c3;
		archive->pop_back();
	}
	return integer;
}

std::string serializeDouble(const double num) {
	/* @brief serializes double to a string
	 * @param num double to serialize
	 * @return string
	 * */
	uint64_t buffer;
	memcpy((void*)& buffer, (void*)& num, sizeof(double));
	return serializeInteger<uint64_t>(buffer);
}

double deserializeDouble(std::string * archive) {
	/* @brief deserializes double from string archive
	 * @param archive string which contains serialized data
	 * @return double
	 * */
	auto buffer = deserializeInteger<uint64_t>(archive);
	double num;
	memcpy((void*)& num, (void*)& buffer, sizeof(double));
	return num;
}

std::string serializeString(const std::string str) {
	/* @brief serializes str, first all characters of str are
	 * parsed and then the number of characters is appended.
	 * @param  str string to serialize
	 * @return string
	 * */
	return str + serializeInteger<unsigned long>(str.length());
}

std::string deserializeString(std::string * archive) {
	/* @brief deserializes string from string archive
	 * @param archive string which contains serialized data
	 * @return string
	 * */
	 // get length
	auto length = deserializeInteger<unsigned long>(archive);
	// preallocate string
	std::string str(length, '0');
	// get characters
	for (unsigned long i = 0; i < length; i++) {
		str[length - i - 1] = archive->back();
		archive->pop_back();
	}
	return str;
}

std::string serializeStringVector(const std::vector<std::string> stringVector) {
	/* @brief serializes a vector of strings
	 * @param stringVector a vector containing strings objects
	 * @return string containing serialized data
	 * */
	 // init archive string
	std::string archive;
	// get vector length
	unsigned long length = stringVector.size();
	// serialize contents
	for (unsigned long i = 0; i < length; i++) {
		archive += serializeString(stringVector[length - i - 1]);
	}
	// serialize vector length
	archive += serializeInteger<unsigned long>(length);
	return archive;
}

std::vector<std::string> deserializeStringVector(std::string * archive) {
	/* @brief deserializes vector of strings from string archive
	 * @param archive string which contains serialized data
	 * @return vector of strings
	 * */
	 // get length
	auto length = deserializeInteger<unsigned long>(archive);
	// init vector
	std::vector<std::string> stringVector;
	// deserialize strings
	for (char i = 0; i < length; i++) {
		stringVector.push_back(deserializeString(archive));
	}
	return stringVector;
}

std::string serializeDoubleVector(std::vector<double> doubleVector) {
	/* @brief serializes a vector of doubles
	 * @param doubleVector a vector containing strings objects
	 * @return string containing serialized data
	 * */
	 // init archive string
	std::string archive;
	// get vector length
	unsigned long length = doubleVector.size();
	// serialize contents
	for (unsigned long i = 0; i < length; i++) {
		archive += serializeDouble(doubleVector[length - i - 1]);
	}
	// serialize vector length
	archive += serializeInteger<unsigned long>(length);
	return archive;
}

std::vector<double> deserializeDoubleVector(std::string * archive) {
	/* @brief deserializes vector of doubles from string archive
	 * @param archive string which contains serialized data
	 * @return vector of doubles
	 * */
	 // get length
	auto length = deserializeInteger<unsigned long>(archive);
	// init vector
	std::vector<double> doubleVector;
	// deserialize doubles
	for (char i = 0; i < length; i++) {
		doubleVector.push_back(deserializeDouble(archive));
	}
	return doubleVector;
}

#endif
