#pragma once

/*
    Part of Grbl_ESP32
    2021 -  Stefan de Bruijn, Mitch Bradley

    Grbl_ESP32 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Grbl_ESP32 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Grbl_ESP32.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "../Configuration/Configurable.h"
#include <IPAddress.h>

namespace Machine {
    class WifiConfig : public Configuration::Configurable {
    public:
        WifiConfig() : _ipAddress(IPAddress(10, 0, 0, 1)), _gateway(IPAddress(10, 0, 0, 1)), _netmask(IPAddress(255, 255, 0, 0)) {}
        
        Setting<IPAddress> _ipAddress;
        Setting<IPAddress> _gateway;
        Setting<IPAddress> _netmask;
        Setting<String>    _ssid = "GRBL_ESP";

        // Passwords don't belong in a YAML!
        // String _password = "12345678";

        Setting<bool> _dhcp = true;

        void group(Configuration::HandlerBase& handler) override {
            handler.item("ssid", _ssid);
            // handler.item("password", _password);

            handler.item("ip_address", _ipAddress);
            handler.item("gateway", _gateway);
            handler.item("netmask", _netmask);

            handler.item("dhcp", _dhcp);
        }
    };
}
