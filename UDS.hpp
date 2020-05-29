#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;
class UDS
{

private:

    unsigned short ServicIdentifier;  //(SID) (0x00 - 0x3E) Obavezno polje
    unsigned short SubFunction; // Opciono polje
    unsigned short DataIdentifier; // Opciono polje u zavisnosti od servica
	unsigned int DataRec;	// Opciono polje
	bool PositiveResponse;
	bool NegativResponse;

public:

    UDS()
    {
		ServicIdentifier = 0;
        SubFunction = 0;
		DataIdentifier = 0;
		DataRec = 0;
		PositiveResponse = false;
		NegativResponse = false;
        cout << "\t\nUspesno ste inicijalizovali UDS objekat" << endl;
    }
    unsigned int get_ServicIdentifier()
    {
        return ServicIdentifier;
    }

    unsigned int get_SubFunction()
    {
        return SubFunction;
    }

    unsigned int get_DataIdentifier()
    {
        return DataIdentifier;
    }

	bool get_PSR()
	{
		return PositiveResponse;
	}

	bool get_NRC()
	{
		return NegativResponse;
	}

    void set_ServicIdentifier(unsigned int SID);
    void set_SubFunction(unsigned int SbF);
    void set_DataIdentifier(unsigned int DID);

};

void print_UDS(UDS* obj1);

