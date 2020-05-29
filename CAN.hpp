#include <iostream>
#include <stdio.h>
#include <string>
#include "types.h"

using namespace std;

class CanMessage
{
private:

    unsigned int arbitradion_id;
    unsigned int can_flags;
    unsigned int dlc;
    string message;
public:
    CanMessage()
    {
        cout << "\t\nUspesno ste inicijalizovali CANMessage objekat" << endl;
        arbitradion_id = 5;
        can_flags = 1;
        dlc = 8;
        message = "CAN Transmit message!";

    }

    void set_arbitradion_id(unsigned int ar_id);
    void set_can_flags(unsigned int cflgs);
    void set_dlc(unsigned int Dlc);
    void set_str(string s);
    unsigned int get_arbitration_id()
    {
        return arbitradion_id;
    }
    unsigned int get_can_flags()
    {
        return can_flags;
    }
    unsigned int get_dlc()
    {
        return dlc;
    }

    string get_str()
    {
        return message;
    }

};

void print_Can_msg(CanMessage* obj1);
