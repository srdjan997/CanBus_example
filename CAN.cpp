#include "CAN.hpp"
#include <string.h>

void CanMessage::set_arbitradion_id(unsigned int ar_id)
{
    arbitradion_id = ar_id;
}
void CanMessage::set_can_flags(unsigned int cflgs)
{
    can_flags = cflgs;
}
void CanMessage::set_dlc(unsigned int Dlc)
{
    dlc = Dlc;
}

void CanMessage::set_str(string s)
{
    message = "";
    message.append(s);
}

void print_Can_msg(CanMessage* obj1)
{
    cout << "\tVasa poruka se sastoji od: "<< endl;
    cout << "\tArbitrazni broj: " << obj1->get_arbitration_id()<< endl;
    cout << "\tCAN flag: " << obj1->get_can_flags() << endl;
    cout << "\tDLC: " << obj1->get_dlc() << endl;
    cout << "\tPoruke koju saljte: " << obj1->get_str() << endl;

}
