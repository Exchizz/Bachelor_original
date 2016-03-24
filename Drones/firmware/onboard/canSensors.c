/*
    This file is part of AutoQuad.

    AutoQuad is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AutoQuad is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with AutoQuad.  If not, see <http://www.gnu.org/licenses/>.

    Copyright © 2011-2014  Bill Nesbitt
*/

#include "canSensors.h"
#include "aq_timer.h"
#include <CoOS.h>
#include "gps.h"
canSensorsStruct_t canSensorsData;

void canSensorsReceiveTelem(uint8_t canId, uint8_t doc, void *data) {

    double data_double;
    memcpy(&data_double,data,8);

    // Mathias
    if(canId == CAN_SENSORS_GPS_LAT){
      switch(doc){
      case 0x01:
        gpsData.lat = data_double;
        debug_printf("got lat\t");
        break;
       case 0x02:
        gpsData.lon = data_double;
        debug_printf("lon\t");
        break;
       case 0x03:
        gpsData.lastPosUpdate = timerMicros();
        gpsData.height = data_double;
        gpsData.hAcc = 0;
        gpsData.vAcc = 0;
        debug_printf("height\n");
        CoSetFlag(gpsData.gpsPosFromCanFlag);
        break;
      }
    }
    /*
            gpsData.lastPosUpdate = timerMicros();
        gpsData.lat = *(float *)(data+4);
        gpsData.lon = *(float *)(data); // Add 4 because void is 1 byte, and we want next 4 bytes.
        gpsData.height = 30;
        gpsData.hAcc = 0;
        gpsData.vAcc = 0;
        CoSetFlag(gpsData.gpsPosFromCanFlag);
        debug_printf("DOC: %X\n", doc);
    */
    // End Mathias
    canSensorsData.values[canId] = *(float *)data;

    // record reception time
    canSensorsData.rcvTimes[canId] = timerMicros();
}

void canSensorsInit(void) {
    int i;

    for (i = 0; i < CAN_SENSORS_NUM; i++) {
        if ((canSensorsData.nodes[i] = canFindNode(CAN_TYPE_SENSOR, i)) != 0) {
            canTelemRegister(canSensorsReceiveTelem, CAN_TYPE_SENSOR);

            // request telemetry
            canSetTelemetryValue(CAN_TT_NODE, canSensorsData.nodes[i]->networkId, 0, CAN_TELEM_VALUE);
            canSetTelemetryRate(CAN_TT_NODE, canSensorsData.nodes[i]->networkId, CAN_SENSORS_RATE);
        }
    }
}