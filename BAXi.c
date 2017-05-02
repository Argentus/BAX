/**
 ** file BAXi.c
 ** ver: 0.9 of Jan 2017
 **
 ** BAX Programming language interpreter
 ** by author: Radovan Bezak
 ** radobezak@gmail.com
 ** 2016
 **
 ** I, the author, release this software, to all
 ** possible extent, into the public domain.
 **
 **/

#include "BAXi.h"

typedef struct BAX_token_str {
	char * str;
	int8_t len;
} BAX_token_str;

// Char classification helper functions
char isWhitespace(char a) {
	if ( a == ' ' || a == '\t' )
		return 1;
	return 0;
}

char isLower(char a) {
	if (a >= 'a' && a <= 'z')
		return 1;
	return 0;

}

char isUpper(char a) {
	if (a >= 'A' && a <= 'Z')
		return 1;
	return 0;

}

char isAlpha(char a) {
	if ( isLower(a) || isUpper(a) )
		return 1;
	return 0;
}

char isNum(char a) {
	if ( a >= '0' && a <= '9' )
		return 1;
	return 0;
}

BAX_TOKEN BAX_next_token2 (char * source, uint8_t * i) {
	
	uint8_t token_done = 0;
	uint8_t offset = 0;
	int8_t len = 0;
	enum BAX_tokenizer_state {
		BAX_TS_NONE,
		BAX_TS_VAR,
		BAX_TS_CMD,
		BAX_TS_ZERO,
		BAX_TS_DEC,
		BAX_TS_HEX,
		BAX_TS_CHAR,
		BAX_TS_MEM,
		
	} state;

	while (!token_done) {
		switch (state) {
			case BAX_TS_NONE:
				if ( isWhitespace(source[*i]) ) {
					++i;
				} else if ( isAlpha(source[*i]) ) {
					state = BAX_TS_VAR;
					++len;
					++i;
				}
				break;
			case BAX_TS_VAR:

				break;
			case BAX_TS_CMD:

				break;
			
		}
	}
}

BAX_token_str BAX_next_token (char * source, uint8_t * i) {
	
	uint8_t offset = 0;
	int8_t len = 0;

	// Skip whitespace
	while (source[*i] == ' ' || source[*i] == '\t' || source[*i] == '\n') {
		++(*i);
	}
	
	offset = *i;
	if (source[*i] == '\0')
		return (BAX_token_str) {source + offset, (int8_t) 0};

	
	while (source[*i] != ' ' && source[*i] != '\t' && source[*i] != '\0' && source[*i] != '\n') {
		++(*i);
		++len;
	}

	return (BAX_token_str) {source + offset, len};

}

BAX_TOKEN BAX_get_CMD (BAX_token_str token) {
	switch (token.str[0]) {

		case 'c':
		case 'C':
			return BAX_CMD_CAL;

		case 'i':
		case 'I':
			switch (token.str[1]) {
				case 'f':
				case 'F':
					return BAX_CMD_IF;
				case 'n':
				case 'N':
					return BAX_CMD_IN;
			}
			break;

		case 'j':
		case 'J':
			return BAX_CMD_JMP;

		case 'l':
		case 'L':
			switch (token.str[1]) {
				case 'e':
				case 'E':
					return BAX_CMD_LET;
				case 's':
				case 'S':
					return BAX_CMD_LST;
			}
			break;

		case 'o':
		case 'O':
			return BAX_CMD_OUT;

		case 'r':
		case 'R':
			return BAX_CMD_RTN;

		case 's':
		case 'S':
			return BAX_CMD_SUB;

		
	}
	return BAX_NONE;
}

int8_t BAX_eat_equals_sign (char * sourceLine, uint8_t * i) {
	BAX_token_str nextTok = BAX_next_token(sourceLine, i);
	if (nextTok.len == 1 && nextTok.str[0] == '=')
		return 1;
	else {
		i -= nextTok.len;
		return 0;
	}
}

/* -----------------
 * function: BAX_translate_var
 * -----------------
 * Translates a var from BAX source
 * into BAX byte code and stores it
 * in supplied buffer. Returns number
 * of bytes generated (1 or 0).
 * ---
 */
int8_t BAX_translate_var (char * sourceLine, uint8_t * byteCode, uint8_t * i) {

	BAX_token_str nextTok = BAX_next_token(sourceLine, i);
	if (nextTok.len == 0)
		return 0;
	*i += nextTok.len;
	
	if (nextTok.len == 1) {
		if (nextTok.str[0] == '.')
			byteCode[0] = (uint8_t) BAX_VAR_DOT;
		else if (isLower(nextTok.str[0]))
			byteCode[0] = (uint8_t) BAX_VAR_A + nextTok.str[0] - 'a';
		else if (isUpper(nextTok.str[0]))
			byteCode[0] = (uint8_t) BAX_VAR_A + nextTok.str[0] - 'A';
		else
			// Invalid var
			return 0;
		return 1;

	} else {
		// Nothing yet - maybe memory locations later
		return 0;
	}
}



/* -----------------
 * function: BAX_translate_factor
 * -----------------
 * Translates a term from BAX source
 * into BAX byte code and stores it
 * in supplied buffer. Returns number
 * of bytes generated.
 * ---
 */
int8_t BAX_translate_factor (char * sourceLine, uint8_t * byteCode, uint8_t * i) {


	// For now, only decimal number
	uint8_t j = 0;
	BAX_token_str nextTok;
	int16_t num = 0;
	int8_t sign = 1;

	nextTok = BAX_next_token(sourceLine, i);
	if (nextTok.len == 0)
		return -1;

	// Is this a var?
	if (nextTok.str[0] >= 'A' && nextTok.str[0] <= 'Z') {
		byteCode[0] = BAX_VAR_A + nextTok.str[0] - 'A';
		return 1;
	}

	if (nextTok.str[0] == '-') {
		sign = -1;
		++j;
	}

	for (/* j */; j < nextTok.len; j++) {
		if (isNum(nextTok.str[j])) {
			num = num * 10 + nextTok.str[j] - '0';
		} else {
			// Return i to start of this token
			*i -= nextTok.len;
			return -1;
		}
	}

	byteCode[0] = (uint8_t) BAX_INT16;
	((uint16_t *)(byteCode + 1))[0] = (int16_t) num * sign;
	return 3;

}

/* -----------------
 * function: BAX_translate_logical_term
 * -----------------
 * Translates a logical term from BAX source
 * into BAX byte code and stores it
 * in supplied buffer. Returns number
 * of bytes generated.
 * ---
 */
int8_t BAX_translate_logical_term (char * sourceLine, uint8_t * byteCode, uint8_t * i) {
	// log_term: <expression> <comp-operator> <expression>
	//           ( <condition> )
	//	     ! <log_term>

	uint8_t j;
	uint8_t tempBC[BAX_EXPR_MAX_BYTES];
	uint8_t tempBC_len = 0;
	uint8_t par = 0;	// Need to match parenthesis?
	BAX_TOKEN operator;
	BAX_token_str nextTok;

	uint8_t ret;

	// Is this a condition in parentheses?
	nextTok = BAX_next_token(sourceLine, i);
	if (nextTok.len == 1 && nextTok.str[0] == '(') {

		par = 1;
		ret = BAX_translate_condition (sourceLine, tempBC, i);
		if (ret <= 0)
			return -1;
		else
			tempBC_len += ret;

		// Match parenthesis
		nextTok = BAX_next_token(sourceLine, i);
		if (nextTok.len == 1 && nextTok.str[0] == ')') {
			// Copy BC buffer into main and return
			for (j = 0; j < tempBC_len; j++)
				byteCode[j] = tempBC[j];
			return tempBC_len;
		} else {
			*i -= nextTok.len;
			return -1;
		}

	// Is this a negated term?
	} else if (nextTok.len == 1 && nextTok.str[0] == '!') {
		tempBC[0] = (uint8_t) BAX_L_NOT;
		++ tempBC_len;
		ret = BAX_translate_logical_term(sourceLine, tempBC + 1, i);
		if (ret <= 0)
			return -1;

		tempBC_len += ret;
		// Copy BC buffer into main and return
		for (j = 0; j < tempBC_len; j++)
			byteCode[j] = tempBC[j];
		return tempBC_len;

	}

	// Translate left expression
	ret = BAX_translate_expression (sourceLine, tempBC + 1, i);
	if (ret <= 0)
		return -1;
	else
		tempBC_len += ret;

	// Get operator
	nextTok = BAX_next_token(sourceLine, i);


	// Is it not a comparation operator?
	if (nextTok.str[0] != '=' &&
	    nextTok.str[0] != '>' &&
	    nextTok.str[0] != '<' &&
	    nextTok.str[0] != '!' ) {
	
		// Return i to start of this token
		*i -= nextTok.len;
		return -1;
	}

	// Two-factor expression

	switch (nextTok.str[0]) {
		case '*':
			operator = BAX_M_MUL;
			break;
		case '/':
			operator = BAX_M_DIV;
			break;
		case '%':
			operator = BAX_M_MOD;
			break;
		case '&':
			operator = BAX_M_AND;
			break;
		case '|':
			operator = BAX_M_OR;
			break;
		case '^':
			operator = BAX_M_XOR;
			break;
	}
	tempBC[0] = (uint8_t) operator;
	++ tempBC_len;

	// Translate right expression
	ret = BAX_translate_term (sourceLine, tempBC + tempBC_len, i);
	if (ret <= 0)
		return -1;
	else
		tempBC_len += ret;

	// Copy BC buffer into main and return
	for (j = 0; j < tempBC_len; j++)
		byteCode[j] = tempBC[j];

	return tempBC_len;

}

/* -----------------
 * function: BAX_translate_term
 * -----------------
 * Translates a term from BAX source
 * into BAX byte code and stores it
 * in supplied buffer. Returns number
 * of bytes generated.
 * ---
 */
int8_t BAX_translate_term (char * sourceLine, uint8_t * byteCode, uint8_t * i) {
	// term: <factor> { <math-operator-multiplicative> <term> }?
	//       ( <expression> )

	uint8_t j;
	uint8_t tempBC[BAX_EXPR_MAX_BYTES];
	uint8_t tempBC_len = 0;
	uint8_t par = 0;	// Need to match parenthesis?
	BAX_TOKEN operator;
	BAX_token_str nextTok;

	uint8_t ret;

	// Is this an expression in parentheses?
	nextTok = BAX_next_token(sourceLine, i);
	if (nextTok.len == 1 && nextTok.str[0] == '(') {

		par = 1;
		ret = BAX_translate_expression (sourceLine, tempBC + 1, i);
		if (ret <= 0)
			return -1;
		else
			tempBC_len += ret;

	} else {

		*i -= nextTok.len;

		// Translate left factor
		ret = BAX_translate_factor (sourceLine, tempBC + 1, i);
		if (ret <= 0)
			return -1;
		else
			tempBC_len += ret;

	}

	// Get parenthesis/operator
	nextTok = BAX_next_token(sourceLine, i);

	if (par) { 	// Match parenthesis if needed
		if (nextTok.len == 1 && nextTok.str[0] == ')') {
			par = 0;
			nextTok = BAX_next_token(sourceLine, i);
		} else {
			*i -= nextTok.len;
			return -1;
		}
	}

	// Is it not a multiplicative operator?
	if (nextTok.len != 1 || (nextTok.str[0] != '*' &&
				 nextTok.str[0] != '/' &&
				 nextTok.str[0] != '%' &&
				 nextTok.str[0] != '&' &&
				 nextTok.str[0] != '|' &&
				 nextTok.str[0] != '^' )) {
		
		// Simple one-factor term
		for (j = 0; j < tempBC_len; j++)
			byteCode[j] = tempBC[j + 1];

		// Return i to start of this token
		*i -= nextTok.len;
		return tempBC_len;
	}

	// Two-factor expression

	switch (nextTok.str[0]) {
		case '*':
			operator = BAX_M_MUL;
			break;
		case '/':
			operator = BAX_M_DIV;
			break;
		case '%':
			operator = BAX_M_MOD;
			break;
		case '&':
			operator = BAX_M_AND;
			break;
		case '|':
			operator = BAX_M_OR;
			break;
		case '^':
			operator = BAX_M_XOR;
			break;
	}
	tempBC[0] = (uint8_t) operator;
	++ tempBC_len;

	// Translate right expression
	ret = BAX_translate_term (sourceLine, tempBC + tempBC_len, i);
	if (ret <= 0)
		return -1;
	else
		tempBC_len += ret;

	// Copy BC buffer into main and return
	for (j = 0; j < tempBC_len; j++)
		byteCode[j] = tempBC[j];

	return tempBC_len;

}

/* -----------------
 * function: BAX_translate_condition
 * -----------------
 * Translates a condition from BAX source
 * into BAX byte code and stores it
 * in supplied buffer. Returns number
 * of bytes generated.
 * ---
 */
int8_t BAX_translate_condition (char * sourceLine, uint8_t * byteCode, uint8_t * i) {
	// condition: <log_term> { <log-operator> <condition> }?

	uint8_t j;
	uint8_t tempBC[BAX_EXPR_MAX_BYTES];
	uint8_t tempBC_len = 0;
	uint8_t rightBC_len = 0;
	BAX_TOKEN operator;

	uint8_t ret;

	// Translate left term
	ret = BAX_translate_logical_term (sourceLine, tempBC+1, i);
	if (ret <= 0)
		return -1;
	else
		tempBC_len += ret;

	// Get operator
	BAX_token_str nextTok = BAX_next_token(sourceLine, i);
	if (nextTok.len != 1 || ( nextTok.str[0] != '&' &&
				  nextTok.str[0] != '|' &&
				  nextTok.str[0] != '^' &&
				  nextTok.str[0] != '=')) {
		
		// Simple one-term condition
		for (j = 0; j < tempBC_len; j++)
			byteCode[j] = tempBC[j+1];

		// Return i to start of this token
		*i -= nextTok.len;
		return tempBC_len;
	}

	// Two-term expression

	switch (nextTok.str[0]) {
		case '&':
			operator = BAX_L_AND;
			break;
		case '|':
			operator = BAX_L_OR;
			break;
		case '^':
			operator = BAX_L_XOR;
			break;
		case '=':
			operator = BAX_L_EQUIV;
			break;
	}
	tempBC[0] = (uint8_t) operator;
	++tempBC_len;

	// Translate right expression
	ret = BAX_translate_condition(sourceLine, tempBC + tempBC_len, i);
	if (ret <= 0)
		return -1;
	else
		tempBC_len += ret;

	// Copy BC buffer into main and return
	for (j = 0; j < tempBC_len; j++)
		byteCode[j] = tempBC[j];

	return tempBC_len;

}

/* -----------------
 * function: BAX_translate_expression
 * -----------------
 * Translates an expression from BAX source
 * into BAX byte code and stores it
 * in supplied buffer. Returns number
 * of bytes generated.
 * ---
 */
int8_t BAX_translate_expression (char * sourceLine, uint8_t * byteCode, uint8_t * i) {
	// expression: <term> { <math-operator-additive> <expression> }?

	uint8_t j;
	uint8_t tempBC[BAX_EXPR_MAX_BYTES];
	uint8_t tempBC_len = 0;
	uint8_t rightBC_len = 0;
	BAX_TOKEN operator;

	uint8_t ret;

	// Translate left term
	ret = BAX_translate_term (sourceLine, tempBC+1, i);
	if (ret <= 0)
		return -1;
	else
		tempBC_len += ret;

	// Get operator
	BAX_token_str nextTok = BAX_next_token(sourceLine, i);
	if (nextTok.len != 1 || (nextTok.str[0] != '+' && nextTok.str[0] != '-')) {
		
		// Simple one-term expression
		for (j = 0; j < tempBC_len; j++)
			byteCode[j] = tempBC[j+1];

		// Return i to start of this token
		*i -= nextTok.len;
		return tempBC_len;
	}

	// Two-term expression

	switch (nextTok.str[0]) {
		case '+':
			operator = BAX_M_PLUS;
			break;
		case '-':
			operator = BAX_M_MINUS;
			break;
	}
	tempBC[0] = (uint8_t) operator;
	++tempBC_len;

	// Translate right expression
	ret = BAX_translate_expression (sourceLine, tempBC + tempBC_len, i);
	if (ret <= 0)
		return -1;
	else
		tempBC_len += ret;

	// Copy BC buffer into main and return

	// Copy left operand
	for (j = 0; j < tempBC_len; j++)
		byteCode[j] = tempBC[j];

	return tempBC_len;

}

/* -----------------
 * function: BAX_translate_line
 * -----------------
 * Translates a line of BAX source
 * into BAX byte code and stores it
 * in supplied buffer. Returns number
 * of bytes generated.
 * ---
 */
int8_t BAX_translate_line (char * sourceLine, uint8_t * byteCode) {

	uint8_t i;
	uint16_t line_number = 0;
	uint8_t ret = 0;

	uint8_t byteCode_buffer[256]; // Set to zero
	for (i = 0; i < 255; i++)
		byteCode_buffer[i] = (uint8_t) BAX_NONE;
	uint8_t byteCode_len = 0;
	
	i = 0;

	// Get line number if present
	while (sourceLine[i] >= '0' && sourceLine[i] <= '9') {
		line_number *= 10;
		line_number += sourceLine[i] - '0';
		++i;
	}

	BAX_token_str nextTok;
	nextTok = BAX_next_token(sourceLine, &i);
	if (nextTok.len != 0) {
		// Get command
		BAX_TOKEN cmd = BAX_get_CMD(nextTok);

		switch (cmd) {
			case BAX_NONE:
				return -1;

			case BAX_CMD_LET:
				// LET <variable> = <expression>
				byteCode_buffer[0] = (uint8_t) BAX_CMD_LET;
				byteCode_len = 1;
				// Get var to be assigned
				ret = BAX_translate_var(sourceLine, byteCode_buffer+1, &i);
				if (ret <= 0)
					return -1;
				else
					byteCode_len += ret;
				// Eat equals sign
				if (!BAX_eat_equals_sign(sourceLine, &i))
					return -1;
				
				// Get expression to assign
				ret = BAX_translate_expression(sourceLine, byteCode_buffer + byteCode_len, &i);
				if (ret <= 0)
					return -1;
				else
					byteCode_len += ret;
				break;

			case BAX_CMD_LST:
				return -1;
				break;

			case BAX_CMD_IF:
				return -1;
				break;

			case BAX_CMD_JMP:
				return -1;
				break;

			case BAX_CMD_SUB:
				return -1;
				break;

			case BAX_CMD_RTN:
				return -1;
				break;

			case BAX_CMD_OUT:
				// OUT <variable>
				byteCode_buffer[0] = (uint8_t) BAX_CMD_OUT;
				byteCode_len = 1;
				// Get var to be printed
				ret = BAX_translate_var(sourceLine, byteCode_buffer+1, &i);
				if (ret <= 0)
					return -1;
				else
					byteCode_len += ret;
				break;

			case BAX_CMD_IN:
				return -1;
				break;

			case BAX_CMD_CAL:
				return -1;
				break;

			case BAX_CMD_REQ:
				return -1;
				break;
		}
	} else {
		// No command given
		byteCode_len = 0;
	}

	if (byteCode_len != 0) {
		// Copy buffered byte code into main
		for (i = 0; i < byteCode_len; i++)
			byteCode[i] = byteCode_buffer[i];
	}

	return byteCode_len;
}


/*
 * Testing
 */
#include <stdio.h>

int16_t eval_expr(BAX_env * env, uint8_t * byteCode, uint8_t * i);
uint8_t interpret_line(BAX_env * env, uint8_t * byteCode, uint8_t * i);

int main () {
	uint8_t byteCode[256];
	char input[256];
	uint8_t i = -1;
	uint8_t ret;
	BAX_env env;
	
	do {
		for (i=0; i<255; i++)
			byteCode[i] = 0;

		printf("> ");
		i = -1;
		while (input[i++] != '\n') {
			input[i] = getchar();
		}
		input[i] = '\0';

		i = 0;

		int8_t len = 0; 
		len = BAX_translate_line(input, byteCode);
		if (len < 0) {
			printf("Syntax error!\n");
			continue;
		}

		uint8_t ind = 0;
		ret = interpret_line(&env, byteCode, &ind);
	} while (ret != -1);
}

uint8_t interpret_line(BAX_env * env, uint8_t * byteCode, uint8_t * i) {

	switch (byteCode[*i]) {
		case BAX_NONE:
			return -1;

		case BAX_CMD_LET:
			{
			BAX_TOKEN var = (BAX_TOKEN) byteCode[*i+1];
			if (var < BAX_VAR_A || var > BAX_VAR_Z)
				return -1;
			*i += 2;
			int16_t result = eval_expr(env, byteCode, i);

			env->vars[var - BAX_VAR_A] = result;
			}
			break;

		case BAX_CMD_OUT:
			{
			BAX_TOKEN var = (BAX_TOKEN) byteCode[*i+1];
			if (var < BAX_VAR_A || var > BAX_VAR_Z)
				return -1;
			*i += 2;

			printf("%i\n", env->vars[var - BAX_VAR_A]);
			}
			break;

		default:
			return -1;
	}
}

int16_t eval_expr(BAX_env * env, uint8_t * byteCode, uint8_t * i) {

	if (byteCode[*i] == BAX_INT16) {
		*i += 3;
		return ((int16_t *)(byteCode+(*i)-2))[0];
	} else if (byteCode[*i] == BAX_INT8) {
		*i += 2;
		return byteCode[(*i)-1];
	} else if (byteCode[*i] >= BAX_VAR_A && byteCode[*i] <= BAX_VAR_Z) {
		*i += 1;
		return env->vars[byteCode[(*i)-1] - BAX_VAR_A];
	} else {
		BAX_TOKEN operator = byteCode[*i];
		++(*i);
		int16_t left = eval_expr(env, byteCode, i);
		int16_t right = eval_expr(env, byteCode, i);
		switch (operator) {
			case BAX_M_PLUS:
				return left + right;
			case BAX_M_MINUS:
				return left - right;
			case BAX_M_MUL:
				return left * right;
			case BAX_M_DIV:
				return left / right;
			case BAX_M_MOD:
				return left % right;
			case BAX_M_AND:
				return left & right;
			case BAX_M_OR:
				return left | right;
			case BAX_M_XOR:
				return left ^ right;

		}
	}

}
