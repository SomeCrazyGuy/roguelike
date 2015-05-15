#ifndef C_INPUT_H_
#define C_INPUT_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

//user input
char* input_string(const char* const);
int input_char(const char* const);
int input_bool(const char* const);
int input_number(const char* const, const int, const int);

/*
 *    I N P U T    F U N C T I O N S
 */
char* input_string(const char* const prompt) {
    char *out = NULL;
    size_t n = 0;
	do {
		printf("%s> ", prompt);
		fflush(stdout);
        getline(&out, &n, stdin);
		if(out[0] == '\n') {
			printf("Invalid Input!\n");
			continue;
		}
		out[strlen(out) - 1] = '\0';
		break;
	} while(1);
	return out;
}

int input_char(const char* const prompt) {
	int ret;
    char *tmp;
	do {
		tmp = input_string(prompt);
		ret = tmp[0];
		if(!isgraph(ret)) {
			printf("Invalid Choice!\n");
			continue;
		}
		break;
	} while(1);
    free(tmp);
	return ret;
}

int input_bool(const char* const prompt) {
	int ret;
	do {
		ret = input_char(prompt);
		if (!(ret=='y'||ret=='n')) {
			printf("Invalid Selection!\n");
			continue;
		}
		break;
	} while(1);
	return (ret=='y');
}

int input_number(const char* const prompt, const int min, const int max) {
	char *tmp;
	int ret;
	do {
		tmp = input_string(prompt);
		ret = atoi(tmp);
		if(ret < min || ret > max) {
			printf("Invalid Number!\n");
			continue;
		}
		break;
	} while(1);
    free(tmp);
	return ret;
}
#endif //C_INPUT_H_
