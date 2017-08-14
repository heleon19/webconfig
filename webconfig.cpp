#include <Arduino.h>
#include "webconfig.h"
#include "webpage.h"

bool _debug = 1;

WebConfigSysParam _systemParams[WEB_PARAM_SIZE];
WebConfigParam * _params;
int _paramsSize = 0;
int8_t _configChanged = 0;

ESP8266WebServer _server(80);
uint8_t _wifiState = 255;
const char * _customScript = "\n";
const char * _customCss = "\n";

WebConfig::WebConfig(void)
{
  /* mount file system */
  SPIFFS.begin();

  /* write system parameter default values */
  String ssid = "ESP_" + WiFi.macAddress();
  strncpy(_systemParams[0].name, PARAM_NAME_AP_SSID, MAX_VALUE_SIZE);
  strncpy(_systemParams[0].value, ssid.c_str(), MAX_SYSTEM_VALUE_SIZE);
  strncpy(_systemParams[1].name, PARAM_NAME_AP_PSWD, MAX_VALUE_SIZE);
  strncpy(_systemParams[1].value, "", MAX_SYSTEM_VALUE_SIZE);
  strncpy(_systemParams[2].name, PARAM_NAME_CL_SSID, MAX_VALUE_SIZE);
  strncpy(_systemParams[2].value, "", MAX_SYSTEM_VALUE_SIZE);
  strncpy(_systemParams[3].name, PARAM_NAME_CL_PSWD, MAX_VALUE_SIZE);
  strncpy(_systemParams[3].value, "", MAX_SYSTEM_VALUE_SIZE);

}

WebConfig::WebConfig(bool debug)
{
  _debug = debug;
  WebConfig();
}

int8_t WebConfig::init(void)
{
  WebConfigParam params[0];
  init(params, 0);
}

int8_t WebConfig::init(WebConfigParam params[], int size)
{
  _params = params;
  _paramsSize = size;

  loadConfig();

  WiFi.mode(WIFI_AP_STA);

  if(_debug) Serial.print(F("Try to connected to: "));
  if(_debug) Serial.println(getParameter(PARAM_NAME_CL_SSID));
  updateWifi();

  _server.on("/", [](){ _server.send(200, F("text/html"), _mainWebPage); });
  _server.on(F("/custom.js"), [](){ _server.send(200, F("application/javascript"), _customScript); });
  _server.on(F("/custom.css"), [](){ _server.send(200, F("text/css"), _customCss); });
  _server.on(F("/default.css"), [](){ _server.send(200, F("text/css"), _defaultCss); });
  _server.on(F("/config.json"), [=](){ sendConfigJson(); });
  _server.on(F("/store"), [=](){ receiveConfig(); });

  _server.begin();
  if(_debug) Serial.println(F("HTTP server started"));

  return NO_ERR;
}

int8_t WebConfig::loop(void)
{
  /* debug IP address if state changed */
  uint8_t state = WiFi.status();
  if(_debug && _wifiState != state)
  {
    Serial.print(F("IP address: "));
    Serial.println(WiFi.localIP());
  }
  _wifiState = state;

  /* handle web server requests */
  _server.handleClient();

  int8_t configChanged = _configChanged;
  if(configChanged > 0) updateWifi();
  _configChanged = 0;
  return configChanged;
}

void WebConfig::setCustomJS(const char * script)
{
  _customScript = script;
}

void WebConfig::setCustomCSS(const char * css)
{
  _customCss = css;
}

int8_t WebConfig::loadConfig(void)
{
  File configFile = SPIFFS.open(F("/config.txt"), "r");
  if (!configFile) {
    if(_debug) Serial.println(F("Failed to open config file"));
    return ERR_OPEN_CONFIG_FOR_READ;
  }

  String pair;
  do
  {
    pair = configFile.readStringUntil(TERMINATION_CHAR);
    int indxTermination = pair.indexOf('=');
    if(indxTermination > 0)
    {
      String name = pair.substring(0, indxTermination);
      String value = pair.substring(indxTermination + 1, pair.length());

      bool found = false;
      for(int i = 0; i < WEB_PARAM_SIZE && found == false; i++)
      {
        if(strcmp(name.c_str(), _systemParams[i].name) == 0)
        {
          strncpy(_systemParams[i].value, value.c_str(), MAX_SYSTEM_VALUE_SIZE);
          found = true;
        }
      }
      for(int i = 0; i < _paramsSize && found == false; i++)
      {
        if(strcmp(name.c_str(), _params[i].name) == 0)
        {
          strncpy(_params[i].value, value.c_str(), MAX_VALUE_SIZE);
          found = true;
        }
      }
    }
  }
  while(pair.length() > 0);

  return NO_ERR;
}

int8_t WebConfig::saveConfig(void)
{
  File configFile = SPIFFS.open(F("/config.txt"), "w+");
  if (!configFile)
  {
    if(_debug) Serial.println(F("Failed to open config file for writing"));
    return ERR_OPEN_CONFIG_FOR_WRITE;
  }

  for(int i = 0; i < WEB_PARAM_SIZE; i++)
  {
    configFile.print(_systemParams[i].name);
    configFile.print('=');
    configFile.print(_systemParams[i].value);
    configFile.print(TERMINATION_CHAR);
  }

  for(int i = 0; i < _paramsSize; i++)
  {
    configFile.print(_params[i].name);
    configFile.print('=');
    configFile.print(_params[i].value);
    configFile.print(TERMINATION_CHAR);
  }

  return NO_ERR;
}

const char * WebConfig::getParameter(const char * name)
{
  for(int i = 0; i < WEB_PARAM_SIZE; i++)
  {
    if(strcmp(name, _systemParams[i].name) == 0)
    {
      return _systemParams[i].value;
    }
  }
  for(int i = 0; i < _paramsSize; i++)
  {
    if(strcmp(name, _params[i].name) == 0)
    {
      return _params[i].value;
    }
  }
  return "null";
}

void WebConfig::sendConfigJson(void)
{
  /* count content lenght */
  int size = 2;
  for(int i = 0; i < WEB_PARAM_SIZE; i++)
  {
    size += strlen(_systemParams[i].name);
    size += strlen(_systemParams[i].value);
    size += 5;
    if(_paramsSize > 0 || i + 1 < WEB_PARAM_SIZE)
    {
      size++;
    }
  }

  for(int i = 0; i < _paramsSize; i++)
  {
    size += strlen(_params[i].name);
    size += strlen(_params[i].value);
    size += 5;
    if(i + 1 < _paramsSize)
    {
      size++;
    }
  }

  /* send content in json format */
  _server.client().print(F("HTTP/1.0 200 OK\r\n"));
  _server.client().print(F("Content-Type: text/html\r\n"));
  _server.client().print(F("Content-Length: "));
  _server.client().print(size);
  _server.client().print(F("\r\n\r\n"));

  _server.client().print("{");

  for(int i = 0; i < WEB_PARAM_SIZE; i++)
  {
    _server.client().print("\"");
    _server.client().print(_systemParams[i].name);
    _server.client().print("\":\"");
    _server.client().print(_systemParams[i].value);
    _server.client().print("\"");
    if(_paramsSize > 0 || i + 1 < WEB_PARAM_SIZE)
    {
      _server.client().print(",");
    }
  }

  for(int i = 0; i < _paramsSize; i++)
  {
    _server.client().print("\"");
    _server.client().print(_params[i].name);
    _server.client().print("\":\"");
    _server.client().print(_params[i].value);
    _server.client().print("\"");
    if(i + 1 < _paramsSize)
    {
      _server.client().print(",");
    }
  }
  _server.client().print("}");
}

void WebConfig::receiveConfig(void)
{
  int notFoundCounter = 0;
  for (int ia = 0; ia < _server.args(); ia++)
  {
    bool found = false;
    for(int i = 0; i < WEB_PARAM_SIZE && found == false; i++)
    {
      if(strcmp(_server.argName(ia).c_str(), _systemParams[i].name) == 0)
      {
        strncpy(_systemParams[i].value, _server.arg(ia).c_str(), MAX_SYSTEM_VALUE_SIZE);
        found = true;
      }
    }
    for(int i = 0; i < _paramsSize && found == false; i++)
    {
      if(strcmp(_server.argName(ia).c_str(), _params[i].name) == 0)
      {
        strncpy(_params[i].value, _server.arg(ia).c_str(), MAX_VALUE_SIZE);
        found = true;
      }
    }
    if(!found) notFoundCounter++;

    if(_debug)
    {
      String message;
      message += _server.argName(ia) + ": ";
      message += _server.arg(ia) + "\n";
      Serial.print(message);
    }
  }

  if(notFoundCounter > 0)
  {
    _server.send(400, F("text/plain"), F("Parameter not found in configuration"));
  }
  else if(saveConfig() == 0)
  {
    _server.send(200, F("text/html"), _successWebPage);
    _configChanged = PARAM_CHANGED;
  }
  else
  {
    _server.send(400, F("text/plain"), F("Cannot save configuration"));
  }
}

void WebConfig::updateWifi(void)
{
  WiFi.softAP(getParameter(PARAM_NAME_AP_SSID), getParameter(PARAM_NAME_AP_PSWD), 11, false, 1);
  WiFi.begin(getParameter(PARAM_NAME_CL_SSID), getParameter(PARAM_NAME_CL_PSWD));
}

bool WebConfig::isWifiConnected(void)
{
  return WiFi.status() == WL_CONNECTED;
}
