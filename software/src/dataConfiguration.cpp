#include "dataConfiguration.h"

class WritableData *currentRoomTemperature = createWritableDataPoint("CurrentRoomTemperature", DATA_10_INT, 30000,
                                                                     prepareSendCurrentRoomTemperature);

class WritableData *desiredRoomTemperature = createWritableDataPoint("DesiredRoomTemperature", DATA_1_INT, 0,
                                                                     prepareSendDesiredRoomTemperature);

void configureData() {
    debugPrintln("CONFIGURE DATA");
    currentRoomTemperature->preventCommunicationAfter(1000 * 60 * 30); // 30 min
}
