//Receiver

#include <VirtualWire.h>

void setup() {
  vw_set_ptt_inverted(true);
  vw_set_rx_pin(12);
  vw_setup(4000);
  pinMode(13, OUTPUT);
  vw_rx_start();
  Serial.begin(9600);
}

void loop() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  
  if (vw_get_message(buf, &buflen)) {
    Serial.println(int(buf[0]));
    Serial.print("ok");
  }
}
