#ifndef webconfig_h
#define webconfig_h

#include <Arduino.h>
#include "FS.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define WEB_PARAM_SIZE 4
#define MAX_SYSTEM_VALUE_SIZE 128
#define MAX_VALUE_SIZE 32

#define TERMINATION_CHAR '&'

#define PARAM_NAME_AP_SSID "sys-ap-ssid"
#define PARAM_NAME_AP_PSWD "sys-ap-password"
#define PARAM_NAME_CL_SSID "sys-client-ssid"
#define PARAM_NAME_CL_PSWD "sys-client-password"

#define PARAM_CHANGED                1
#define NO_ERR                       0
#define ERR_OPEN_CONFIG_FOR_READ    -1
#define ERR_OPEN_CONFIG_FOR_WRITE   -3

struct WebConfigSysParam
{
  char name[MAX_VALUE_SIZE + 1];
  char value[MAX_SYSTEM_VALUE_SIZE + 1];
};

struct WebConfigParam
{
  char name[MAX_VALUE_SIZE + 1];
  char value[MAX_VALUE_SIZE + 1];
};

class WebConfig //: public SomeOtherClass
{
  public:
    WebConfig(void);
    WebConfig(bool debug);
    int8_t init(void);
    int8_t init(WebConfigParam params[], int size);
    void setCustomJS(const char * script);
    void setCustomCSS(const char * css);
    int8_t loop(void);
    const char * getParameter(const char * name);
    bool isWifiConnected(void);
  private:
    int8_t loadConfig(void);
    int8_t saveConfig(void);
    void updateWifi(void);
    void sendConfigJson(void);
    void receiveConfig(void);
};

#endif
