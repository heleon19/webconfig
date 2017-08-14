
#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "webconfig.h"


/* WebConfig */
WebConfig config(true); // debug config
void printWebConfigParams(void);

/* Program sequencer */
#define STEP_WAIT_FOR_WIFI_CONNECT  0
#define STEP_OPERATION              1
uint8_t step = 0;
bool debug = 1;

void setup(void)
{
  delay(1000);
  Serial.begin(115200);

  config.init();
  if(debug) printWebConfigParams();
}

void loop(void)
{
  switch (step)
  {
    case STEP_WAIT_FOR_WIFI_CONNECT:
    {
      if(config.isWifiConnected())
      {
        if(debug)
        {
          Serial.print(F("IP address: "));
          Serial.println(WiFi.localIP());
        }
        step = STEP_OPERATION;
      }
      break;
    }
    case STEP_OPERATION:
    {
      //Here we are connected to wifi and can execute our application
    }
  }

  config.loop(); // Returns 0 if config remains, returns value >0 if config changed
}

void printWebConfigParams(void)
{
  Serial.println(config.getParameter(PARAM_NAME_AP_SSID));
  Serial.println(config.getParameter(PARAM_NAME_AP_PSWD));
  Serial.println(config.getParameter(PARAM_NAME_CL_SSID));
  Serial.println(config.getParameter(PARAM_NAME_CL_PSWD));
}
