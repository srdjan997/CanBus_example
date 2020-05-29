#pragma once


#define RECEIVE_EVENT_SIZE         1        // DO NOT EDIT! Currently 1 is supported only
#define RX_QUEUE_SIZE              4096     // internal driver queue size in CAN events
#define RX_QUEUE_SIZE_FD           16384    // driver queue size for CAN-FD Rx events
#define ENABLE_CAN_FD_MODE_NO_ISO  0        // switch to activate no iso mode on a CAN FD channel
#define UNUSED_PARAM(a) { a=a; }

#define UDS_WRITE_DATA_BY_ID          0x2E
#define UDS_READ_DATA_BY_ID           0x22
#define UDS_CLEAR_DTC                 0x14
#define UDS_DIAGNOSTIC_CONTROL        0x10
#define UDS_ECU_RESET                 0x11
#define UDS_TESTER_PRESENT            0x3E
#define UDS_READ_DTC                  0x19

#define MAX_DATA_ARRAY_SIZE 100


using namespace std;
