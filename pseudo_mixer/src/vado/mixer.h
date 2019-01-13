/*
  mixer.h - Library for constructing data packets for a Vado Identity Mixer
  Created by Russell Keane, December 8, 2018.
  Released into the public domain.
*/
#ifndef vado_mixer
#define vado_mixer

#include "Arduino.h"
#include "constants.h"
#include "systemState.h"
#include "temperature_sensor.h"

class Mixer
{
  public:
    Mixer(int powerPin, int solenoidSelectionPin, int proportioningValvePowerPin, int proportioningValveDirectionPin, ThermistorParams thermistorParams);
    bool GetResponse(ControllerState& controllerState, byte setMsg[]);
    void UpdateSystemState(ControllerState& controllerState);
    void Process();
    
  private:
    int _powerPin = 0;
    int _solenoidSelectionPin = 0;
    int _proportioningValvePowerPin = 0;
    int _proportioningValveDirectionPin = 0;

    bool _isOn = false;
    int _targetTemperature = 0;

    char _curMsg = INTRO;
    char CalcResponse(ControllerState& controllerState);
    const void SetMessage(char required, byte setMsg[]);

    //Outgoing messages
    static const byte MSG_INTRO[MSG_LEN];
    static const byte MSG_PREPARING[MSG_LEN];
    static const byte MSG_HEARTBEAT[MSG_LEN]; //it might be that this should be the current mixer water temperature
    static const byte MSG_READY[MSG_LEN];
    //Outgoing message types
    static const char INTRO = 0;
    static const char PREPARING = 2;
    static const char HEARTBEAT = 3;
    static const char READY = 4;

    TemperatureSensor _temperatureSensor;
    enum TempSuitability {
      TooCold,
      JustRight,
      TooHot
    };

    static TempSuitability EvaluateTempSuitability(int requiredTemp, float currentTemp);

    //relays used switch on (leds lit) with a LOW signal
    //so use these values to reverse the signal without making the code confusing
    #define RELAY_HIGH LOW
    #define RELAY_LOW HIGH

    static const float ALLOWABLE_TEMP_DIFFERENTIAL;
    void ChangeTemp(bool makeColder);
};

#endif