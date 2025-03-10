#include "dataConfiguration.h"

#ifdef ENABLE_SLOT_1
class WritableData *currentRoomTemperature = createWritableDataPoint("CurrentRoomTemperature", DATA_10_INT, 30000, 0x01,
                                                                     prepareSendCurrentRoomTemperature);

class WritableData *desiredRoomTemperature = createWritableDataPoint("DesiredRoomTemperature", DATA_1_INT, 0, 0x01,
                                                                     prepareSendDesiredRoomTemperature);
#endif

#ifdef ENABLE_SLOT_2
class WritableData *currentRoomTemperatureH2 = createWritableDataPoint("CurrentRoomTemperatureH2", DATA_10_INT, 30000, 0x02,
                                                                       prepareSendCurrentRoomTemperatureH2);
class WritableData *desiredRoomTemperatureH2 = createWritableDataPoint("DesiredRoomTemperatureH2", DATA_1_INT, 0, 0x02,
                                                                     prepareSendDesiredRoomTemperatureH2);
#endif
#ifdef ENABLE_SLOT_3
class WritableData *currentRoomTemperatureH3 = createWritableDataPoint("CurrentRoomTemperatureH3", DATA_10_INT, 30000, 0x02,
                                                                       prepareSendCurrentRoomTemperatureH3);
class WritableData *desiredRoomTemperatureH3 = createWritableDataPoint("DesiredRoomTemperatureH3", DATA_1_INT, 0, 0x03,
                                                                       prepareSendDesiredRoomTemperatureH3);
#endif


void configureData() {
    debugPrintln("CONFIGURE DATA");
#ifdef ENABLE_SLOT_1
    currentRoomTemperature->preventCommunicationAfter(1000 * 60 * 30); // 30 min
#endif
#ifdef ENABLE_SLOT_2
    currentRoomTemperatureH2->preventCommunicationAfter(1000 * 60 * 30); // 30 min
#endif
#ifdef ENABLE_SLOT_3
    currentRoomTemperatureH3->preventCommunicationAfter(1000 * 60 * 30); // 30 min
#endif
}