/*
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

/* Protocol implementation was provided by FETtec */

#include <AP_SerialManager/AP_SerialManager.h>
#include <AP_Math/AP_Math.h>
#include <GCS_MAVLink/GCS_MAVLink.h>
#include <GCS_MAVLink/GCS.h>
#include <AP_Logger/AP_Logger.h>

#include "AP_FETtecOneWire.h"
#if HAL_AP_FETTECONEWIRE_ENABLED
#include <stdio.h>

const AP_Param::GroupInfo AP_FETtecOneWire::var_info[] = {
    // @Param: MASK
    // @DisplayName: Channel Bitmask
    // @Description: Enable of FETtec OneWire ESC protocol to specific channels
    // @Bitmask: 0:Channel1,1:Channel2,2:Channel3,3:Channel4,4:Channel5,5:Channel6,6:Channel7,7:Channel8,8:Channel9,9:Channel10,10:Channel11,11:Channel12,12:Channel13,13:Channel14,14:Channel15,15:Channel16
    // @User: Standard
    AP_GROUPINFO("MASK",  1, AP_FETtecOneWire, motor_mask, 0),

    AP_GROUPEND
};

AP_FETtecOneWire *AP_FETtecOneWire::_singleton;

static uint8_t FETtecOneWire_ResponseLength[54];
static uint8_t FETtecOneWire_RequestLength[54];

AP_FETtecOneWire::AP_FETtecOneWire()
{
    _singleton = this;
    FETtecOneWire_ResponseLength[OW_OK] = 1;
    FETtecOneWire_ResponseLength[OW_BL_PAGE_CORRECT] = 1;   // BL only
    FETtecOneWire_ResponseLength[OW_NOT_OK] = 1;
    FETtecOneWire_ResponseLength[OW_BL_START_FW] = 0;       // BL only
    FETtecOneWire_ResponseLength[OW_BL_PAGES_TO_FLASH] = 1; // BL only
    FETtecOneWire_ResponseLength[OW_REQ_TYPE] = 1;
    FETtecOneWire_ResponseLength[OW_REQ_SN] = 12;
    FETtecOneWire_ResponseLength[OW_REQ_SW_VER] = 2;
    FETtecOneWire_ResponseLength[OW_RESET_TO_BL] = 0;
    FETtecOneWire_ResponseLength[OW_THROTTLE] = 1;
    FETtecOneWire_ResponseLength[OW_REQ_TLM] = 2;
    FETtecOneWire_ResponseLength[OW_BEEP] = 0;

    FETtecOneWire_ResponseLength[OW_SET_FAST_COM_LENGTH] = 1;

    FETtecOneWire_ResponseLength[OW_GET_ROTATION_DIRECTION] = 1;
    FETtecOneWire_ResponseLength[OW_SET_ROTATION_DIRECTION] = 1;

    FETtecOneWire_ResponseLength[OW_GET_USE_SIN_START] = 1;
    FETtecOneWire_ResponseLength[OW_SET_USE_SIN_START] = 1;

    FETtecOneWire_ResponseLength[OW_GET_3D_MODE] = 1;
    FETtecOneWire_ResponseLength[OW_SET_3D_MODE] = 1;

    FETtecOneWire_ResponseLength[OW_GET_ID] = 1;
    FETtecOneWire_ResponseLength[OW_SET_ID] = 1;

/*
    FETtecOneWire_ResponseLength[OW_GET_LINEAR_THRUST] = 1;
    FETtecOneWire_ResponseLength[OW_SET_LINEAR_THRUST] = 1;
*/

    FETtecOneWire_ResponseLength[OW_GET_EEVER] = 1;

    FETtecOneWire_ResponseLength[OW_GET_PWM_MIN] = 2;
    FETtecOneWire_ResponseLength[OW_SET_PWM_MIN] = 1;

    FETtecOneWire_ResponseLength[OW_GET_PWM_MAX] = 2;
    FETtecOneWire_ResponseLength[OW_SET_PWM_MAX] = 1;

    FETtecOneWire_ResponseLength[OW_GET_ESC_BEEP] = 1;
    FETtecOneWire_ResponseLength[OW_SET_ESC_BEEP] = 1;

    FETtecOneWire_ResponseLength[OW_GET_CURRENT_CALIB] = 1;
    FETtecOneWire_ResponseLength[OW_SET_CURRENT_CALIB] = 1;

    FETtecOneWire_ResponseLength[OW_SET_LED_TMP_COLOR] = 0;
    FETtecOneWire_ResponseLength[OW_GET_LED_COLOR] = 5;
    FETtecOneWire_ResponseLength[OW_SET_LED_COLOR] = 1;

    FETtecOneWire_RequestLength[OW_OK] = 1;
    FETtecOneWire_RequestLength[OW_BL_PAGE_CORRECT] = 1; // BL only
    FETtecOneWire_RequestLength[OW_NOT_OK] = 1;
    FETtecOneWire_RequestLength[OW_BL_START_FW] = 1;       // BL only
    FETtecOneWire_RequestLength[OW_BL_PAGES_TO_FLASH] = 1; // BL only
    FETtecOneWire_RequestLength[OW_REQ_TYPE] = 1;
    FETtecOneWire_RequestLength[OW_REQ_SN] = 1;
    FETtecOneWire_RequestLength[OW_REQ_SW_VER] = 1;
    FETtecOneWire_RequestLength[OW_RESET_TO_BL] = 1;
    FETtecOneWire_RequestLength[OW_THROTTLE] = 1;
    FETtecOneWire_RequestLength[OW_REQ_TLM] = 1;
    FETtecOneWire_RequestLength[OW_BEEP] = 2;

    FETtecOneWire_RequestLength[OW_SET_FAST_COM_LENGTH] = 4;

    FETtecOneWire_RequestLength[OW_GET_ROTATION_DIRECTION] = 1;
    FETtecOneWire_RequestLength[OW_SET_ROTATION_DIRECTION] = 1;

    FETtecOneWire_RequestLength[OW_GET_USE_SIN_START] = 1;
    FETtecOneWire_RequestLength[OW_SET_USE_SIN_START] = 1;

    FETtecOneWire_RequestLength[OW_GET_3D_MODE] = 1;
    FETtecOneWire_RequestLength[OW_SET_3D_MODE] = 1;

    FETtecOneWire_RequestLength[OW_GET_ID] = 1;
    FETtecOneWire_RequestLength[OW_SET_ID] = 1;

/*
    FETtecOneWire_RequestLength[OW_GET_LINEAR_THRUST] = 1;
    FETtecOneWire_RequestLength[OW_SET_LINEAR_THRUST] = 1;
*/

    FETtecOneWire_RequestLength[OW_GET_EEVER] = 1;

    FETtecOneWire_RequestLength[OW_GET_PWM_MIN] = 1;
    FETtecOneWire_RequestLength[OW_SET_PWM_MIN] = 2;

    FETtecOneWire_RequestLength[OW_GET_PWM_MAX] = 1;
    FETtecOneWire_RequestLength[OW_SET_PWM_MAX] = 2;

    FETtecOneWire_RequestLength[OW_GET_ESC_BEEP] = 1;
    FETtecOneWire_RequestLength[OW_SET_ESC_BEEP] = 1;

    FETtecOneWire_RequestLength[OW_GET_CURRENT_CALIB] = 1;
    FETtecOneWire_RequestLength[OW_SET_CURRENT_CALIB] = 1;

    FETtecOneWire_RequestLength[OW_SET_LED_TMP_COLOR] = 4;
    FETtecOneWire_RequestLength[OW_GET_LED_COLOR] = 1;
    FETtecOneWire_RequestLength[OW_SET_LED_COLOR] = 5;

}

void AP_FETtecOneWire::init()
{
    AP_SerialManager& serial_manager = AP::serialmanager();
    _uart = serial_manager.find_serial(AP_SerialManager::SerialProtocol_FETtechOneWire, 0);
    if (_uart) {
        _uart->begin(2000000);
    }
    Init();
}

void AP_FETtecOneWire::update()
{
    if (!initialised) {
        initialised = true;
        init();
        last_send_us = AP_HAL::micros();
        return;
    }

    if (_uart == nullptr) {
        return;
    }

    const uint16_t mask = uint16_t(motor_mask.get());

    // tell SRV_Channels about ESC capabilities
    SRV_Channels::set_digital_mask(mask);
    for (uint8_t i = 0; i < MOTOR_COUNT_MAX; i++) {
        SRV_Channel* c = SRV_Channels::srv_channel(i);
        if (c == nullptr) {
            continue;
        }
        motorpwm[i] = c->get_output_pwm();
    }

    uint16_t requestedTelemetry[MOTOR_COUNT_MAX] = {0};
    TelemetryAvailable = ESCsSetValues(motorpwm, requestedTelemetry, MOTOR_COUNT_MAX, TLM_request);
    if (TelemetryAvailable != -1) {
        for (uint8_t i = 0; i < MOTOR_COUNT_MAX; i++) {
            completeTelemetry[i][TelemetryAvailable] = requestedTelemetry[i];
            completeTelemetry[i][5]++;
        }
    }
    if (++TLM_request == 5) {
        TLM_request = 0;
    }

    if (TelemetryAvailable != -1) {
        AP_Logger *logger = AP_Logger::get_singleton();
        const uint32_t now = AP_HAL::millis();
        // log at 10Hz
        if (logger && logger->logging_enabled() && now - last_log_ms > 100) {
            for (uint8_t i = 0; i < MOTOR_COUNT_MAX; i++) {
                printf(" esc: %d", i + 1);
                printf(" Temperature: %d", completeTelemetry[i][0]);
                printf(", Voltage: %d", completeTelemetry[i][1]);
                printf(", Current: %d", completeTelemetry[i][2]);
                printf(", E-rpm: %d", completeTelemetry[i][3]);
                printf(", consumption: %d", completeTelemetry[i][4]);
                printf("\n");

                logger->Write_ESC(i,
                        AP_HAL::micros64(),
                        completeTelemetry[i][3] * 100U,
                        completeTelemetry[i][1],
                        completeTelemetry[i][2],
                        completeTelemetry[i][0] * 100U,
                        completeTelemetry[i][4],
                        0,
                        0);
            }
            last_log_ms = now;
        }
    }

}

/*
  send ESC telemetry messages over MAVLink
  @param mav_chan mavlink channel
 */
void AP_FETtecOneWire::send_esc_telemetry_mavlink(uint8_t mav_chan)
{
    if (TelemetryAvailable == -1) {
        return;
    }
    uint8_t temperature[4] {};
    uint16_t voltage[4] {};
    uint16_t current[4] {};
    uint16_t totalcurrent[4] {};
    uint16_t rpm[4] {};
    uint16_t count[4] {};
    for (uint8_t i=0; i<MOTOR_COUNT_MAX; i++) {
        uint8_t idx = i % 4;
        temperature[idx] = completeTelemetry[i][0];
        voltage[idx] = completeTelemetry[i][1];
        current[idx] = completeTelemetry[i][2];
        rpm[idx] = completeTelemetry[i][3];
        totalcurrent[idx] = completeTelemetry[i][4];
        count[idx] = completeTelemetry[i][5];
        if (idx == 3 || i == MOTOR_COUNT_MAX - 1) {
            if (!HAVE_PAYLOAD_SPACE((mavlink_channel_t)mav_chan, ESC_TELEMETRY_1_TO_4)) {
                return;
            }
            if (i < 4) {
                mavlink_msg_esc_telemetry_1_to_4_send((mavlink_channel_t)mav_chan, temperature, voltage, current, totalcurrent, rpm, count);
            } else if (i < 8) {
                mavlink_msg_esc_telemetry_5_to_8_send((mavlink_channel_t)mav_chan, temperature, voltage, current, totalcurrent, rpm, count);
            } else {
                mavlink_msg_esc_telemetry_9_to_12_send((mavlink_channel_t)mav_chan, temperature, voltage, current, totalcurrent, rpm, count);
            }
        }
    }
}

/*
    initialize FETtecOneWire protocol
*/
void AP_FETtecOneWire::Init()
{
    if (_firstInitDone == 0) {
        _FoundESCs = 0;
        _ScanActive = 0;
        _SetupActive = 0;
        _minID = MAX_SUPPORTED_CH;
        _maxID = 0;
        _IDcount = 0;
        _FastThrottleByteCount = 0;
        for (uint8_t i = 0; i < MAX_SUPPORTED_CH; i++) {
            _activeESC_IDs[i] = 0;
        }
    }
    _IgnoreOwnBytes = 0;
    _PullSuccess = 0;
    _PullBusy = 0;
    _firstInitDone = 1;
}

/*
    generates used 8 bit CRC
    @param crc byte to be added to CRC
    @param crc_seed CRC where it gets added too
    @return 8 bit CRC
*/
uint8_t AP_FETtecOneWire::UpdateCrc8(uint8_t crc, uint8_t crc_seed)
{
    uint8_t crc_u, i;
    crc_u = crc;
    crc_u ^= crc_seed;
    for (i = 0; i < 8; i++) {
        crc_u = (crc_u & 0x80) ? 0x7 ^ (crc_u << 1) : (crc_u << 1);
    }
    return (crc_u);
}

/*
    generates used 8 bit CRC for arrays
    @param Buf 8 bit byte array
    @param BufLen count of bytes that should be used for CRC calculation
    @return 8 bit CRC
*/
uint8_t AP_FETtecOneWire::GetCrc8(uint8_t* Buf, uint16_t BufLen)
{
    uint8_t crc = 0;
    for (uint16_t i = 0; i < BufLen; i++) {
        crc = UpdateCrc8(Buf[i], crc);
    }
    return (crc);
}

/*
    transmitts a FETtecOneWire frame to a ESC
    @param ESC_id id of the ESC
    @param Bytes 8 bit array of bytes. Where byte 1 contains the command, and all following bytes can be the payload
    @param Length length of the Bytes array
*/
void AP_FETtecOneWire::Transmit(uint8_t ESC_id, uint8_t* Bytes, uint8_t Length)
{
    /*
    a frame looks like:
    byte 1 = frame header (master is always 0x01)
    byte 2 = target ID (5bit)
    byte 3 & 4 = frame type (always 0x00, 0x00 used for bootloader. here just for compatibility)
    byte 5 = frame length over all bytes
    byte 6 - X = request type, followed by the payload
    byte X+1 = 8bit CRC
    */
    uint8_t transmitArr[256] = {0x01, ESC_id, 0x00, 0x00};
    transmitArr[4] = Length + 6;
    for (uint8_t i = 0; i < Length; i++) {
        transmitArr[i + 5] = Bytes[i];
    }
    transmitArr[Length + 5] = GetCrc8(transmitArr, Length + 5); // crc
    _uart->write(transmitArr, Length + 6);
    _IgnoreOwnBytes += Length + 6;
}

/*
    reads the answer frame of a ESC
    @param Bytes 8 bit byte array, where the received answer gets stored in
    @param Length the expected answer length
    @param returnFullFrame can be OW_RETURN_RESPONSE or OW_RETURN_FULL_FRAME
    @return 1 if the expected answer frame was there, 0 if dont
*/
uint8_t AP_FETtecOneWire::Receive(uint8_t* Bytes, uint8_t Length, uint8_t returnFullFrame)
{
    /*
    a frame looks like:
    byte 1 = frame header (0x02 = bootloader, 0x03 = ESC firmware)
    byte 2 = sender ID (5bit)
    byte 3 & 4 = frame type (always 0x00, 0x00 used for bootloader. here just for compatibility)
    byte 5 = frame length over all bytes
    byte 6 - X = answer type, followed by the payload
    byte X+1 = 8bit CRC
    */

    //ignore own bytes
    while (_IgnoreOwnBytes > 0 && _uart->available()) {
        _IgnoreOwnBytes--;
        _uart->read();
    }
    // look for the real answer
    if (_uart->available() >= Length + 6u) {
        // sync to frame starte byte
        uint8_t testFrameStart = 0;
        do {
            testFrameStart = _uart->read();
        }
        while (testFrameStart != 0x02 && testFrameStart != 0x03 && _uart->available());
        // copy message
        if (_uart->available() >= Length + 5u) {
            uint8_t ReceiveBuf[20] = {0};
            ReceiveBuf[0] = testFrameStart;
            for (uint8_t i = 1; i < Length + 6; i++) {
                ReceiveBuf[i] = _uart->read();
            }
            // check CRC
            if (GetCrc8(ReceiveBuf, Length + 5) == ReceiveBuf[Length + 5]) {
                if (!returnFullFrame) {
                    for (uint8_t i = 0; i < Length; i++) {
                        Bytes[i] = ReceiveBuf[5 + i];
                    }
                } else {
                    for (uint8_t i = 0; i < Length + 6; i++) {
                        Bytes[i] = ReceiveBuf[i];
                    }
                }
                return 1;
            } else {
                return 0;
            } // crc missmatch
        } else {
            return 0;
        } // no answer yet
    } else {
        return 0;
    } // no answer yet
}

/*
    makes all connected ESCs beep
    @param beepFreqency a 8 bit value from 0-255. higher make a higher beep
*/
void AP_FETtecOneWire::Beep(uint8_t beepFreqency)
{
    if (_IDcount > 0) {
        uint8_t request[2] = {OW_BEEP, beepFreqency};
        uint8_t spacer[2] = {0, 0};
        for (uint8_t i = _minID; i < _maxID + 1; i++) {
            Transmit(i, request, FETtecOneWire_RequestLength[request[0]]);
            // add two zeros to make sure all ESCs can catch their command as we don't wait for a response here
            _uart->write(spacer, 2);
            _IgnoreOwnBytes += 2;
        }
    }
}

/*
    sets the racewire color for all ESCs
    @param R red brightness
    @param G green brightness
    @param B blue brightness
*/
void AP_FETtecOneWire::RW_LEDcolor(uint8_t R, uint8_t G, uint8_t B)
{
    if (_IDcount > 0) {
        uint8_t request[4] = {OW_SET_LED_TMP_COLOR, R, G, B};
        uint8_t spacer[2] = {0, 0};
        for (uint8_t i = _minID; i < _maxID + 1; i++) {
            Transmit(i, request, FETtecOneWire_RequestLength[request[0]]);
            // add two zeros to make sure all ESCs can catch their command as we don't wait for a response here
            _uart->write(spacer, 2);
            _IgnoreOwnBytes += 2;
        }
    }
}

/*
    Resets a pending pull request
    returns nothing
*/
void AP_FETtecOneWire::PullReset()
{
    _PullSuccess = 0;
    _PullBusy = 0;
}

/*
    Pulls a complete request between for ESC
    @param ESC_id  id of the ESC
    @param command 8bit array containing the command that should be send including the possible payload
    @param response 8bit array where the response will be stored in
    @param returnFullFrame can be OW_RETURN_RESPONSE or OW_RETURN_FULL_FRAME
    @return 1 if the request is completed, 0 if dont
*/
uint8_t AP_FETtecOneWire::PullCommand(uint8_t ESC_id, uint8_t* command, uint8_t* response,
        uint8_t returnFullFrame)
{
    if (!_PullBusy) {
        _PullBusy = 1;
        _PullSuccess = 0;
        Transmit(ESC_id, command, FETtecOneWire_RequestLength[command[0]]);
    } else {
        if (Receive(response, FETtecOneWire_ResponseLength[command[0]], returnFullFrame)) {
            _PullSuccess = 1;
            _PullBusy = 0;
        }
    }
    return _PullSuccess;
}

/*
    scans for ESCs in bus. should be called until _ScanActive >= MAX_SUPPORTED_CH
    returns the currend scanned ID
*/
uint8_t AP_FETtecOneWire::ScanESCs()
{
    static uint16_t delayLoops = 500;
    static uint8_t scanID = 0;
    static uint8_t scanState = 0;
    static uint8_t scanTimeOut = 0;
    uint8_t response[18] = {0};
    uint8_t request[1] = {0};
    if (_ScanActive == 0) {
        delayLoops = 500;
        scanID = 0;
        scanState = 0;
        scanTimeOut = 0;
        return _ScanActive + 1;
    }
    if (delayLoops > 0) {
        delayLoops--;
        return _ScanActive;
    }
    if (scanID < _ScanActive) {
        scanID = _ScanActive;
        scanState = 0;
        scanTimeOut = 0;
    }
    if (scanTimeOut == 3 || scanTimeOut == 6 || scanTimeOut == 9 || scanTimeOut == 12) {
        PullReset();
    }
    if (scanTimeOut < 15) {
        switch (scanState) {
        case 0:request[0] = OW_OK;
            if (PullCommand(scanID, request, response, OW_RETURN_FULL_FRAME)) {
                scanTimeOut = 0;
                _activeESC_IDs[scanID] = 1;
                _FoundESCs++;
                if (response[0] == 0x02) {
                    _foundESCs[scanID].inBootLoader = 1;
                } else {
                    _foundESCs[scanID].inBootLoader = 0;
                }
                delayLoops = 1;
                scanState++;
            } else {
                scanTimeOut++;
            }
            break;
        case 1:request[0] = OW_REQ_TYPE;
            if (PullCommand(scanID, request, response, OW_RETURN_RESPONSE)) {
                scanTimeOut = 0;
                _foundESCs[scanID].ESCtype = response[0];
                delayLoops = 1;
                scanState++;
            } else {
                scanTimeOut++;
            }
            break;
        case 2:request[0] = OW_REQ_SW_VER;
            if (PullCommand(scanID, request, response, OW_RETURN_RESPONSE)) {
                scanTimeOut = 0;
                _foundESCs[scanID].firmWareVersion = response[0];
                _foundESCs[scanID].firmWareSubVersion = response[1];
                delayLoops = 1;
                scanState++;
            } else {
                scanTimeOut++;
            }
            break;
        case 3:request[0] = OW_REQ_SN;
            if (PullCommand(scanID, request, response, OW_RETURN_RESPONSE)) {
                scanTimeOut = 0;
                for (uint8_t i = 0; i < 12; i++) {
                    _foundESCs[scanID].serialNumber[i] = response[i];
                }
                delayLoops = 1;
                return scanID + 1;
            } else {
                scanTimeOut++;
            }
            break;
        }
    } else {
        PullReset();
        return scanID + 1;
    }
    return scanID;
}

/*
    starts all ESCs in bus and prepares them for receiving the fast throttle command should be called until _SetupActive >= MAX_SUPPORTED_CH
    returns the current used ID
*/
uint8_t AP_FETtecOneWire::InitESCs()
{
    static uint8_t delayLoops = 0;
    static uint8_t activeID = 1;
    static uint8_t State = 0;
    static uint8_t TimeOut = 0;
    static uint8_t wakeFromBL = 1;
    static uint8_t setFastCommand[4] = {OW_SET_FAST_COM_LENGTH, 0, 0, 0};
    uint8_t response[18] = {0};
    uint8_t request[1] = {0};
    if (_SetupActive == 0) {
        delayLoops = 0;
        activeID = 1;
        State = 0;
        TimeOut = 0;
        wakeFromBL = 1;
        return _SetupActive + 1;
    }
    while (_activeESC_IDs[_SetupActive] == 0 && _SetupActive < MAX_SUPPORTED_CH) {
        _SetupActive++;
    }

    if (_SetupActive == MAX_SUPPORTED_CH && wakeFromBL == 0) {
        return _SetupActive;
    } else if (_SetupActive == MAX_SUPPORTED_CH && wakeFromBL) {
        wakeFromBL = 0;
        activeID = 1;
        _SetupActive = 1;
        State = 0;
        TimeOut = 0;

        _minID = MAX_SUPPORTED_CH;
        _maxID = 0;
        _IDcount = 0;
        for (uint8_t i = 0; i < MAX_SUPPORTED_CH; i++) {
            if (_activeESC_IDs[i] != 0) {
                _IDcount++;
                if (i < _minID) {
                    _minID = i;
                }
                if (i > _maxID) {
                    _maxID = i;
                }
            }
        }

        if (_IDcount == 0
                || _maxID - _minID > _IDcount - 1) { // loop forever
            wakeFromBL = 1;
            return activeID;
        }
        _FastThrottleByteCount = 1;
        int8_t bitCount = 12 + (_IDcount * 11);
        while (bitCount > 0) {
            _FastThrottleByteCount++;
            bitCount -= 8;
        }
        setFastCommand[1] = _FastThrottleByteCount; // just for older ESC FW versions since 1.0 001 this byte is ignored as the ESC calculates it itself
        setFastCommand[2] = _minID;                 // min ESC id
        setFastCommand[3] = _IDcount;               // count of ESCs that will get signals
    }

    if (delayLoops > 0) {
        delayLoops--;
        return _SetupActive;
    }

    if (activeID < _SetupActive) {
        activeID = _SetupActive;
        State = 0;
        TimeOut = 0;
    }

    if (TimeOut == 3 || TimeOut == 6 || TimeOut == 9 || TimeOut == 12) {
        PullReset();
    }

    if (TimeOut < 15) {
        if (wakeFromBL) {
            switch (State) {
            case 0:request[0] = OW_BL_START_FW;
                if (_foundESCs[activeID].inBootLoader == 1) {
                    Transmit(activeID, request, FETtecOneWire_RequestLength[request[0]]);
                    delayLoops = 5;
                } else {
                    return activeID + 1;
                }
                State = 1;
                break;
            case 1:request[0] = OW_OK;
                if (PullCommand(activeID, request, response, OW_RETURN_FULL_FRAME)) {
                    TimeOut = 0;
                    if (response[0] == 0x02) {
                        _foundESCs[activeID].inBootLoader = 1;
                        State = 0;
                    } else {
                        _foundESCs[activeID].inBootLoader = 0;
                        delayLoops = 1;
                        return activeID + 1;
                    }
                } else {
                    TimeOut++;
                }
                break;
            }
        } else {
            if (PullCommand(activeID, setFastCommand, response, OW_RETURN_RESPONSE)) {
                TimeOut = 0;
                delayLoops = 1;
                return activeID + 1;
            } else {
                TimeOut++;
            }
        }
    } else {
        PullReset();
        return activeID + 1;
    }
    return activeID;
}

/*
    checks if the requested telemetry is available. 
    @param Telemetry 16bit array where the read Telemetry will be stored in.
    @return the telemetry request number or -1 if unavailable
*/
int8_t AP_FETtecOneWire::CheckForTLM(uint16_t* Telemetry)
{
    int8_t return_TLM_request = 0;
    if (_IDcount > 0) {
        // empty buffer
        while (_IgnoreOwnBytes > 0 && _uart->available()) {
            _uart->read();
            _IgnoreOwnBytes--;
        }

        // first two byte are the ESC Telemetry of the first ESC. next two byte of the second....
        if (_uart->available() == (_IDcount * 2) + 1u) {
            // look if first byte in buffer is equal to last byte of throttle command (crc)
            if (_uart->read() == _lastCRC) {
                for (uint8_t i = 0; i < _IDcount; i++) {
                    Telemetry[i] = _uart->read() << 8;
                    Telemetry[i] |= _uart->read();
                }
                return_TLM_request = _TLM_request;
            } else {
                return_TLM_request = -1;
            }
        } else {
            return_TLM_request = -1;
        }
    } else {
        return_TLM_request = -1;
    }
    return return_TLM_request;
}

/*
    does almost all of the job.
    scans for ESCs if not already done.
    initializes the ESCs if not already done.
    sends fast throttle signals if init is complete.
    @param motorValues a 16bit array containing the throttle signals that should be sent to the motors. 0-2000 where 1001-2000 is positive rotation and 999-0 reversed rotation
    @param Telemetry 16bit array where the read telemetry will be stored in.
    @param motorCount the count of motors that should get values send
    @param tlmRequest the requested telemetry type (OW_TLM_XXXXX)
    @return the telemetry request if telemetry was available, -1 if dont
*/
int8_t AP_FETtecOneWire::ESCsSetValues(uint16_t* motorValues, uint16_t* Telemetry, uint8_t motorCount,
        uint8_t tlmRequest)
{
    int8_t return_TLM_request = -2;

    // init should not be done too fast. as at last the bootloader has some timing requirements with messages. so loop delays must fit more or less
    if (_ScanActive < MAX_SUPPORTED_CH || _SetupActive < MAX_SUPPORTED_CH) {
        const uint32_t now = AP_HAL::micros();
        if (now - last_send_us < DELAY_TIME_US) {
            return 0;
        } else {
            last_send_us = now;
        }

        if (_ScanActive < MAX_SUPPORTED_CH) {
            // scan for all ESCs in onewire bus
            _ScanActive = ScanESCs();
        } else if (_SetupActive < MAX_SUPPORTED_CH) {
            if (_FoundESCs == 0) {
                _ScanActive = 0;
            } else {
                // check if in bootloader, start ESCs FW if they are and prepare fast throttle command
                _SetupActive = InitESCs();
            }
        }
    } else {
        //send fast throttle signals
        if (_IDcount > 0) {

            // check for telemetry
            return_TLM_request = CheckForTLM(Telemetry);
            _TLM_request = tlmRequest;

            //prepare fast throttle signals
            uint16_t useSignals[24] = {0};
            uint8_t OneWireFastThrottleCommand[36] = {0};
            if (motorCount > _IDcount) {
                motorCount = _IDcount;
            }
            for (uint8_t i = 0; i < motorCount; i++) {
                useSignals[i] = constrain_int16(motorValues[i], 0, 2000);
            }

            uint8_t actThrottleCommand = 0;

            // byte 1:
            // bit 0 = TLMrequest, bit 1,2,3 = TLM type, bit 4 = first bit of first ESC (11bit)signal, bit 5,6,7 = frame header
            // so ABBBCDDD
            // A = TLM request yes or no
            // B = TLM request type (temp, volt, current, erpm, consumption, debug1, debug2, debug3)
            // C = first bit from first throttle signal
            // D = frame header
            OneWireFastThrottleCommand[0] = 128 | (_TLM_request << 4);
            OneWireFastThrottleCommand[0] |= ((useSignals[actThrottleCommand] >> 10) & 0x01) << 3;
            OneWireFastThrottleCommand[0] |= 0x01;

            // byte 2:
            // AAABBBBB
            // A = next 3 bits from (11bit)throttle signal
            // B = 5bit target ID
            OneWireFastThrottleCommand[1] = (((useSignals[actThrottleCommand] >> 7) & 0x07)) << 5;
            OneWireFastThrottleCommand[1] |= ALL_ID;

            // following bytes are the rest 7 bit of the first (11bit) throttle signal, and all bit from all other signals, followed by the CRC byte
            uint8_t BitsLeftFromCommand = 7;
            uint8_t actByte = 2;
            uint8_t bitsFromByteLeft = 8;
            uint8_t bitsToAddLeft = (12 + (((_maxID - _minID) + 1) * 11)) - 16;
            while (bitsToAddLeft > 0) {
                if (bitsFromByteLeft >= BitsLeftFromCommand) {
                    OneWireFastThrottleCommand[actByte] |=
                            (useSignals[actThrottleCommand] & ((1 << BitsLeftFromCommand) - 1))
                                    << (bitsFromByteLeft - BitsLeftFromCommand);
                    bitsToAddLeft -= BitsLeftFromCommand;
                    bitsFromByteLeft -= BitsLeftFromCommand;
                    actThrottleCommand++;
                    BitsLeftFromCommand = 11;
                    if (bitsToAddLeft == 0) {
                        actByte++;
                        bitsFromByteLeft = 8;
                    }
                } else {
                    OneWireFastThrottleCommand[actByte] |=
                            (useSignals[actThrottleCommand] >> (BitsLeftFromCommand - bitsFromByteLeft))
                                    & ((1 << bitsFromByteLeft) - 1);
                    bitsToAddLeft -= bitsFromByteLeft;
                    BitsLeftFromCommand -= bitsFromByteLeft;
                    actByte++;
                    bitsFromByteLeft = 8;
                    if (BitsLeftFromCommand == 0) {
                        actThrottleCommand++;
                        BitsLeftFromCommand = 11;
                    }
                }
            }
            // empty buffer
            while (_uart->available()) {
                _uart->read();
            }

            // send throttle signal
            OneWireFastThrottleCommand[_FastThrottleByteCount - 1] = GetCrc8(
                    OneWireFastThrottleCommand, _FastThrottleByteCount - 1);
            _uart->write(OneWireFastThrottleCommand, _FastThrottleByteCount);
            // last byte of signal can be used to make sure the first TLM byte is correct, in case of spike corruption
            _IgnoreOwnBytes = _FastThrottleByteCount - 1;
            _lastCRC = OneWireFastThrottleCommand[_FastThrottleByteCount - 1];
            // the ESCs will answer the TLM as 16bit each ESC, so 2byte each ESC.
        }
    }
    return return_TLM_request; // returns the read tlm as it is 1 loop delayed
}
#endif  // HAL_AP_FETTECONEWIRE_ENABLED
