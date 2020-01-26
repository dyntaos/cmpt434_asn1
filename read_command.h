/**********************************
 **           CMPT 434           **
 **  University of Saskatchewan  **
 **         Assignment 1         **
 **----------------------------- **
 **          Kale Yuzik          **
 **     kay851@mail.usask.ca     **
 **      kay851    11071571      **
 **********************************/

#ifndef _READ_COMMAND_H
#define _READ_COMMAND_H

#include <kv_packet.h>


kv_message_command read_command(char **token1, char **token2);
void free_command(char **token1, char **token2);

#endif //_READ_COMMAND_H