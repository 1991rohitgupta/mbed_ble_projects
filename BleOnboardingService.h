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

#ifndef MBED_BLE_BLE_ONBOARDING_SERVICE_H__
#define MBED_BLE_BLE_ONBOARDING_SERVICE_H__

#if BLE_FEATURE_GATT_SERVER

#include <stdio.h>
#include "platform/mbed_assert.h"
#include "ble/BLE.h"
#include "ble/GattServer.h"

using mbed::callback;

/**
 * BLE On-boarding service.
 *
 * @par usage
 *
 * When this class is instantiated, it adds a custom service in the GattServer.
 */
class BleOnboardingService {
    typedef BleOnboardingService Self;

public:
    BleOnboardingService() :
        _p_ssid_char("485f4145-52b9-4644-af1f-7a6b9322490f", 0),
        _p_pwd_char("0a924ca7-87cd-4699-a3bd-abdcd9cf126a", 0),
        _s_ssid_char("8dd6a1b7-bc75-4741-8a26-264af75807de", 0),
		_s_pwd_char("8dd6a1e7-bc75-4741-8a26-264af75807de", 0),
        _ble_onboarding_service(
            /* uuid */ "51311102-030e-485f-b122-f8f381aa84ed",
            /* characteristics */ _ble_onboarding_characteristics,
            /* numCharacteristics */ sizeof(_ble_onboarding_characteristics) /
                                     sizeof(_ble_onboarding_characteristics[0])
        ),
        _server(NULL),
        _event_queue(NULL)
    {
        // update internal pointers (value, descriptors and characteristics array)
        _ble_onboarding_characteristics[0] = &_p_ssid_char;
        _ble_onboarding_characteristics[1] = &_p_pwd_char;
        _ble_onboarding_characteristics[2] = &_s_ssid_char;
        _ble_onboarding_characteristics[3] = &_s_pwd_char;

    }



    void start(BLE &ble_interface, events::EventQueue &event_queue)
    {
        _server = &ble_interface.gattServer();

        // register the service
        printf("Adding Custom demo service\r\n");
        ble_error_t err = _server->addService(_ble_onboarding_service);

        if (err) {
            printf("Error %u during demo service registration.\r\n", err);
            return;
        }

        // read write handler
        _server->onDataWritten(as_cb(&Self::when_data_written));
        _server->onDataRead(as_cb(&Self::when_data_read));


        // print the handles
        printf("BLE on-boarding service registered\r\n");
        printf("service handle: %u\r\n", _ble_onboarding_service.getHandle());
        printf("\tPrimary SSID characteristic value handle %u\r\n", _p_ssid_char.getValueHandle());
        printf("\tPrimary Password characteristic value handle %u\r\n", _p_pwd_char.getValueHandle());
        printf("\tSecondary SSID characteristic value handle %u\r\n", _s_ssid_char.getValueHandle());
        printf("\tSecondary Password characteristic value handle %u\r\n", _s_pwd_char.getValueHandle());

    }

private:

    /**
     * Handler called after an attribute has been written.
     */
    void when_data_written(const GattWriteCallbackParams *e)
    {
        printf("data written:\r\n");
        printf("\tconnection handle: %u\r\n", e->connHandle);
        printf("\tattribute handle: %u", e->handle);
        if (e->handle == _p_ssid_char.getValueHandle()) {
            printf(" (Primary SSID characteristic)\r\n");
        } else if (e->handle == _p_pwd_char.getValueHandle()) {
            printf(" (Primary Password characteristic)\r\n");
        } else if (e->handle == _s_ssid_char.getValueHandle()) {
            printf(" (Secondary SSID characteristic)\r\n");
        } else if (e->handle == _s_pwd_char.getValueHandle()) {
            printf(" (Secondary Password characteristic)\r\n");
        } else {
            printf("\r\n");
        }
        printf("\twrite operation: %u\r\n", e->writeOp);
        printf("\toffset: %u\r\n", e->offset);
        printf("\tlength: %u\r\n", e->len);
        printf("\t data: ");

        for (size_t i = 0; i < e->len; ++i) {
            printf("%02X", e->data[i]);
        }

        printf("\r\n");
    }

    /**
     * Handler called after an attribute has been read.
     */
    void when_data_read(const GattReadCallbackParams *e)
    {
        printf("data read:\r\n");
        printf("\tconnection handle: %u\r\n", e->connHandle);
        printf("\tattribute handle: %u", e->handle);
        if (e->handle == _p_ssid_char.getValueHandle()) {
            printf(" (Primary SSID characteristic)\r\n");
        } else if (e->handle == _p_pwd_char.getValueHandle()) {
            printf(" (Primary Password characteristic)\r\n");
        } else if (e->handle == _s_ssid_char.getValueHandle()) {
            printf(" (Secondary SSID characteristic)\r\n");
        } else if (e->handle == _s_pwd_char.getValueHandle()) {
            printf(" (Secondary Password characteristic)\r\n");
        } else {
            printf("\r\n");
        }
    }


private:
    /**
     * Helper that construct an event handler from a member function of this
     * instance.
     */
    template<typename Arg>
    FunctionPointerWithContext<Arg> as_cb(void (Self::*member)(Arg))
    {
        return makeFunctionPointer(this, member);
    }


    ReadWriteGattCharacteristic<uint32_t> _p_ssid_char;
    ReadWriteGattCharacteristic<uint32_t> _p_pwd_char;
    ReadWriteGattCharacteristic<uint32_t> _s_ssid_char;
    ReadWriteGattCharacteristic<uint32_t> _s_pwd_char;

    // list of the characteristics of the ble_onboarding service
    GattCharacteristic* _ble_onboarding_characteristics[4];

    // demo service
    GattService _ble_onboarding_service;

    GattServer* _server;
    events::EventQueue *_event_queue;
};

#endif /* BLE_FEATURE_GATT_SERVER */
#endif /* MBED_BLE_BLE_ONBOARDING_SERVICE_H__ */
