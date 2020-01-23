#ifndef _READ_COMMAND_H
#define _READ_COMMAND_H

#include <kv_packet.h>


kv_message_command read_command(char **token1, char **token2);
void free_command(char **token1, char **token2);

#endif //_READ_COMMAND_H