/*
  WifiServices.cpp -  wifi services functions class

  Copyright (c) 2014 Luc Lebosse. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "WifiServices.h"

#include "../Machine/MachineConfig.h"

#ifdef ENABLE_WIFI

#    include "WebServer.h"
#    include "TelnetServer.h"
#    include "NotificationsService.h"
#    include "Commands.h"
#    include "../Report.h"  // info_all

#    include <WiFi.h>
#    include <FS.h>
#    include <SPIFFS.h>
#    include "WifiServices.h"
#    include <ESPmDNS.h>
#    include <ArduinoOTA.h>

namespace WebUI {
    WiFiServices wifi_services;

    WiFiServices::WiFiServices() {}
    WiFiServices::~WiFiServices() { end(); }

    bool WiFiServices::begin() {
        bool no_error = true;
        //Sanity check
        if (!(config->_comms->_staConfig || config->_comms->_apConfig)) {
            return false;
        }
        if (WiFi.getMode() == WIFI_OFF) {
            return false;
        }

        String& h = config->_comms->_hostname.get();

        ArduinoOTA
            .onStart([]() {
                String type;
                if (ArduinoOTA.getCommand() == U_FLASH) {
                    type = "sketch";
                } else {  // U_SPIFFS
                    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
                    type = "filesystem";
                    SPIFFS.end();
                }
                info_all("Start OTA updating %s", type.c_str());
            })
            .onEnd([]() { info_all("End OTA"); })
            .onProgress([](unsigned int progress, unsigned int total) { info_all("OTA Progress: %u%%", (progress / (total / 100))); })
            .onError([](ota_error_t error) {
                info_all("OTA Error(%u):", error);
                switch (error) {
                    case OTA_AUTH_ERROR:
                        info_all("Auth Failed");
                        break;
                    case OTA_BEGIN_ERROR:
                        info_all("Begin Failed");
                        break;
                    case OTA_CONNECT_ERROR:
                        info_all("Connect Failed");
                        break;
                    case OTA_RECEIVE_ERROR:
                        info_all("Receive Failed");
                        break;
                    case OTA_END_ERROR:
                        info_all("End Failed");
                        break;
                }
            });
        ArduinoOTA.begin();
        //no need in AP mode
        if (WiFi.getMode() == WIFI_STA) {
            //start mDns
            if (!MDNS.begin(h.c_str())) {
                info_all("Cannot start mDNS");
                no_error = false;
            } else {
                info_all("Start mDNS with hostname:http://%s.local/", h.c_str());
            }
        }
        web_server.begin();
        telnet_server.begin();
        notificationsservice.begin();

        //be sure we are not is mixed mode in setup
        WiFi.scanNetworks(true);
        return no_error;
    }
    void WiFiServices::end() {
        notificationsservice.end();
        telnet_server.end();
        web_server.end();

        //stop OTA
        ArduinoOTA.end();

        //Stop mDNS
        MDNS.end();
    }

    void WiFiServices::handle() {
        COMMANDS::wait(0);
        //to avoid mixed mode due to scan network
        if (WiFi.getMode() == WIFI_AP_STA) {
            // In principle it should be sufficient to check for != WIFI_SCAN_RUNNING,
            // but that does not work well.  Doing so makes scans in AP mode unreliable.
            // Sometimes the first try works, but subsequent scans fail.
            if (WiFi.scanComplete() >= 0) {
                WiFi.enableSTA(false);
            }
        }
        ArduinoOTA.handle();
        web_server.handle();
        telnet_server.handle();
    }
}
#endif
