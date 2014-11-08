#ifndef C_INPUT_H_
#define C_INPUT_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

//user input
const char* input_string(char*, const int, const char* const);
int input_char(const char* const);
int input_bool(const char* const);
int input_number(const char* const, const int, const int);

/*
 *    I N P U T    F U N C T I O N S
 */
const char* input_string(char* out, const int max, const char* const prompt) {
	do {
		printf("%s> ", prompt);
		fflush(stdout);
		fgets(out, max, stdin);
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
	char tmp[128] = {0};
	int ret;
	do {
		input_string(tmp, 127, prompt);
		ret = tmp[0];
		if(!isgraph(ret)) {
			printf("Invalid Choice!\n");
			continue;
		}
		break;
	} while(1);
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
	char tmp[128] = {0};
	int ret;
	do {
		input_string(tmp, 127, prompt);
		ret = atoi(tmp);
		if(ret < min || ret > max) {
			printf("Invalid Number!\n");
			continue;
		}
		break;
	} while(1);
	return ret;
}
#endif //C_INPUT_H_
