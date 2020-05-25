#include "string.h"
#include <windows.h>
#include "vxlapi.h"
#include <cstdio>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <windows.h>


#define RECEIVE_EVENT_SIZE         1        // DO NOT EDIT! Currently 1 is supported only
#define RX_QUEUE_SIZE              4096     // internal driver queue size in CAN events
#define RX_QUEUE_SIZE_FD           16384    // driver queue size for CAN-FD Rx events
#define ENABLE_CAN_FD_MODE_NO_ISO  0        // switch to activate no iso mode on a CAN FD channel
#define UNUSED_PARAM(a) { a=a; }

using namespace std;


////////////////////////////////////////////////////////////////////////////
// Funkcije (Tredova)															   
DWORD	  WINAPI RxCanFdThread(PVOID par);
DWORD     WINAPI RxThread(PVOID par);
DWORD     WINAPI TxThread(LPVOID par);

////////////////////////////////////////////////////////////////////////////
// Funckije (Prototipova)
void     Help(void);
void     PrintConfig(void);
XLstatus CreateRxThread(void);

void argument_call(int a, char *b[]);

XLstatus Transmit(unsigned int txID, XLaccess xlChanMaskTx);

XLstatus InitDriver(XLaccess *pxlChannelMaskTx, unsigned int *pxlChannelIndex);

void uds_print();

void uds_choice(int num);