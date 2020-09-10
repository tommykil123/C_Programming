#include "calc.h"

void skip_whitespace(char **str) {
    while (isspace(*str[0])) {
        (*str)++;
    }
}

char peek(char **str) {
    skip_whitespace(str);
    return *str[0];
}

char parse_token(char **str) {
    skip_whitespace(str);
    char token = *str[0];
    if (!token) {
        fprintf(stderr, "parsing error. unexpected end of string\n");
        exit(1);
    }
    (*str)++;
    return token;
}

int parse_num(char **str) {
    char c_number = *str[0];
    if ((peek(str) >= '0' && peek(str) <= '9') && c_number == ' ') {
        c_number = peek(str);
        fprintf(stderr, "parsing number error. unexpected junk at end. got '%c'\n", c_number);
        exit(1);
    }
    if (c_number >= 65) {
        fprintf(stderr, "parsing error. expected number (0-9,+,-,.) got '%c'\n", c_number);
        exit(1);
    }
    if (c_number < 40 && c_number > 0 && c_number != ' ') {
        fprintf(stderr, "parsing error. expected number (0-9,+,-,.) got '%c'\n", c_number);
        exit(1);
    }

    if (c_number >= '0' && c_number <= '9') {
        (*str)++;
    }
    int number = (int)(c_number - '0');
    return number;
}

int digit(char **inputs) { // output -1 if not a digit
    int value = parse_num(inputs);
    if (value < 0 || value > 9) {
        value = -1;
    }
    return value;
}

int dot(char **inputs) {
    int value = 0;
    if (inputs[0][0] == '.') {
        value = 1;
        parse_token(inputs);
    }
    return value;
}

double digits(char **inputs) {
    int dig = 0;
    int b_dot = 0;
    int dec_idx = 0;
    double value = 0;
    while (1) {
        if (b_dot) {
            if (dot(inputs)) {
                fprintf(stderr, "dot error: another . after .\n");
                exit(1);
            }
        }
        if (!b_dot && dig >= 0) {
            value = value * 10 + dig;
            b_dot = dot(inputs);
            dig = digit(inputs);
        } else if (b_dot && dig >= 0) {
            value += dig * pow(10, --dec_idx);
            dig = digit(inputs);
        } else {
            break;
        }
    }
    return value;
}

double sign(char **inputs) {
    double value = 1.0;
    char c_value = peek(inputs);
    if (c_value == '-') {
        parse_token(inputs);
        value = -1.0;
    }
    return value;
}

double number(char **inputs) {
    double value = sign(inputs);
    value = value * digits(inputs);
    // printf("%f\n", value);
    return value;
}

double parenthesis(char **inputs) {
    double value;
    char peek_val = peek(inputs);
    // if (peek_val == '-' || peek_val == '.' || (peek_val >= '0' && peek_val <= '9')) {
    //     value = number(inputs);
    // }
    if (peek(inputs) == '(') {
        parse_token(inputs);
        value = additive(inputs);
        if (peek(inputs) == ')') {
            parse_token(inputs);
        } else {
            fprintf(stderr, "error. expected ')' but got '%c'\n", peek(inputs));
            exit(1);
        }
    } else {
        value = number(inputs);
    }
    return value;
}

double multiplicative(char **inputs) {
    double value = parenthesis(inputs);
    char next_char;
    while (peek(inputs) == '*' || peek(inputs) == '/') {
        next_char = parse_token(inputs);
        if (next_char == '*') {
            value *= parenthesis(inputs);
        } else {
            value /= parenthesis(inputs);
        }
        if (peek(inputs) == '\0') {
            break;
        }
    }
    return value;
}

double additive(char **inputs) {
    double value = multiplicative(inputs);
    char next_char;
    while (peek(inputs) == '+' || peek(inputs) == '-') {
        next_char = parse_token(inputs);
        if (next_char == '+') {
            value += multiplicative(inputs);
        } else {
            value -= multiplicative(inputs);
        }
        if (peek(inputs) == '\0') {
            break;
        }
    }
    return value;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <statement>\n", argv[0]);
        return 1;
    }

    if (argc == 3) {
        fprintf(stderr, "unexpected expressiong %s\n", argv[2]);
        return 1;
    }
    char *inputs = argv[1];
    double output = additive(&inputs);
    printf("%f\n", output);
    return 0;
}
