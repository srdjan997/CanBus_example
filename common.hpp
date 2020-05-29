#pragma once
#include "vxlapi.h"
#include "string.h"
#include <iostream>
#include "types.h"
#include <string>





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
XLaccess        g_TXThreadTxMask;


void argument_call(int a, char *b[])
{
	unsigned int txID;
	if (a > 1) {
		g_BaudRate = atoi(b[1]);
		if (g_BaudRate) {
			cout << "------------------------------------------";
			cout << endl << "BaudRate: " << g_BaudRate << endl;
			a--;
			b++;
		}
	}
	if (a > 1) {
		strncpy_s(g_AppName, XL_MAX_APPNAME, b[1], XL_MAX_APPNAME);
		g_AppName[XL_MAX_APPNAME] = 0;
		printf("\t\nNaziv Aplikacije = %s", g_AppName);
		a--;
		b++;
	}
	if (a > 1) {
		sscanf_s(b[1], "%x", &txID);
		if (txID) {
			printf("TX ID = %x\n", txID);
		}
	}
	cout << endl << "------------------------------------------";

}

void PrintConfig()
{

	unsigned int i;
	char str[XL_MAX_LENGTH + 1] = "";

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

int hexadecimalToDecimal(char hexVal[])
{

	int len = strlen(hexVal);
	int base = 1;
	int dec_val = 0;

	for (int i = len-1; i >= 0; i--)
	{
		if (hexVal[i] > '0' && hexVal[i] <='9')
		{
			dec_val += (hexVal[i] - 48)*base;
			base = base * 16;
		}
		else if (hexVal[i] >= 'A' && hexVal[i] <='F')
		{
			dec_val += (hexVal[i] - 55)*base; 
			base = base * 16;
		}
	}
	return dec_val;
}

char* NCharacterFromInput(int cnt)
{
	char *input = new char[cnt + 2];
	cin.getline(input, cnt + 1);
	cin.clear();
	fflush(stdin);
	return input;
}

bool ExtractNum(char* s, char* arr)
{
	int len = 0;
	len = strlen(s);
	int  j = 0;
	bool ok = true;


	for (int i = 0; i < len; i++)
	{
		if (s[i] == '0' && s[i + 1] == 'x' || s [i + 1] == 'X')
		{
			i += 2;
		}
		else if( i < 3)
			ok = false;
		if (s[i] >= '0' && s[i] <= '9')
		{
			arr[j] = s[i];
			j++;
		}
		if (s[i] >= 'A' && s[i] <= 'F')
		{
			arr[j] = s[i];
			j++;
		}
		if (s[i] >= 'a' && s[i] <= 'f')
		{
			arr[j] = s[i];
			j++;
		}
	}

	return ok;
}

void choice_serv(int num)
{
	switch (num)
	{
	case 25:
		cout << "Izabrali ste Citanje DTC- a" << endl;
		break;
	case 20:
		cout << "Izabrali servis za Ciscenje DTC- a " << endl;
		break;
	case 34:
		cout << "Izabrali ste servis za citanje podataka na osnovu ID- a" << endl;
		break;
	case 46:
		cout << "Izabrali ste servis za pisanje podataka na osnovu ID- a" << endl;
		break;
	case 62:
		cout << "Izabrali ste servis za Tester Present" << endl;
		break;
	case 17:
		cout << "Izabrali ste servis za promenu dijagnosticke sesije " << endl;
		break;
	case 16:
		cout << "Izabrali ste servis za reset ECU- a" << endl;
		break;

	default:
		cout << "Izabrali ste servis koji ne podrzava nasa aplikacija !" << endl;
	}

}