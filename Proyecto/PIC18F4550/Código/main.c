/***************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

To request to license the code under the MLA license (www.microchip.com/mla_license), 
please contact mla_licensing@microchip.com
 ***************************/

/* INCLUDES ********************/
#include "system.h"

#include "app_device_cdc_basic.h"
#include "app_led_usb_status.h"

#include "usb.h"
#include "usb_device.h"
#include "usb_device_cdc.h"

/************************
 * Function:        void main(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Main program entry point.
 *
 * Note:            None
 ***********************/


void APP_DeviceCDCBasicDemoTasks_Microside(void) {
    uint8_t readCommand[64];
    uint8_t writeCommand[64];
    if (USBUSARTIsTxTrfReady() == true) {
        uint8_t i, i_res = 0, numBytesRead;
        numBytesRead = getsUSBUSART(readCommand, sizeof (readCommand));
        for (i = 0; i < numBytesRead; i++) {
            switch (readCommand[i]) {
                case '?':
                    if (PORTAbits.RA2) {
                        writeCommand[i_res] = '1';
                        i_res++;
                    } else {
                        writeCommand[i_res] = '0';
                        i_res++;
                    }
                    break;
                case '1':
                    LATAbits.LA1 = 1;
                    break;
                case '0':
                    LATAbits.LA1 = 0;
                    break;
                default:
                    break;
            }
        }
        if (i_res > 0)
            putUSBUSART(writeCommand, i_res);
    }
    CDCTxService();
}

MAIN_RETURN main(void) {
    ADCON1bits.PCFG = 0xFF;
    TRISAbits.RA1 = 0;
    TRISAbits.RA2 = 1;
    LATA = 0;
    SYSTEM_Initialize(SYSTEM_STATE_USB_START);

    USBDeviceInit();
    USBDeviceAttach();
    APP_DeviceCDCBasicDemoInitialize();
    while (1) {
        SYSTEM_Tasks();

#if defined(USB_POLLING)
        // Interrupt or polling method.  If using polling, must call
        // this function periodically.  This function will take care
        // of processing and responding to SETUP transactions
        // (such as during the enumeration process when you first
        // plug in).  USB hosts require that USB devices should accept
        // and process SETUP packets in a timely fashion.  Therefore,
        // when using polling, this function should be called
        // regularly (such as once every 1.8ms or faster** [see
        // inline code comments in usb_device.c for explanation when
        // "or faster" applies])  In most cases, the USBDeviceTasks()
        // function does not take very long to execute (ex: <100
        // instruction cycles) before it returns.
        USBDeviceTasks();
#endif

        //Application specific tasks
        //APP_DeviceCDCBasicDemoTasks();
        APP_DeviceCDCBasicDemoTasks_Microside();

    }//end while
}//end main

/***************************
 End of File
 */