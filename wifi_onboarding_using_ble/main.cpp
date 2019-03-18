/* mbed Microcontroller Library
 * Copyright (c) 2017 ARM Limited
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

#include <stdio.h>

#include "platform/Callback.h"
#include "events/EventQueue.h"
#include "platform/NonCopyable.h"

#include "ble/BLE.h"
#include "ble/Gap.h"
#include "ble/GattClient.h"
#include "ble/GapAdvertisingParams.h"
#include "ble/GapAdvertisingData.h"
#include "ble/GattServer.h"
#include "BLEProcess.h"
#include "BleOnboardingService.h"

using mbed::callback;

extern int stdio_uart_inited;

void passkeyDisplayCallback(Gap::Handle_t handle, const SecurityManager::Passkey_t passkey)
{
    printf("Input passKey: ");
    for (unsigned i = 0; i < Gap::ADDR_LEN; i++) {
        printf("%c ", passkey[i]);
    }
    printf("\r\n");
}

void securitySetupCompletedCallback(Gap::Handle_t handle, SecurityManager::SecurityCompletionStatus_t status)
{
    if (status == SecurityManager::SEC_STATUS_SUCCESS) {
        printf("Security success\r\n");
    } else {
        printf("Security failed. Reason:");
        switch(status)
        {
        case SecurityManager::SEC_STATUS_TIMEOUT:
        	printf("SEC_STATUS_TIMEOUT\r\n");
        	break;
        case SecurityManager::SEC_STATUS_PDU_INVALID:
        	printf("SEC_STATUS_PDU_INVALID\r\n");
        	break;
        case SecurityManager::SEC_STATUS_PASSKEY_ENTRY_FAILED:
        	printf("SEC_STATUS_PASSKEY_ENTRY_FAILED\r\n");
        	break;
        case SecurityManager::SEC_STATUS_OOB_NOT_AVAILABLE:
        	printf("SEC_STATUS_OOB_NOT_AVAILABLE\r\n");
        	break;
        case SecurityManager::SEC_STATUS_AUTH_REQ:
        	printf("SEC_STATUS_AUTH_REQ\r\n");
        	break;
        case SecurityManager::SEC_STATUS_CONFIRM_VALUE:
        	printf("SEC_STATUS_CONFIRM_VALUE\r\n");
        	break;
        case SecurityManager::SEC_STATUS_PAIRING_NOT_SUPP:
        	printf("SEC_STATUS_PAIRING_NOT_SUPP\r\n");
        	break;
        case SecurityManager::SEC_STATUS_ENC_KEY_SIZE:
        	printf("SEC_STATUS_ENC_KEY_SIZE\r\n");
        	break;
        case SecurityManager::SEC_STATUS_SMP_CMD_UNSUPPORTED:
        	printf("SEC_STATUS_SMP_CMD_UNSUPPORTED\r\n");
        	break;
        case SecurityManager::SEC_STATUS_UNSPECIFIED:
        	printf("SEC_STATUS_UNSPECIFIED\r\n");
        	break;
        case SecurityManager::SEC_STATUS_REPEATED_ATTEMPTS:
        	printf("SEC_STATUS_REPEATED_ATTEMPTS\r\n");
        	break;
        case SecurityManager::SEC_STATUS_INVALID_PARAMS:
        	printf("SEC_STATUS_INVALID_PARAMS\r\n");
        	break;
        case SecurityManager::SEC_STATUS_DHKEY_CHECK_FAILED:
        	printf("SEC_STATUS_DHKEY_CHECK_FAILED\r\n");
        	break;
        case SecurityManager::SEC_STATUS_COMPARISON_FAILED:
        	printf("SEC_STATUS_COMPARISON_FAILED\r\n");
        	break;
        case SecurityManager::SEC_STATUS_SUCCESS:
        	break;
        }
    }
}


int main() {

    stdio_uart_inited = false;
    bool scEnabled=0;
    BLE &ble_interface = BLE::Instance();
    events::EventQueue event_queue;
    caBleService ca_service_instance;
    dummy a;
    a.count =9;
    BLEProcess ble_process(event_queue, ble_interface,a);
    printf("%d\r\n",ble_process.d.count);
    ble_process.on_init(callback(&ca_service_instance, &caBleService::start));

    // bind the event queue to the ble interface, initialize the interface
    // and start advertising
    ble_process.start();

    ble_interface.securityManager().onPasskeyDisplay(passkeyDisplayCallback);
    ble_interface.securityManager().onSecuritySetupCompleted(securitySetupCompletedCallback);
    ble_interface.securityManager().getSecureConnectionsSupport(&scEnabled);
    // Process the event queue.
    event_queue.dispatch_forever();

    return 0;
}
