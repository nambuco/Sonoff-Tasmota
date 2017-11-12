/*
  thermostat.ino - sonoff TH hard thermostat support for Sonoff-Tasmota

  Copyright (C) 2017  Theo Arends

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*********************************************************************************************\
 * Hard Thermostat 
 *
 * Colaboration of nambuco / Paulo H F Alves
\*********************************************************************************************/


enum ThermoCommands {
    CMND_THERMOSTAT, CMND_SETPOINT };
const char kThermoCommands[] PROGMEM =
    D_CMND_THERMOSTAT "|" D_CMND_SETPOINT ;
  
/*********************************************************************************************\
 * Commands
\*********************************************************************************************/

boolean ThermoCommand(char *type, uint16_t index, char *dataBuf, uint16_t data_len, int16_t payload)
{
  char command [CMDSZ];
  char sunit[CMDSZ];
  boolean serviced = true;
  uint8_t status_flag = 0;
 // uint8_t unit = 0;
//  unsigned long nvalue = 0;

  int command_code = GetCommandCode(command, sizeof(command), type, kThermoCommands);
  if (CMND_THERMOSTAT == command_code) {
//    if ((payload >= 0) && (payload < 3601)) {
//     Settings.hlw_pmin = payload;
//    }
 //   nvalue = Settings.hlw_pmin;
     snprintf_P(log_data, sizeof(log_data), PSTR("Thermostat Comand %d"), payload);
     AddLog(LOG_LEVEL_DEBUG);
     snprintf_P(mqtt_data, sizeof(mqtt_data), S_JSON_COMMAND_NVALUE, command, payload);
    }
  else if (CMND_SETPOINT == command_code) {
    snprintf_P(log_data, sizeof(log_data), PSTR("Setpoint Comand %d"), payload);
    AddLog(LOG_LEVEL_DEBUG);
      if ((payload >= -50) && (payload < +50)) {
        Settings.thermo_setpoint = payload;
    }
    snprintf_P(mqtt_data, sizeof(mqtt_data), S_JSON_COMMAND_NVALUE, command,  Settings.thermo_setpoint);
//    nvalue = Settings.hlw_pmax;
  }
  else {
    serviced = false;
  }
//  if (!status_flag) {
//    if (Settings.flag.value_units) {
//      snprintf_P(mqtt_data, sizeof(mqtt_data), S_JSON_COMMAND_NVALUE_SPACE_UNIT, command, nvalue, GetTextIndexed(sunit, sizeof(sunit), unit, kUnitNames));
//    } else {
//      snprintf_P(mqtt_data, sizeof(mqtt_data), S_JSON_COMMAND_NVALUE, command, nvalue);
//    }
//  }
  return serviced;
}

// Hard Thermostat main function
byte teste;
void ThermoFunction (int tele_period)
{
#ifdef USE_DS18B20

  teste ++;
  snprintf_P(log_data, sizeof(log_data), PSTR("Hard Thermostat Function %d"), teste);
  AddLog(LOG_LEVEL_DEBUG);  
#endif  // USE_DS18B20
}
