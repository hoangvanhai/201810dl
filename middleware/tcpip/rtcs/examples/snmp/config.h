#ifndef _config_h_
#define _config_h_

/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 1989-2008 ARC International
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   Configurable information for the RTCS examples.
*
*
*END************************************************************************/




#define ENET_DEVICE     0

/*
** Define the Ethernet MAC address, IP address and IP network mask
** These are ignored if ENET_DEVICE is not #define'd
*/
#ifndef ENET_IPADDR
    #define ENET_IPADDR  IPADDR(192,168,1,202) 
#endif

#ifndef ENET_IPMASK
    #define ENET_IPMASK  IPADDR(255,255,255,0)
#endif

#ifndef ENET_IPGATEWAY
    #define ENET_IPGATEWAY  IPADDR(0,0,0,0) 
#endif

/* Define TRAP receiver addresses here */
#define ENET_TRAP_ADDR1 IPADDR(192,168,1,205)
#define ENET_TRAP_ADDR2 IPADDR(192,168,1,206)
#define ENET_TRAP_ADDR3 IPADDR(192,168,1,207)
#define ENET_TRAP_ADDR4 IPADDR(192,168,1,208)
#define ENET_TRAP_ADDR5 IPADDR(192,168,1,209)

/*
** The target's Ethernet address
** The first three bytes should be 00 00 5E
** The last three bytes should match the last three bytes of
** your test IP address
*/
#undef  _IPBN       /* IP address byte number b */
#define _IPBN(b)    (((ENET_IPADDR) >> (b*8)) & 255)
#define ENET_ENETADDR   {0x00, 0x00, 0x5E, _IPBN(2), _IPBN(1), _IPBN(0)}

/*
** Uncomment this line to enable PPP
*/

//#define PPP_DEVICE      "ittya:"

/*
** Define PPP_DEVICE_DUN only when using PPP to communicate
** to Win9x Dial-Up Networking over a null-modem
** This is ignored if PPP_DEVICE is not #define'd
*/
#define PPP_DEVICE_DUN

/*
** Define the local and remote IP addresses for the PPP link
** These are ignored if PPP_DEVICE is not #define'd
*/
#define PPP_LOCADDR     IPADDR(192,168,1,203) 
#define PPP_PEERADDR    IPADDR(192,168,1,204) 

/*
** Define a default gateway
*/
#define GATE_ADDR        IPADDR(0,0,0,0)

#ifndef ENET_IPGATEWAY
    #define ENET_IPGATEWAY  IPADDR(0,0,0,0)
#endif

/* Enable iwconfig commad */
#define DEMOCFG_USE_WIFI           BSP_ENET_WIFI_ENABLED  /* USE WIFI Interface */

#if DEMOCFG_USE_WIFI
    #include "iwcfg.h"

    #define DEMOCFG_SSID            "WiFi_Demo"
    //Possible Values managed or adhoc
    #define DEMOCFG_NW_MODE         "managed"
    //Possible vales
    // 1. "wep"
    // 2. "wpa"
    // 3. "wpa2"
    // 4. "none"
    #define DEMOCFG_SECURITY        "none"
    #define DEMOCFG_PASSPHRASE      NULL
    #define DEMOCFG_WEP_KEY         "ABCDE"
    //Possible values 1,2,3,4
    #define DEMOCFG_WEP_KEY_INDEX   1

    #ifdef BSP_ENET_WIFI_DEVICE
      #define DEMOCFG_DEFAULT_DEVICE       BSP_ENET_WIFI_DEVICE
    #else
      #define DEMOCFG_DEFAULT_DEVICE       (1)
    #endif

    #ifdef BSP_DEFAULT_ENET_DEVICE
      #undef BSP_DEFAULT_ENET_DEVICE
    #endif

   #define BSP_DEFAULT_ENET_DEVICE  DEMOCFG_DEFAULT_DEVICE

#endif //DEMOCFG_USE_WIFI

#endif
/* EOF */
