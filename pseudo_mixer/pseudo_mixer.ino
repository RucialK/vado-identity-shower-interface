#include "src/vado/controller.h"
#include "src/vado/mixer.h"

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

Controller controller;
ControllerState contState;
Mixer mixer;

void setup() {
  Serial.begin(baudRate, serialConfig);
  Serial1.begin(baudRate, serialConfig);

  Serial.setTimeout(MAX_READ_TIME);
}

void loop()
{
  if (ReadData()) {
    contState = controller.parse(receivedData);
    if (contState.isValid() && controller.StateChanged(contState)) {
      controller.SetState(contState);
    }
    PrintData();
  }

  mixer.GetResponse(contState, message);
  SendData(message);
}

void PrintData() {
  static bool bOn = false;
  static bool bPaused = false;
  static int temp = 0;
  static int flow = 0;
  static int outlet = 0;

  bool stateChanged = false;
  if (contState.isOn() != bOn) {
    bOn = contState.isOn();
    stateChanged = true;
  }
  if (contState.isPaused() != bPaused) {
    bPaused = contState.isPaused();
    stateChanged = true;
  }
  if (contState.temp() != temp) {
    temp = contState.temp();
    stateChanged = true;
  }
  if (contState.flow() != flow) {
    flow = contState.flow();
    stateChanged = true;
  }
  if (contState.isMainOutlet() != outlet) {
    outlet = contState.isMainOutlet();
    stateChanged = true;
  }

  if (stateChanged) {
    Serial.println("State changed");
    Serial.print("Power is ");
    Serial.println(bOn ? "on" : bPaused ? "paused" : "off");
    
    Serial.print("Temp is ");
    Serial.println(temp);
    Serial.print("Flow is ");
    Serial.println(flow);
    Serial.print("Outlet is ");
    Serial.println(outlet);
    Serial.println("");
  }
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
    return MSG_LEN == Serial1.readBytes(receivedData, MSG_LEN);
  }
  return false;
}
