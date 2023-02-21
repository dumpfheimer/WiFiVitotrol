#ifndef DATA_ENUMS_H
#define DATA_ENUMS_H

#include <Arduino.h>

#define WRITABLE_DATAPOINT_SIZE 10

enum DataType {
	DATA_1_INT = 1,
	DATA_10_INT = 10
};
enum DataSource {
	WIFI = 3,
	HEATER = 4
};


#endif