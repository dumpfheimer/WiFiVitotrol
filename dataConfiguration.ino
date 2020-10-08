class WritableData *currentRoomTemperature = createWritableDataPoint("CurrentRoomTemperature", DATA_10_INT, 60000, prepareSendCurrentRoomTemperature);
class WritableData *desiredRoomTemperature = createWritableDataPoint("DesiredRoomTemperature", DATA_1_INT, 0, prepareSendDesiredRoomTemperature);

void configureData() {
  DEBUG_SERIAL.println("CONFIGURE DATA");
  currentRoomTemperature->preventCommunicationAfter(300000);
}
