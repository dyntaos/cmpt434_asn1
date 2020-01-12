#ifndef _READ_COMMAND_H
#define _READ_COMMAND_H

typedef enum command {invalid, add, get_value, get_all, remove_cmd, quit} command;

command read_command(char **token1, char **token2);
void free_command(char **token1, char **token2);

#endif //_READ_COMMAND_H