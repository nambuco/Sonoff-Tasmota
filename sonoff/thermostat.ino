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

enum ThermoStates {
    STATE_ON, STATE_OFF_DELAY, STATE_OFF, STATE_DISABLED};

byte thermo_state = STATE_DISABLED;
uint8 thermo_timer = 0;

#define OFF_DELAY 90  // minimal off time in seconds
#define HYSTERESIS 2  // Hysteresys in 0.1 Degrees

  
/*********************************************************************************************\
 * Commands
\*********************************************************************************************/

boolean ThermoCommand(char *type, uint16_t index, char *dataBuf, uint16_t data_len, int16_t payload)
{
  char command [CMDSZ];
  char sunit[CMDSZ];
  boolean serviced = true;

  int command_code = GetCommandCode(command, sizeof(command), type, kThermoCommands);
  if (CMND_THERMOSTAT == command_code) {
    snprintf_P(log_data, sizeof(log_data), PSTR("Thermostat Comand %d"), payload);
    AddLog(LOG_LEVEL_DEBUG);
    if ((payload == 0) || (payload == 1)) {
      Settings.thermo = payload;
      snprintf_P(mqtt_data, sizeof(mqtt_data), S_JSON_COMMAND_NVALUE, command, Settings.thermo);
    }
  }
  else if (CMND_SETPOINT == command_code) {
    snprintf_P(log_data, sizeof(log_data), PSTR("Setpoint Comand %d"), payload);
    AddLog(LOG_LEVEL_DEBUG);
      if ((payload >= -500) && (payload < +500)) {
        Settings.thermo_setpoint = payload;
      }
    snprintf_P(mqtt_data, sizeof(mqtt_data), S_JSON_COMMAND_NVALUE, command,  Settings.thermo_setpoint);
  }
  else {
    serviced = false;
  }
  return serviced;
}

// Hard Thermostat main function
void ThermoFunction (int tele_period)
{
#ifdef USE_DS18B20
  float temp;
  thermo_timer ++;
  if ((pin[GPIO_DSB] < 99) && (tele_period == 0)) {
    if (Ds18b20ReadTemperature(temp)) {
//      snprintf_P(log_data, sizeof(log_data), PSTR("Hard Thermostat Function %d | State %d | Temp %s"), Settings.thermo_setpoint, thermo_state, String(temp * 10).c_str());
//      AddLog(LOG_LEVEL_DEBUG);
      if (thermo_state == STATE_ON) {
        if (Settings.thermo == 0) {
          ExecuteCommandPower(1, 0);
          thermo_state = STATE_DISABLED;
        }
        else if ((temp * 10) < (Settings.thermo_setpoint)) {
          ExecuteCommandPower(1,0);
          thermo_timer = 0;
          thermo_state = STATE_OFF_DELAY;
        }
        else {
          ExecuteCommandPower(1,1);
        }
      }
      else if (thermo_state == STATE_OFF) {
        if (Settings.thermo == 0) {
          ExecuteCommandPower(1, 0);
          thermo_state = STATE_DISABLED;
        }
        else if ((temp * 10) > (Settings.thermo_setpoint + HYSTERESIS)) {
          ExecuteCommandPower(1,1);
          thermo_state = STATE_ON;
        }
        else {
          ExecuteCommandPower(1,0);
        }
      }
      else if (thermo_state == STATE_OFF_DELAY) {
        if (thermo_timer > OFF_DELAY) {
        thermo_state = STATE_OFF;
        thermo_timer = 0;
        }
      }
      else if (thermo_state == STATE_DISABLED) {
        if (Settings.thermo == 1) {
          ExecuteCommandPower(1, 0);
          thermo_state = STATE_OFF;
        }
      }
    }
  }
#endif  // USE_DS18B20
}
