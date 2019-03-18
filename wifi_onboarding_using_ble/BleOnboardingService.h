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
 * CA BLE On-boarding service.
 * CA_BLE_SERVICE
 * @par usage
 *
 * When this class is instantiated, it adds a custom service in the GattServer.
 */
class caBleService {
    typedef caBleService Self;

public:
    caBleService() :
        _cirrent_device_info_char("00004A01-0000-1000-8000-C4f78190bfd8", 0),
        _seconds_since_power_on_char("00004A0A-0000-1000-8000-C4f78190bfd8", 0),
        _ca_indicate_char("00004A0B-0000-1000-8000-C4f78190bfd8", 0),
		_ca_notify_char("00004A0C-0000-1000-8000-C4f78190bfd8", 0),
        _join_private_network_char("00004A0D-0000-1000-8000-C4f78190bfd8", 0),
        _id_yourself_char("00004A0E-0000-1000-8000-C4f78190bfd8", 0),
		_status_notify_char("00004A0F-0000-1000-8000-C4f78190bfd8", 0),
        _ca_ble_onboarding_service(
        	/* uuid */ 	"00003801-0000-1000-8000-C4f78190bfd8",
            /* characteristics */ _ca_ble_onboarding_characteristics,
            /* numCharacteristics */ sizeof(_ca_ble_onboarding_characteristics) /
                                     sizeof(_ca_ble_onboarding_characteristics[0])
        ),
        _server(NULL),
        _event_queue(NULL)
    {


        // update internal pointers (value, descriptors and characteristics array)
    	_ca_ble_onboarding_characteristics[0] = &_cirrent_device_info_char;
    	_ca_ble_onboarding_characteristics[1] = &_seconds_since_power_on_char;
    	_ca_ble_onboarding_characteristics[2] = &_ca_indicate_char;
    	_ca_ble_onboarding_characteristics[3] = &_ca_notify_char;
    	_ca_ble_onboarding_characteristics[4] = &_join_private_network_char;
    	_ca_ble_onboarding_characteristics[5] = &_id_yourself_char;
    	_ca_ble_onboarding_characteristics[6] = &_status_notify_char;

    	_seconds_since_power_on_char.setReadAuthorizationCallback(this,&Self::clientReadReqCb);

    }



    void start(BLE &ble_interface, events::EventQueue &event_queue)
    {
        _server = &ble_interface.gattServer();

        // register the service
        printf("Adding Custom demo service\r\n");
        ble_error_t err = _server->addService(_ca_ble_onboarding_service);

        if (err) {
            printf("Error %u during ca service registration.\r\n", err);
            return;
        }

        // read write handler
        _server->onDataSent(as_cb(&Self::when_data_sent));
        _server->onDataWritten(as_cb(&Self::when_data_written));
        _server->onDataRead(as_cb(&Self::when_data_read));


        // print the handles
        printf("Cirrent Agent Service registered\r\n");
        printf("service handle: %u\r\n", _ca_ble_onboarding_service.getHandle());
        for(uint8_t i=0;i<_ca_ble_onboarding_service.getCharacteristicCount();i++)
        {
        	GattCharacteristic *char1 = _ca_ble_onboarding_service.getCharacteristic(i);
        	printf("\t Characteristic UUID = %u, Handle= %u\r\n",char1->getValueHandle() ,char1->getValueHandle());
        }
    }

private:
    void clientReadReqCb(GattReadAuthCallbackParams* param)
    {
    	printf("Read Initiated for _seconds_since_power_on_char\r\n");
    }
    /**
     * Handler called when a notification or an indication has been sent.
     */
    void when_data_sent(unsigned count)
    {
        printf("sent %u updates\r\n", count);
    }
    /**
     * Handler called after an attribute has been written.
     */
    void when_data_written(const GattWriteCallbackParams *e)
    {
    	bool enabled;
    	uint8_t val= 0x12;
        printf("data written:\r\n");
        printf("\tconnection handle: %u\r\n", e->connHandle);
        printf("\tattribute handle: %u", e->handle);
        if (e->handle == _ca_indicate_char.getValueHandle())
        {
            printf(" CA Indicate characteristic has  been written to by the Client \r\n");
            _server->areUpdatesEnabled(_ca_indicate_char, &enabled);
            if(enabled)
            {
            	printf(" _ca_indicate_char characteristic Indication are Enabled \r\n");
            	_ca_indicate_char.set(*_server,val);
            	printf(" _ca_indicate_char characteristic Updated \r\n");
            }
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
        printf("\r\n");

    }
    /**
     * Helper that construct an event handler from a member function of this
     * instance.
     */
    template<typename Arg>
    FunctionPointerWithContext<Arg> as_cb(void (Self::*member)(Arg))
    {
        return makeFunctionPointer(this, member);
    }

    template<typename T>
    class WriteIndicateCharacteristic : public GattCharacteristic {
    public:
    		WriteIndicateCharacteristic(const UUID & uuid, const T& initial_value) :
            GattCharacteristic(
                /* UUID */ uuid,
                /* Initial value */ &_value,
                /* Value size */ sizeof(_value),
                /* Value capacity */ sizeof(_value),
                /* Properties */ GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE |
                                 GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE,
                /* Descriptors */ NULL,
                /* Num descriptors */ 0,
                /* variable len */ false
            ),
            _value(initial_value) {
        }
        ble_error_t get(GattServer &server, T& dst) const
        {
            uint16_t value_length = sizeof(dst);
            return server.read(getValueHandle(), &dst, &value_length);
        }

        ble_error_t set(
            GattServer &server, const T &value, bool local_only = false
        ) const {
            return server.write((GattAttribute::Handle_t)getValueHandle(), &value, sizeof(value), local_only);
        }

    private:
        T _value;
    };

    template<typename T>
    class NoPropertyCharacteristic : public GattCharacteristic {
    public:
    	NoPropertyCharacteristic(const UUID & uuid, const T& initial_value) :
            GattCharacteristic(
                /* UUID */ uuid,
                /* Initial value */ &_value,
                /* Value size */ sizeof(_value),
                /* Value capacity */ sizeof(_value),
                /* Properties */ GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NONE,
                /* Descriptors */ NULL,
                /* Num descriptors */ 0,
                /* variable len */ false
            ),
            _value(initial_value) {
        }

    private:
        T _value;
    };

    ReadOnlyGattCharacteristic<uint8_t> _cirrent_device_info_char;
    ReadOnlyGattCharacteristic<uint8_t>_seconds_since_power_on_char;
    WriteIndicateCharacteristic<uint8_t>_ca_indicate_char;
    NoPropertyCharacteristic<uint8_t>_ca_notify_char;
    WriteIndicateCharacteristic<uint8_t>_join_private_network_char;
    WriteOnlyGattCharacteristic<uint8_t>_id_yourself_char;
    ReadOnlyGattCharacteristic<uint8_t>_status_notify_char;

    // list of the characteristics of the ble_onboarding service

    GattCharacteristic* _ca_ble_onboarding_characteristics[7];

    // demo service
    GattService _ca_ble_onboarding_service;

    GattServer* _server;
    events::EventQueue *_event_queue;
};

#endif /* BLE_FEATURE_GATT_SERVER */
#endif /* MBED_BLE_BLE_ONBOARDING_SERVICE_H__ */
