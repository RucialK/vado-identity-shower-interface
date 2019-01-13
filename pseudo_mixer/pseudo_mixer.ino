#include "src/vado/controller.h"
#include "src/vado/mixer.h"
#include "src/vado/thermistor_params.h"

/////////////////////////////////////////////////////////////////
//Transmission Details
//Serial config
long baudRate = 9600;
byte serialConfig = SERIAL_8N2;

//max expected time for reading data
const int MAX_READ_TIME = 10;
//Transmission Details
/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//Timing
//Vars used to ensure signals are sent at the correct times to the controller
long unsigned lastCom = 0;
long unsigned curMicros = 0;
const unsigned long comPeriod = 55240; //should 55240 - 55280 - Arduino is not perfect at timing so 55240 seems to equate to roughly 55260
const unsigned long MIN_READ_MILLIS = 10;
//Timing
/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//Outgoing message details
//Outgoing register
byte message[MSG_LEN];
//Outgoing message details
/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//Incoming register
byte receivedData[MSG_LEN];
int nCurByte = 0;
//Controller message details
/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//Pins
//State pins
const int POWER_PIN = 3;

//Thermistor Pins
const int THERMISTOR_POWER_PIN = 2;
const int THERMISTOR_PIN = A0;

//Proportioning Valve Pins
const int PROPORTIONING_VALVE_POWER_PIN = 4;
const int PROPORTIONING_VALVE_DIRECTION_PIN = 5;
//Pins
/////////////////////////////////////////////////////////////////


Controller controller;
ThermistorParams thermistorParams(THERMISTOR_PIN, 3435, 9940, THERMISTOR_POWER_PIN);
Mixer mixer(POWER_PIN, PROPORTIONING_VALVE_POWER_PIN, PROPORTIONING_VALVE_DIRECTION_PIN, thermistorParams);

void setup() {
  Serial.begin(baudRate, serialConfig);
  Serial1.begin(baudRate, serialConfig);

  Serial.setTimeout(MAX_READ_TIME);
}

void loop()
{
  if (ReadData()) {
    ControllerState requestedState = controller.parse(receivedData);
    if (requestedState.isValid()) {
      if (controller.StateChanged(requestedState)) {
        controller.SetState(requestedState);
        PrintData(requestedState);
        mixer.UpdateSystemState(requestedState);
      }
    }
    mixer.GetResponse(requestedState, message);
  }

  mixer.Process();

  SendData(message);
}

void PrintData(ControllerState& newState) {
  Serial.println("State changed");
  Serial.print("Power is ");
  Serial.println(newState.isOn() ? "on" : newState.isPaused() ? "paused" : "off");
  
  Serial.print("Temp is ");
  Serial.println(newState.temp());
  Serial.print("Flow is ");
  Serial.println(newState.flow());
  Serial.print("Outlet is ");
  Serial.println(newState.isMainOutlet());
  Serial.println("");
}

void SendData(byte sendMsg[]) {
  curMicros = micros();
  if ((curMicros > (lastCom + comPeriod)) || (curMicros < lastCom)) {
    lastCom = curMicros;
    Serial1.write(sendMsg, MSG_LEN);
  }
}

bool ReadData() {
  if (Serial1.available() > 0) {
    byte byteRead;
    unsigned long startMillis = millis();
    unsigned long readTimeout = startMillis + MIN_READ_MILLIS;
    int nCurByte = 0;
    while (Serial1.available() > 0 || millis() < readTimeout || millis() < startMillis) {
      if (Serial1.available() > 0) {
        byteRead = Serial1.read();
        receivedData[nCurByte++] = byteRead;
        if (nCurByte >= MSG_LEN) {
          return true;
        }
        else if (byteRead == PACKET_TERMINATOR) {
          return false;
        }
      }
    }
  }
  return false;
}
