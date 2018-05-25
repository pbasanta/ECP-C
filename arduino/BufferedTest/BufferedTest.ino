#include <SerialPort.h>

// port zero, 63 character RX and TX buffers
SerialPort<0, 63, 63> NewSerial;

void setup() {
  NewSerial.begin(9600);
while(){
  while (!NewSerial.available()) {}
  do {
    NewSerial.write(NewSerial.read());
  } while(NewSerial.available());
}
}
void loop() {
  
}
