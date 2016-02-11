/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed-drivers/mbed.h"
#include "ble/BLE.h"
#include "LEDService.h"
#include "ws2812.h"
#include "pixel-array.h"
#include "eddystone/EddystoneService.h"

#define WS2812_BUF 8

PixelArray px(WS2812_BUF);
// See the program page for information on the timing numbers
// The given numbers are for the K64F
WS2812 ws(P0_2, WS2812_BUF);

// Eddystone URL
static const char defaultUrl[] = "https://goo.gl/Vmoj51";
// Normal name & services
static char beaconName[] = "LED9";
static uint16_t uuid16_list[] = { LEDService::LED_SERVICE_UUID };

static const PowerLevels_t defaultAdvPowerLevels = {-47, -33, -21, -13};
static const PowerLevels_t radioPowerLevels      = {-30, -16, -4, 4};

LEDService *ledServicePtr;

void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    (void) params;
    BLE::Instance().gap().startAdvertising();
}

/**
 * This callback allows the LEDService to receive updates to the ledState Characteristic.
 *
 * @param[in] params
 *     Information about the characterisitc being updated.
 */
void onDataWrittenCallback(const GattWriteCallbackParams *params) {
    if ((params->handle == ledServicePtr->getValueHandle())) {
        unsigned int color;
        memcpy(&color, params->data, 4);
        px.SetAll(color);
        ws.write(px.getBuf());
    }
}

/**
 * This function is called when the ble initialization process has failled
 */
void onBleInitError(BLE &ble, ble_error_t error)
{
    /* Initialization error handling should go here */
}

/**
 * Callback triggered when the ble initialization process has finished
 */
void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
    BLE&        ble   = params->ble;
    ble_error_t error = params->error;

    if (error != BLE_ERROR_NONE) {
        /* In case of error, forward the error handling to onBleInitError */
        onBleInitError(ble, error);
        return;
    }

    /* Ensure that it is the default instance of BLE */
    if(ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        return;
    }

    ble.gap().onDisconnection(disconnectionCallback);
    ble.gattServer().onDataWritten(onDataWrittenCallback);

    // Set up Eddystone
    auto eddyServicePtr = new EddystoneService(ble, defaultAdvPowerLevels, radioPowerLevels, 0);
    eddyServicePtr->setURLData(defaultUrl);
    // The name of the beacon and the service list
    eddyServicePtr->setNormalFrameData(beaconName, strlen(beaconName), uuid16_list, sizeof(uuid16_list));

    // Every 500 ms. Eddystone URL, then Normal frame
    eddyServicePtr->setUIDFrameAdvertisingInterval(0);
    eddyServicePtr->setTLMFrameAdvertisingInterval(0);
    eddyServicePtr->setURLFrameAdvertisingInterval(500);
    eddyServicePtr->setNormalFrameAdvertisingInterval(500);

    eddyServicePtr->startBeaconService();

    bool initialValueForLEDCharacteristic = false;
    ledServicePtr = new LEDService(ble, initialValueForLEDCharacteristic);
}

void app_start(int, char **)
{
    ws.useII(WS2812::PER_PIXEL); // use per-pixel intensity scaling
    px.SetAll(0x00000000);
    ws.write(px.getBuf());

    BLE &ble = BLE::Instance();
    ble.init(bleInitComplete);
}
