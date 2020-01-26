/**********************************
 **           CMPT 434           **
 **  University of Saskatchewan  **
 **         Assignment 1         **
 **----------------------------- **
 **          Kale Yuzik          **
 **     kay851@mail.usask.ca     **
 **      kay851    11071571      **
 **********************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <ctype.h>
#include <stdint.h>

#include <read_command.h>


kv_message_command read_command(char **token1, char **token2) {
	char *input, *token;
	kv_message_command cmd = invalid;
	uint16_t token_count, token_len;

	input = readline(">> ");

	token = strtok(input, " ");
	token_count = 0;
	free_command(token1, token2);

	while (token != NULL) {
		token_count++;
		token_len = strlen(token);

		for (int i = 0; i < token_len; i++) {
			token[i] = tolower(token[i]);
		}

		switch (token_count) {
			case 1:
				if (strcmp("add", token) == 0) {
					cmd = add;
				} else if (strcmp("getvalue", token) == 0) {
					cmd = get_value;
				} else if (strcmp("getall", token) == 0) {
					cmd = get_all;
				} else if (strcmp("remove", token) == 0) {
					cmd = remove_cmd;
				} else if (strcmp("quit", token) == 0) {
					cmd = quit;
				} else {
					return invalid;
				}
				break;
			case 2:
				*token1 = (char*) malloc(token_len + 1);
				strcpy(*token1, token);
				break;
			case 3:
				*token2 = (char*) malloc(token_len + 1);
				strcpy(*token2, token);
				break;
			default:
				free_command(token1, token2);
				return invalid;
				break;
		}

		token = strtok(NULL, " ");
	}

	free(input);
	return cmd;
}

void free_command(char **token1, char **token2) {
	free(*token1);
	free(*token2);
	*token1 = NULL;
	*token2 = NULL;
}