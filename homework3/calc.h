#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

void skip_whitespace(char **str);
char peek(char **str);
char parse_token(char **str);
int parse_num(char **str);
int digit(char **inputs);
double digits(char **inputs);
double sign(char **inputs);
double number(char **inputs);
double parenthesis(char **inputs);
double multiplicative(char **inputs);
double additive(char **inputs);
// int main(int argc, char **argv);
