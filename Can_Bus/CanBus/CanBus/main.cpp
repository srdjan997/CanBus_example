#include "common.h"

int main(int argc, char * argv[])
{


	XLstatus      xlStatus;
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
	int Di = 0;;
	int S_id = 0;
	int code = 0;

	argument_call(argc,argv);
	Sleep(300);

	Help();
	Sleep(700);

	xlStatus = InitDriver(&xlChanMaskTx, &xlChanIndex);
	printf("- Init: %s\n",xlGetErrorString(xlStatus));
	Sleep(700);
	cout << "\n----------------------------------------------------------\n";
	cout << "\tPrikaz mogucih dijagnostickih komandi: " << endl;
	Sleep(200);

	uds_print();

	cout << "Molim Vas unesite kod za Request Service Id: ";
	cin >> S_id;

	uds_choice(S_id);
	/*for (int i = 0; i < 15; i++)
	{
		cout << "*";
		Sleep(100);
	}
	cout << endl;
	*/
	return 0;
}