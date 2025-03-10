#include "dataConfiguration.h"

class WritableData *currentRoomTemperature = createWritableDataPoint("CurrentRoomTemperature", DATA_10_INT, 30000, 0x01,
                                                                     prepareSendCurrentRoomTemperature);

#ifdef ENABLE_SLOT_2
class WritableData *currentRoomTemperatureH2 = createWritableDataPoint("CurrentRoomTemperatureH2", DATA_10_INT, 30000, 0x02,
                                                                       prepareSendCurrentRoomTemperatureH2);
#endif
#ifdef ENABLE_SLOT_3
class WritableData *currentRoomTemperatureH3 = createWritableDataPoint("CurrentRoomTemperatureH3", DATA_10_INT, 30000, 0x02,
                                                                       prepareSendCurrentRoomTemperatureH3);
#endif

class WritableData *desiredRoomTemperature = createWritableDataPoint("DesiredRoomTemperature", DATA_1_INT, 0, 0x01,
                                                                     prepareSendDesiredRoomTemperature);

void configureData() {
    debugPrintln("CONFIGURE DATA");
    currentRoomTemperature->preventCommunicationAfter(1000 * 60 * 30); // 30 min
}
