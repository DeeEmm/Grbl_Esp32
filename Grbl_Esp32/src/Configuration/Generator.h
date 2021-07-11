/*
    Part of Grbl_ESP32
    2021 -  Stefan de Bruijn

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

#pragma once

#include <vector>

#include "../Pin.h"
#include "../StringRange.h"
#include "../StringStream.h"
#include "HandlerBase.h"

namespace Configuration {
    class Configurable;

    class Generator : public HandlerBase {
        Generator(const Generator&) = delete;
        Generator& operator=(const Generator&) = delete;

        int                 indent_;
        SimpleOutputStream& dst_;
        bool                lastIsNewline_ = false;

        inline void indent() {
            lastIsNewline_ = false;
            for (int i = 0; i < indent_ * 2; ++i) {
                dst_ << ' ';
            }
        }

        void enter(const char* name);
        void add(Configuration::Configurable* configurable);
        void leave();

    protected:
        void        enterSection(const char* name, Configurable* value) override;
        bool        matchesUninitialized(const char* name) override { return false; }
        HandlerType handlerType() override { return HandlerType::Generator; }

    public:
        Generator(SimpleOutputStream& dst);

        void item(const char* name, Setting<int>& value, int32_t minValue, int32_t maxValue) override {
            indent();
            dst_ << name << ": " << value << '\n';
        }

        void item(const char* name, Setting<float>& value, float minValue, float maxValue) override {
            indent();
            dst_ << name << ": " << value << '\n';
        }

        void item(const char* name, Setting<std::vector<speedEntry>>& value) {
            indent();
            dst_ << name << ": ";
            const std::vector<speedEntry>& vec = value;
            if (vec.size() == 0) {
                dst_ << "None";
            } else {
                const char* separator = "";
                for (speedEntry n : vec) {
                    dst_ << separator << n.speed << '=' << n.percent << '%';
                    separator = " ";
                }
            }
            dst_ << '\n';
        }

        void item(const char* name, Setting<UartData>& wordLength, Setting<UartParity>& parity, Setting<UartStop>& stopBits) override {
            indent();

            UartData wl = wordLength;
            dst_ << name << ": " << (int(wl) - int(UartData::Bits5) + 5);
            switch (parity) {
                case UartParity::Even:
                    dst_ << 'E';
                    break;
                case UartParity::Odd:
                    dst_ << 'O';
                    break;
                case UartParity::None:
                    dst_ << 'N';
                    break;
            }
            switch (stopBits) {
                case UartStop::Bits1:
                    dst_ << '1';
                    break;
                case UartStop::Bits1_5:
                    dst_ << "1.5";
                    break;
                case UartStop::Bits2:
                    dst_ << '2';
                    break;
            }
            dst_ << '\n';
        }

        void item(const char* name, Setting<StringRange>& value, int minLength, int maxLength) override {
            indent();
            dst_ << name << ": " << value << '\n';
        }

        void item(const char* name, Setting<bool>& value) override {
            indent();
            const char* bval = value ? "true" : "false";
            dst_ << name << ": " << bval << '\n';
        }

        void item(const char* name, Setting<Pin>& value) override {
            indent();
            dst_ << name << ": " << value << '\n';
        }
        void item(const char* name, Setting<IPAddress>& value) override {
            indent();
            const IPAddress& ip = value;
            dst_ << name << ": " << ip.toString() << '\n';
        }
        void item(const char* name, Setting<int>& value, EnumItem* e) override {
            indent();
            const char* str = "unknown";
            for (; e->name; ++e) {
                if (value == e->value) {
                    str = e->name;
                    break;
                }
            }
            dst_ << name << ": " << str << '\n';
        }
    };
}
