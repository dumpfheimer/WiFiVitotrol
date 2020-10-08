#ifdef ESP8266
void printAsHex(SoftwareSerial* serial, byte *array, unsigned int len)
{
    serial->print("0x");
    for (unsigned int i = 0; i < len; i++)
    {
      if ( array[i] >= 16 ) {
        serial->print(array[i], HEX);
      } else {
        serial->print('0');
        serial->print(array[i], HEX);
      }
    }
    serial->print('\n');
}
#endif

void printAsHex(HardwareSerial* serial, byte *array, unsigned int len)
{
    serial->print("0x");
    for (unsigned int i = 0; i < len; i++)
    {
      if ( array[i] >= 16 ) {
        serial->print(array[i], HEX);
      } else {
        serial->print('0');
        serial->print(array[i], HEX);
      }
    }
}
void clearBuff() {
  while (bufferPointer > 0) {
    buffer[bufferPointer] = 0;
    bufferPointer--;
  }
  buffer[0] = 0;
  bufferPointer = 0;
}
void printBuff() {
  uint8_t i = 0;
  DEBUG_SERIAL.println("BUFFER:");
  for (i = 0; i < bufferPointer; i = i + 1) {
    DEBUG_SERIAL.print(buffer[i], HEX);
    DEBUG_SERIAL.print(" ");
  }
  DEBUG_SERIAL.println("");
  DEBUG_SERIAL.flush();
}
