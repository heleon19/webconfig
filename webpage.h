#ifndef webpage_h
#define webpage_h

#include <Arduino.h>

PROGMEM const char _mainWebPage[] = ""
"<!DOCTYPE html>\n"
"<html>\n"
  "<head>\n"
    "<meta charset='UTF-8'>\n"
    "<title>ESP8266 Web Configurtor</title>\n"
    "<link rel='stylesheet' type='text/css' href='default.css'>\n"
    "<link rel='stylesheet' type='text/css' href='custom.css'>\n"
    "<script src='custom.js'></script>\n"
  "</head>\n"
  "<body>\n"
    "<h1 id='title'>ESP8266 Web Configurator</h1>\n"
    "<div id='form'></div>\n"
    "<script>\n"
      "document.addEventListener('DOMContentLoaded', function(event){\n"
        "var xmlHttp = new XMLHttpRequest();\n"
        "xmlHttp.onreadystatechange = function() {\n"
          "if (xmlHttp.readyState == 4 && xmlHttp.status == 200){\n"
            "var json = JSON.parse(xmlHttp.responseText);\n"
            "var f = document.createElement('form');\n"
            "f.setAttribute('method','post');\n"
            "f.setAttribute('action','store');\n"
            "Object.keys(json).forEach(function(key){\n"
              "var l = document.createElement('label');\n"
              "l.id = 'lbl-' + key;\n"
              "l.for = key;\n"
              "l.innerHTML = key.toUpperCase();\n"
              "f.appendChild(l);\n"
              "var i = document.createElement('input');\n"
              "i.type = 'text';\n"
              "i.name = key;\n"
              "i.value = json[key];\n"
              "i.id = key;\n"
              "f.appendChild(i);\n"
              "var br = document.createElement('br');\n"
              "f.appendChild(br);\n"
            "});\n"
            "var s = document.createElement('input');\n"
            "s.type = 'submit'\n"
            "s.value = 'Save Configuration'\n"
            "f.appendChild(s);\n"
            "document.getElementById('form').appendChild(f);\n"
          "}\n"
        "}\n"
        "xmlHttp.open('GET', 'config.json', true);\n"
        "xmlHttp.send(null);\n"
      "});\n"
    "</script>\n"
  "</body>\n"
"</html>\n";

PROGMEM const char _successWebPage[] = ""
"<!DOCTYPE html>\n"
"<html>\n"
  "<head>\n"
    "<meta charset='UTF-8'>\n"
    "<title>ESP8266 Web Configurtor</title>\n"
    "<link rel='stylesheet' type='text/css' href='default.css'>\n"
    "<link rel='stylesheet' type='text/css' href='custom.css'>\n"
    "<script src='custom.js'></script>\n"
  "</head>\n"
  "<body>\n"
    "<h1 id='title'>ESP8266 Web Configurator</h1>\n"
    "<p>Configuration successful saved!</p>\n"
    "<p><a href='/'>Back</a></p>\n"
  "</body>\n"
"</html>\n";

PROGMEM const char _defaultCss[] = ""
"body {\n"
  "font-family: Arial, Helvetica, sans-serif;\n"
  "text-align: center;\n"
"}\n"
"label {\n"
  "display: inline-block;\n"
  "width: 200px;\n"
  "margin-top: 10px;\n"
  "text-align: left;\n"
"}\n"
"input[type='submit'] {\n"
  "margin-top: 20px;\n"
"}\n";

#endif
