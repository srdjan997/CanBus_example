#include "UDS.hpp"

void UDS::set_ServicIdentifier(unsigned int sid)
{
    ServicIdentifier = sid;
}
void UDS::set_SubFunction(unsigned int sbf)
{
    SubFunction = sbf;
}
void UDS::set_DataIdentifier(unsigned int did)
{
    DataIdentifier = did;
}



void print_UDS(UDS* obj1)
{
    cout << "\tVase postavke za UDS: " << endl;
    cout << "\tService Identifier: " << obj1->get_ServicIdentifier() << endl;
    cout << "\tSub Funcition: " << obj1->get_SubFunction() << endl;
	cout << "\tData Identifier: " << obj1->get_DataIdentifier() << endl;
}
