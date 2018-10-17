/**
 * @file    main.cpp
 * @brief   mbed Endpoint Sample main (dual-stack example w/ HTTPS)
 * @author  Doug Anson
 * @version 1.0
 * @see
 *
 * Copyright (c) 2018
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

// mbed include
#include "mbed.h"

// Serial port
Serial pc(USBTX,USBRX);

// include the SSL CA PEM
#include "ssl_ca_pem.h"

// ************************************************************************
// Stack A: mbed Cloud Client Plugin
// ************************************************************************
#include "mbedCloudClientPlugin.h"

// ************************************************************************
// Stack B: HTTP Exerciser 
// ************************************************************************
#include "https_request.h"
void init_http_exerciser() {
}

void ping_http() {     
    // Dual-stacked endpoint will share this global (from mbedCloudClientPlugin)
    extern NetworkInterface* __network_interface;
    
    // make sure we have a valid network interface (setup by Stack A...) 
    if (__network_interface != NULL) {
        // we do... so lets send a HTTPS POST to a reflector...
        const char body[] = "{\"hello\":\"world\"}";
        
        // create the HTTPS POST... 
        HttpsRequest* request = new HttpsRequest(__network_interface, SSL_CA_PEM, HTTP_POST, (const char *)"http://httpbin.org/post");
        HttpResponse* response = request->send();
        
        // if response is NULL, check response->get_error() 
        if (response != NULL) {
            // we got a response... display it...
            printf("HTTPS: status is %d - %s\r\n",response->get_status_code(),response->get_status_message().c_str());
            printf("HTTPS: body is:\r\n%s\r\n", response->get_body_as_string().c_str());
        }
        else {
            // no response... 
            printf("HTTPS: response object is NULL! (check URL...)\r\n");
        }
     
        if (request != NULL) { 
            // this will delete both the request and the response...   
            delete request;
        }
    }
    else {
        // network not initialized yet by Stack A...
        printf("HTTPS: Skipping ping... network stack not yet initialized (OK)...\r\n");
    }
}

void run_http_exerciser(const void *argv) {
    // Wait 
    ThisThread::sleep_for(10000);            // wait for 10 seconds
    while(true) {
        ping_http();
        ThisThread::sleep_for(10000);        // wait for 10 seconds
    }
}

// ************************************************************************
// Main function
// ************************************************************************
int main() {
    // set the baud of the serial port
    pc.baud(115200);
    
    // Announce
    printf("Dual-Stack mbed-cloud-sample (HTTP) v1.0\r\n");
        
    // setup the mbed cloud endpoint stack
    printf("\n\r----- Initializing mbed cloud endpoint -----\n\r");
    init_mbed_cloud_client_plugin();
    printf("\n\r----- Done initializing mbed cloud endpoint.\n\r");
    
    // setup the HTTP exerciser stack
    printf("\n\r----- Initializing http exerciser -----\n\r");
    init_http_exerciser();
    printf("\n\r----- Done initializing http exerciser.\n\r");

    // setup and run a thread for the HTTP Exeriser (stacksize: 4K)
    Thread http_exerciser_thread(osPriorityNormal, 4 * 1024);
    printf("\n\r----- Starting HTTP exerciser thread... -----\n\r");
    http_exerciser_thread.start(callback(run_http_exerciser, (void *)NULL));
    printf("\n\r----- Done starting HTTP exerciser thread.\n\r");
    
    // start the mbed cloud endpoint main event loop (main loop)
    printf("\n\r----- Starting mbed cloud endpoint event loop -----\n\r");
    start_mbed_cloud_client_plugin_event_loop();
    printf("\n\r----- Done starting mbed cloud endpoint event loop.\n\r");
}