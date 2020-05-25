#include "common.h"

char            g_AppName[XL_MAX_APPNAME + 1]; // = "XL_Example";
XLportHandle g_xlPortHandle = XL_INVALID_PORTHANDLE;
XLdriverConfig  g_xlDrvConfig;
XLaccess        g_xlChannelMask;// = 0;
XLaccess        g_xlPermissionMask; // = 0;
unsigned int    g_BaudRate; // = 500000;                 //!< Default baudrate   
int             g_silent; // 0;                      //!< flag to visualize the message events (on/off)   
unsigned int    g_TimerRate; // = 0;

							 ///////////////////////////////////////////////////////////
							 // thread varijable
XLhandle        g_hMsgEvent;                                          //!< notification handle for the receive queue   
HANDLE          g_hRXThread;                                          //!< thread handle (RX)   
HANDLE          g_hTXThread;                                          //!< thread handle (TX)   
int             g_RXThreadRun;                                        //!< flag to start/stop the RX thread   
int             g_TXThreadRun;
unsigned int    g_canFdSupport; //0;                          //!< Global CAN FD support flag
unsigned int    g_canFdModeNoIso = ENABLE_CAN_FD_MODE_NO_ISO;  //!< Global CAN FD ISO (default) / no ISO mode flag
unsigned int    g_TXThreadCanId;                                     //!< CAN-ID the TX thread transmits under
XLaccess        g_TXThreadTxMask;                                     //!< channel mask the TX thread uses for transmitting


																	  
// Printanje Hardverske konfiguracije																	  
//////////////////////////////////////////////////////////////////
void PrintConfig()
{

	unsigned int i;
	char	str[XL_MAX_LENGTH + 1] = "";


	cout << "------------------------------------------" << endl;
	cout << "	Hardverska konfiguracija         \n" << endl;
	cout << "-  	 " << g_xlDrvConfig.channelCount << " kanala" << endl;
	cout << "------------------------------------------" << endl;

	for (i = 0; i < g_xlDrvConfig.channelCount; i++)
	{
		printf("- Ch.: %02d, CM:0x%3I64x,",
			g_xlDrvConfig.channel[i].channelIndex, g_xlDrvConfig.channel[i].channelMask);
		printf(" %23s ", g_xlDrvConfig.channel[i].name);
		if (g_xlDrvConfig.channel[i].transceiverType != XL_TRANSCEIVER_TYPE_NONE) {
			strncpy(str, g_xlDrvConfig.channel[i].transceiverName, 13);
			printf(" %13s -\n", str);
		}
	}
}
//////////////////////////////////////////////////////////////////


// funckija za primanje parametara iz komandne linije
//////////////////////////////////////////////////////////////////
void argument_call(int argc, char * argv[])
{
	unsigned int txID;
	if (argc > 1) {
		g_BaudRate = atoi(argv[1]);
		if (g_BaudRate) {
			cout << "------------------------------------------";
			printf("\t\nBaudrate = %u\n", g_BaudRate);
			argc--;
			argv++;
		}
	}
	if (argc > 1) {
		strncpy_s(g_AppName, XL_MAX_APPNAME, argv[1], XL_MAX_APPNAME);
		g_AppName[XL_MAX_APPNAME] = 0;
		printf("\t\nNaziv Aplikacije = %s\n", g_AppName);
		argc--;
		argv++;
	}
	if (argc > 1) {
		sscanf_s(argv[1], "%x", &txID);
		if (txID) {
			printf("TX ID = %x\n", txID);
		}
	}

}
//////////////////////////////////////////////////////////////////


//Funkcija Help() sluzi za ispis funkcionalnosti aplikacije 
//////////////////////////////////////////////////////////////////
void Help()
{
	

	printf("\n----------------------------------------------------------\n");
	printf("-                   xlCANdemo - HELP                     -\n");
	printf("----------------------------------------------------------\n");
	printf("- Keyboard commands:                                     -\n");
	printf("- 't'      Transmit a message                            -\n");
	printf("- 'b'      Transmit a message burst (toggle)             -\n");
	printf("- 'm'      Transmit a remote message                     -\n");
	printf("- 'g'      Request chipstate                             -\n");
	printf("- 's'      Start/Stop                                    -\n");
	printf("- 'r'      Reset clock                                   -\n");
	printf("- '+'      Select channel      (up)                      -\n");
	printf("- '-'      Select channel      (down)                    -\n");
	printf("- 'i'      Select transmit Id  (up)                      -\n");
	printf("- 'I'      Select transmit Id  (down)                    -\n");
	printf("- 'x'      Toggle extended/standard Id                   -\n");
	printf("- 'o'      Toggle output mode                            -\n");
	printf("- 'a'      Toggle timer                                  -\n");
	printf("- 'v'      Toggle logging to screen                      -\n");
	printf("- 'p'      Show hardware configuration                   -\n");
	printf("- 'y'      Trigger HW-Sync pulse                         -\n");
	printf("- 'h'      Help                                          -\n");
	printf("- 'ESC'    Exit                                          -\n");
	printf("----------------------------------------------------------\n");
	printf("- 'PH'->PortHandle; 'CM'->ChannelMask; 'PM'->Permission  -\n");
	printf("----------------------------------------------------------\n\n");

}
//////////////////////////////////////////////////////////////////



// Inicijalizacija Drajvera
//////////////////////////////////////////////////////////////////
XLstatus InitDriver(XLaccess *pxlChannelMaskTx, unsigned int *pxlChannelIndex) 
{

	XLstatus          xlStatus;
	unsigned int      i;
	XLaccess          xlChannelMaskFd = 0;
	XLaccess          xlChannelMaskFdNoIso = 0;

	xlStatus = xlOpenDriver();

	// kreiranje strukture XL_DRIVER_CONFIG
	XL_DRIVER_CONFIG xldrConf;

	//Inicijalizacija same strukture
	xldrConf.channel[XL_CONFIG_MAX_CHANNELS];
	xldrConf.channelCount = 0;
	xldrConf.dllVersion = 50;
	xldrConf.reserved[10];

	if (XL_SUCCESS == xlStatus) {

		xlStatus = xlGetDriverConfig(&g_xlDrvConfig);

		PrintConfig();

		g_xlChannelMask = 0;
		for (i = 0; i < g_xlDrvConfig.channelCount; i++) {

			// Trazimo sve kanale koji podrzavaju CAN magistralu
			if (g_xlDrvConfig.channel[i].channelBusCapabilities & XL_BUS_ACTIVE_CAP_CAN) {

				if (!*pxlChannelMaskTx) {
					*pxlChannelMaskTx = g_xlDrvConfig.channel[i].channelMask;
					*pxlChannelIndex = g_xlDrvConfig.channel[i].channelIndex;
				}

				// check if we can use CAN FD - the virtual CAN driver supports CAN-FD, but we don't use it
				if ((g_xlDrvConfig.channel[i].channelCapabilities & XL_CHANNEL_FLAG_CANFD_ISO_SUPPORT)
					&& (g_xlDrvConfig.channel[i].hwType != XL_HWTYPE_VIRTUAL)) {
					xlChannelMaskFd |= g_xlDrvConfig.channel[i].channelMask;

					// check CAN FD NO ISO support
					if (g_xlDrvConfig.channel[i].channelCapabilities & XL_CHANNEL_FLAG_CANFD_BOSCH_SUPPORT) {
						xlChannelMaskFdNoIso |= g_xlDrvConfig.channel[i].channelMask;
					}
				}
				else {
					g_xlChannelMask |= g_xlDrvConfig.channel[i].channelMask;
				}

			}
		}

		// if we found a CAN FD supported channel - we use it.
		if (xlChannelMaskFd && !g_canFdModeNoIso) {
			g_xlChannelMask = xlChannelMaskFd;
			printf("- Use CAN-FD for   : CM=0x%I64x\n", g_xlChannelMask);
			g_canFdSupport = 1;
		}

		if (xlChannelMaskFdNoIso && g_canFdModeNoIso) {
			g_xlChannelMask = xlChannelMaskFdNoIso;
			printf("- Use CAN-FD NO ISO for   : CM=0x%I64x\n", g_xlChannelMask);
			g_canFdSupport = 1;
		}

		if (!g_xlChannelMask) {
			printf("ERROR: no available channels found! (e.g. no CANcabs...)\n\n");
			xlStatus = XL_ERROR;
		}


	}
	g_xlPermissionMask = g_xlChannelMask;

	// ------------------------------------
	// Otvaranje porta, ujedno sa svima kanalima
	// ------------------------------------

	// proveravamo mogucnost koriscenja CAN FD
	if (g_canFdSupport) {

		xlStatus = xlOpenPort(&g_xlPortHandle, g_AppName, g_xlChannelMask, &g_xlPermissionMask, RX_QUEUE_SIZE_FD, XL_INTERFACE_VERSION_V4, XL_BUS_TYPE_CAN);
	}
	// ako nemozemo koristimo obican CAN
	else {
		xlStatus = xlOpenPort(&g_xlPortHandle, g_AppName, g_xlChannelMask, &g_xlPermissionMask, RX_QUEUE_SIZE, XL_INTERFACE_VERSION, XL_BUS_TYPE_CAN);

	}

	printf("- OpenPort         : CM=0x%I64x, PH=0x%02X, PM=0x%I64x, %s\n",
		g_xlChannelMask, g_xlPortHandle, g_xlPermissionMask, xlGetErrorString(xlStatus));





	// ------------------------------------
	// Ako imamo odobrenje mozemo izmeniti bus parametre
	// (baudrate)
	/*	typedef struct {
	unsigned int  arbitrationBitRate;
	unsigned int  sjwAbr;              // CAN bus timing for nominal / arbitration bit rate
	unsigned int  tseg1Abr;
	unsigned int  tseg2Abr;
	unsigned int  dataBitRate;
	unsigned int  sjwDbr;              // CAN bus timing for data bit rate
	unsigned int  tseg1Dbr;
	unsigned int  tseg2Dbr;
	unsigned char reserved;            // has to be zero
	unsigned char options;             // CANFD_CONFOPT_
	unsigned char reserved1[2];        // has to be zero
	unsigned int  reserved2;           // has to be zero
	} XLcanFdConf;
	*/
	// ------------------------------------


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

				if (g_canFdModeNoIso) {
					fdParams.options = CANFD_CONFOPT_NO_ISO;
				}


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
//////////////////////////////////////////////////////////////////



// funckija za kreiranje treda za prijem poruke
//////////////////////////////////////////////////////////////////
XLstatus CreateRxThread(void)
{
	XLstatus		 xlStatus = XL_ERROR;
	DWORD	         ThreadId = 0;

	if (g_xlPortHandle != XL_INVALID_PORTHANDLE)
	{
		xlStatus = xlSetNotification(g_xlPortHandle, &g_hMsgEvent, 1);
		// Parametri: PortHandel(OpenPort funkcija), pokazivac na dogadjaj i 1 prestavlja nivo reda dogadjaja
		if (g_canFdSupport) {
			g_hRXThread = CreateThread(0, 0x1000, RxCanFdThread, (LPVOID)0, 0, &ThreadId);
		}
		else {
			g_hRXThread = CreateThread(0, 0x1000, RxThread, (LPVOID)0, 0, &ThreadId);
		}
	}

	return xlStatus;
	
}
//////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////
DWORD WINAPI RxCanFdThread(PVOID par)
{
	return 0;
}
//////////////////////////////////////////////////////////////////



// Kreiranje Treda za primanje poruka
//////////////////////////////////////////////////////////////////
DWORD WINAPI RxThread(LPVOID par)
{
	XLstatus        xlStatus;

	unsigned int    msgsrx = RECEIVE_EVENT_SIZE;
	XLevent         xlEvent;

	UNUSED_PARAM(par);

	g_RXThreadRun = 1;

	//cekanje dogadjaja
	while (g_RXThreadRun) {

		WaitForSingleObject(g_hMsgEvent, 10);

		xlStatus = XL_SUCCESS;

		while (!xlStatus) {

			msgsrx = RECEIVE_EVENT_SIZE;

			xlStatus = xlReceive(g_xlPortHandle, &msgsrx, &xlEvent);
			if (xlStatus != XL_ERR_QUEUE_IS_EMPTY) {

				if (!g_silent) {
					printf("%s\n", xlGetEventString(&xlEvent));
				}

			}
		}

	}
	return NO_ERROR;
}
//////////////////////////////////////////////////////////////////




//Funkcija za slanje poruke
//////////////////////////////////////////////////////////////////
XLstatus Transmit(unsigned int txID, XLaccess xlChanMaskTx)
{

	XLstatus	xlStatus;
	unsigned int messageCount = 1;
	static int cnt = 0;

	if (g_canFdSupport) {

		unsigned int fl[3] = {
			0,
			XL_CAN_TXMSG_FLAG_EDL,
			XL_CAN_TXMSG_FLAG_EDL | XL_CAN_TXMSG_FLAG_BRS,
		};

		//Kreiranje strukture XLcanTxEvent
		XLcanTxEvent canTxEvt;
		unsigned int cntSent;
		unsigned int i;

		memset(&canTxEvt, 0, sizeof(canTxEvt));
		canTxEvt.tag = XL_CAN_EV_TAG_TX_MSG;

		canTxEvt.tagData.canMsg.canId = txID;
		canTxEvt.tagData.canMsg.msgFlags = fl[cnt % (sizeof(fl) / sizeof(fl[0]))]; // ?? 
		canTxEvt.tagData.canMsg.dlc = 8;

		// Ako je EDL setovan probavamo ovako DLC=15 (64 bytes)
		if (canTxEvt.tagData.canMsg.msgFlags & XL_CAN_TXMSG_FLAG_EDL) {
			canTxEvt.tagData.canMsg.dlc = 15;
		}

		++cnt;

		for (i = 1; i<XL_CAN_MAX_DATA_LEN; ++i) {
			canTxEvt.tagData.canMsg.data[i] = (unsigned char)i - 1;
		}
		canTxEvt.tagData.canMsg.data[0] = (unsigned char)cnt;
		xlStatus = xlCanTransmitEx(g_xlPortHandle, xlChanMaskTx, messageCount, &cntSent, &canTxEvt);
	}
	else {

		static XLevent       xlEvent;

		memset(&xlEvent, 0, sizeof(xlEvent));

		xlEvent.tag = XL_TRANSMIT_MSG;
		xlEvent.tagData.msg.id = txID;
		xlEvent.tagData.msg.dlc = 8;
		xlEvent.tagData.msg.flags = 0;
		++xlEvent.tagData.msg.data[0];
		xlEvent.tagData.msg.data[1] = 2;
		xlEvent.tagData.msg.data[2] = 3;
		xlEvent.tagData.msg.data[3] = 4;
		xlEvent.tagData.msg.data[4] = 5;
		xlEvent.tagData.msg.data[5] = 6;
		xlEvent.tagData.msg.data[6] = 7;
		xlEvent.tagData.msg.data[7] = 8;

		xlStatus = xlCanTransmit(g_xlPortHandle, xlChanMaskTx, &messageCount, &xlEvent);
	}

	printf("- Transmit         : CM(0x%I64x), %s\n", xlChanMaskTx, xlGetErrorString(xlStatus));

	return xlStatus;
}
//////////////////////////////////////////////////////////////////

void uds_print() {

	printf("----------------------------------------------------------\n");
	printf("-                   UDS Services - HELP                     -\n");
	printf("----------------------------------------------------------\n");
	printf("- Moguce funkcijie -\n");
	printf("- 1)     Promena dijagnosticke sesije. UDS Servis(0x10) --> KOD 1 \n");
	printf("- 2)     Slanje reset komande. UDS Servis(0x11) --> KOD 2  \n");
	printf("- 3)     Slanje taster present poruke. UDS Servis(0x3E)  --> KOD 3  \n");
	printf("- 4)     Citanje podataka na osnovu ID. UDS Servis (0x22) --> KOD 4  \n");
	printf("- 5)     Pisanje podataka na osnovu ID. UDS Servis (0x2E) --> KOD 5 \n");
	printf("- 6)     Brisanje DTC: UDS Servis(0x14) --> KOD 6 \n");
	printf("- 7)     Citanje DTC na osnovu status maske: UDS Servis(0x19) --> KOD 7 \n");
	printf("----------------------------------------------------------\n");


}


void uds_choice(int num) {

	switch (num)
	{

	case 1:
		printf("\nOdabrali ste funkciju za promenu dijagnosticke sesije.\n");
		break;
	case 2:
		printf("\nOdabrali ste funkciju za slanje reset komande.\n");
		break;
	case 3:
		printf("\nOdabrali ste funkciju za slanje taster present poruke.\n");
		break;
	case 4:
		printf("\nOdabrali ste funkciju za citanje podataka na osnovu ID.\n");
		break;
	case 5:
		printf("\nOdabrali ste funkciju za pisanje podataka na osnovu ID.\n");
		break;
	case 6:
		printf("\nOdabrali ste funkciju za brisanje DTC.\n");
		break;
	case 7:
		printf("\nOdabrali ste funkciju za citanje DTC na osnovu status maske.\n");
		break;
	default:
		printf("\nNiste dobro uneli parametre !\n");
		break;
	}


}