#include <iostream>
#include "CAN.hpp"
#include "UDS.hpp"
#include "vxlapi.h"
#include "common.hpp"
#include "types.h"
#include "main.h"
using namespace std;


///////////// InitDriver ///////////////////////

XLstatus InitDriver(XLaccess *pxlChannelMaskTx, unsigned int *pxlChannelIndex)
{
	XLstatus		xlStatus;
	unsigned int	i;
	XLaccess	    xlChannelMaskFd = 0;

	xlStatus = xlOpenDriver();

	XL_DRIVER_CONFIG xldrConf;		/// Sadrzi polja: dllVersion, brojKanala, niz reservisanih, niz kanala[brKanala]

	xldrConf.channel[XL_CONFIG_MAX_CHANNELS];  // 64 
	xldrConf.channelCount = 0;
	xldrConf.dllVersion = 0;
	xldrConf.reserved[10];

	if (XL_SUCCESS == xlStatus)
	{
		xlStatus = xlGetDriverConfig(&g_xlDrvConfig);

		g_xlChannelMask = 0;

		PrintConfig();

		for (i = 0; i < g_xlDrvConfig.channelCount; i++)
		{
			if (g_xlDrvConfig.channel[i].channelBusCapabilities & XL_BUS_ACTIVE_CAP_CAN)
			{
				if (!*pxlChannelMaskTx)
				{
					*pxlChannelMaskTx = g_xlDrvConfig.channel[i].channelMask;
					*pxlChannelIndex = g_xlDrvConfig.channel[i].channelIndex;
				}

				if ((g_xlDrvConfig.channel[i].channelCapabilities & XL_CHANNEL_FLAG_CANFD_ISO_SUPPORT)
					&& (g_xlDrvConfig.channel[i].hwType != XL_HWTYPE_VIRTUAL))
				{
					xlChannelMaskFd |= g_xlDrvConfig.channel[i].channelMask;
				}
				else
				{
					g_xlChannelMask |= g_xlDrvConfig.channel[i].channelMask;
				}


				if (xlChannelMaskFd && !g_canFdModeNoIso)
				{
					g_xlChannelMask = xlChannelMaskFd;
					cout << "- Koristimo CAN-FD za	CM=0x%I64" << g_xlChannelMask << endl;
					g_canFdSupport = 1;
				}

				if (!g_xlChannelMask)
				{
					cout << "ERROR: nema dostupnih kanala! " << endl << endl;
				}

			}

		}
	}

	g_xlPermissionMask = g_xlChannelMask;

	if (g_canFdSupport)
	{
		xlStatus = xlOpenPort(&g_xlPortHandle, g_AppName, g_xlChannelMask, &g_xlPermissionMask, RX_QUEUE_SIZE_FD, XL_INTERFACE_VERSION_V4, XL_BUS_TYPE_CAN);
	}

	printf("- OpenPort         : CM=0x%I64x, PH=0x%02X, PM=0x%I64x, %s\n",
		g_xlChannelMask, g_xlPortHandle, g_xlPermissionMask, xlGetErrorString(xlStatus));


	if ((XL_SUCCESS == xlStatus) && (XL_INVALID_PORTHANDLE != g_xlPortHandle)) {

		if (g_xlChannelMask == g_xlPermissionMask) {

			if (g_canFdSupport) {
				XLcanFdConf fdParams;

				memset(&fdParams, 0, sizeof(fdParams));

				// arbitration bitrate
				fdParams.arbitrationBitRate = 1000000;
				fdParams.tseg1Abr = 6;
				fdParams.tseg2Abr = 3;
				fdParams.sjwAbr = 2;

				// data bitrate
				fdParams.dataBitRate = fdParams.arbitrationBitRate * 2;
				fdParams.tseg1Dbr = 6;
				fdParams.tseg2Dbr = 3;
				fdParams.sjwDbr = 2;

				xlStatus = xlCanFdSetConfiguration(g_xlPortHandle, g_xlChannelMask, &fdParams);
				printf("- SetFdConfig.     : ABaudr.=%u, DBaudr.=%u, %s\n", fdParams.arbitrationBitRate, fdParams.dataBitRate, xlGetErrorString(xlStatus));

			}
			else {
				xlStatus = xlCanSetChannelBitrate(g_xlPortHandle, g_xlChannelMask, g_BaudRate);
				printf("- SetChannelBitrate: baudr.=%u, %s\n", g_BaudRate, xlGetErrorString(xlStatus));
			}
		}
		else {
			printf("-                  : we have NO init access!\n");
		}
	}
	else {

		xlClosePort(g_xlPortHandle);
		g_xlPortHandle = XL_INVALID_PORTHANDLE;
		xlStatus = XL_ERROR;
	}



	return xlStatus;
}




/////////////////// MAIN //////////////////////

int main(int argc, char* argv[])
{

	argument_call(argc,argv);
	XLstatus xlStatus;
	XLaccess      xlChanMaskTx = 0x0001;
	int           stop = 0;
	int           activated = 0;
	int           chanIndex = 0;
	unsigned int  xlChanIndex = 0;
	unsigned int  txID = 0x01;
	unsigned int  messageCount = 0;
	unsigned int  overrunCount = 0;
	unsigned int  lastTime = 0;
	int           outputMode = XL_OUTPUT_MODE_NORMAL;



    cout << "\nDobro dosli u CAN aplikaciju" << endl;

   /* unsigned int ar = 15;
    unsigned int dlc = 7;
    unsigned int flg = 0;

    string new_str = "CAN Read message accapted!";

    CanMessage obj1;

    cout << "\n\t--- Inicijalna poruka ---" << endl;
    print_Can_msg(&obj1);

    obj1.set_arbitradion_id(ar);
    obj1.set_can_flags(flg);
    obj1.set_dlc(dlc);
    obj1.set_str(new_str);

    cout << endl <<"\n\n\t--- Izmenjena poruka ---" << endl;
    print_Can_msg(&obj1);

    cout << endl << "\t\t--- UDS Service ---" << endl;
    UDS obj2;
    cout << "\n\t--- Inicijalni UDS ---" << endl;
    print_UDS(&obj2);
	*/

	xlStatus = InitDriver(&xlChanMaskTx, &xlChanIndex);
	cout << "- Init: " << xlGetErrorString(xlStatus) << endl;
	cout << "\n----------------------------------------------------------\n";

	
	cout << "Unesite UDS Service Format:(0x _ _): " << endl;
	char *in1 = NCharacterFromInput(4);
	char SID[5] = {0};
	bool okey;
	enum Services { READ_DTC = 25, CLEAR_DTC = 20 , WRITE_BY_ID = 46, READ_BY_ID = 34 ,TESTER_PRESENT_MSG = 62 ,RESET_ECU = 17 ,CHANGE_DIAGNOSTIC_SESSION = 16 };
	okey = ExtractNum(in1, SID);
	if (okey == false)
	{
		cout << "Pogresno ste uneli format UDS Servisa" << endl;
	}
	else
		cout << "Format UDS Servisa je odgovarajuc: 0x" << SID << endl;
		
		cout << "\nVas broj u decimalnom sistemu ima vrednost: " << hexadecimalToDecimal(SID) << endl;
		choice_serv(hexadecimalToDecimal(SID));
	
	
	


	


	//char *in1 = NCharacterFromInput(4);
	//cout << in1 << endl;
	//char hexNum[] = "1A";
	//cout << hexadecimalToDecimal(hexNum) << endl;
	
	

    return 0;
}
