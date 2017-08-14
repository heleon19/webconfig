
#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "webconfig.h"


/* WebConfig */
#define PARAMS_SIZE 7
WebConfigParam params[PARAMS_SIZE];
WebConfig config(true); // debug config
void printWebConfigParams(void);

/* Script to customize webconfig html page
 * we overwrite the page titel */
PROGMEM const char script[] = ""
  "document.addEventListener('DOMContentLoaded', function(event){\n"
    "document.getElementById('title').innerHTML='Mqtt Controller';\n"
    "document.getElementsByTagName('title')[0].innerHTML='Mqtt Controller';\n"
  "});\n";

/* Program sequencer */
#define STEP_WEB_CONFIG             0
#define STEP_WAIT_FOR_WIFI_CONNECT  1
#define STEP_OPERATION              3
uint8_t step = 0;
bool debug = 1;

void setup(void)
{
  delay(1000);
  Serial.begin(115200);

  /* Additional custom script for the web configurator */
  config.setCustomJS(script);

  /* Define custom web configurator parameters and their default values */
  int i = 0;
  strncpy(params[i].name, "mqtt-client", MAX_VALUE_SIZE);
  strncpy(params[i++].value, WiFi.macAddress().c_str(), MAX_VALUE_SIZE);
  strncpy(params[i].name, "mqtt-host", MAX_VALUE_SIZE);
  strncpy(params[i++].value, "mqtt.my-server.ch", MAX_VALUE_SIZE);
  strncpy(params[i].name, "mqtt-port", MAX_VALUE_SIZE);
  strncpy(params[i++].value, "1883", MAX_VALUE_SIZE);
  strncpy(params[i].name, "mqtt-user", MAX_VALUE_SIZE);
  strncpy(params[i++].value, "reto", MAX_VALUE_SIZE);
  strncpy(params[i].name, "mqtt-password", MAX_VALUE_SIZE);
  strncpy(params[i++].value, "", MAX_VALUE_SIZE);
  strncpy(params[i].name, "mqtt-topic", MAX_VALUE_SIZE);
  strncpy(params[i++].value, "MYTOPIC", MAX_VALUE_SIZE);
  strncpy(params[i].name, "device-options", MAX_VALUE_SIZE);
  strncpy(params[i++].value, "0", MAX_VALUE_SIZE);
}

void loop(void)
{
  switch (step)
  {
    case STEP_WEB_CONFIG:
    {
      config.init(params, PARAMS_SIZE);
      if(debug) printWebConfigParams();
      step = STEP_WAIT_FOR_WIFI_CONNECT;
      break;
    }
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
  Serial.println(config.getParameter("mqtt-host"));
  Serial.println(config.getParameter("mqtt-port"));
  Serial.println(config.getParameter("mqtt-user"));
  Serial.println(config.getParameter("mqtt-password"));
  Serial.println(config.getParameter("mqtt-topic"));
  Serial.println(config.getParameter("device-options"));
}
