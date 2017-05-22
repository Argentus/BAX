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
#include "BAXi_lexer.h"

#include <stdio.h>

/* -----------------
 * function: BAX_translate_factor
 * -----------------
 * Translates a factor from BAX source
 * into BAX byte code and stores it
 * in supplied buffer. Returns number
 * of bytes generated.
 * ---
 */
int8_t BAX_translate_factor (BAX_src_token * sourceLine, uint8_t * byteCode, uint8_t * i) {
	fflush(stdout);

	uint8_t j = 0;
	int16_t num = 0;
	int8_t sign = 1;
	if (sourceLine[*i].type == BAX_M_MINUS) {
		sign = -1;
		++(*i);
	}
	if (sourceLine[*i].type >= BAX_SRC_HEX_CONST && sourceLine[*i].type <= BAX_SRC_CHAR_CONST) {
		// PARSE CONSTANTS: HEX DEC BIN CHAR
		if (sourceLine[*i].type == BAX_SRC_HEX_CONST) {
			// HEX
			for (j = 2; j < sourceLine[*i].len; j++) {
				num *= 16;
				if (sourceLine[*i].string[j] >= '0' && sourceLine[*i].string[j] <= '9')
					num += (sourceLine[*i].string[j] - '0');
				else
					switch (sourceLine[*i].string[j]) {
						case 'a':
						case 'A':
							num += 10;
							break;
						case 'b':
						case 'B':
							num += 11;
							break;
						case 'c':
						case 'C':
							num += 12;
							break;
						case 'd':
						case 'D':
							num += 13;
							break;
						case 'e':
						case 'E':
							num += 14;
							break;
						case 'f':
						case 'F':
							num += 15;
							break;
						default:
							return -1;
					}
			}
		} else if (sourceLine[*i].string[0] == '0' && sourceLine[*i].string[1] == 'b') {
			// BIN
			for (j = 2; j < sourceLine[*i].len; j++) {
				num *= 2;
				switch (sourceLine[*i].string[j]) {
					case '0':
						break;
					case '1':
						++num;
						break;
					default:
						return -1;
				}

			}
		} else if (sourceLine[*i].string[0] == '\'') {
			// CHAR
			j = 1;
			if (sourceLine[*i].string[1] == '\\') {
				j = 2;
			}
		} else {
			// DEC
			for (j = 0; j < sourceLine[*i].len; j++) {
				num *= 10;
				if (sourceLine[*i].string[j] >= '0' && sourceLine[*i].string[j] <= '9')
					num += (sourceLine[*i].string[j] - '0');
				else
					return -1;
			}

		}
		++(*i);
	} else if (sourceLine[*i].type >= BAX_VAR_A && sourceLine[*i].type <= BAX_VAR_DOT) {
		if (sign == -1) {
			byteCode[0] = (uint8_t) BAX_M_NEGATIVE;
			byteCode[1] = (uint8_t) sourceLine[*i].type;
			++(*i);
			return 2;
		} else {
			byteCode[0] = (uint8_t) sourceLine[*i].type;
			++(*i);
			return 1;
		}
	} else
		return -1;

	byteCode[0] = (uint8_t) BAX_INT16;
	((uint16_t *)(byteCode + 1))[0] = (int16_t) num * sign;
	return 3;

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
int8_t BAX_translate_term (BAX_src_token * sourceLine, uint8_t * byteCode, uint8_t * i) {
	fflush(stdout);
	// term: <factor> { <math-operator-multiplicative> <term> }?
	//       ( <expression> )

	uint8_t j;
	uint8_t tempBC[BAX_EXPR_MAX_BYTES];
	uint8_t tempBC_len = 0;
	uint8_t par = 0;	// Need to match parenthesis?

	if (sourceLine[*i].type == BAX_NONE)
		return -1;

	int8_t ret;

	// Is this an expression in parentheses?
	if (sourceLine[*i].type == BAX_SRC_LPAR) {
		par = 1;
		++(*i);
		ret = BAX_translate_expression (sourceLine, tempBC + 1, i);
		if (ret <= 0)
			return ret;
		else
			tempBC_len += ret;
	} else {
		// Translate left factor
		ret = BAX_translate_factor (sourceLine, tempBC + 1, i);
		if (ret <= 0)
			return ret;
		else
			tempBC_len += ret;
	}

	if (par) { 	// Match parenthesis if needed
		if (sourceLine[*i].type == BAX_SRC_RPAR) {
			par = 0;
			++(*i);
		} else
			return -1;
	}

	// Is it not a multiplicative operator?
	if (	sourceLine[*i].type != BAX_M_MUL &&
		sourceLine[*i].type != BAX_M_DIV &&
		sourceLine[*i].type != BAX_M_MOD &&
		sourceLine[*i].type != BAX_M_AND &&
		sourceLine[*i].type != BAX_M_OR &&
		sourceLine[*i].type != BAX_M_XOR ) {
		
		// Simple one-factor term
		for (j = 0; j < tempBC_len; j++)
			byteCode[j] = tempBC[j + 1];
		return tempBC_len;
	}

	// Two-factor expression

	tempBC[0] = (uint8_t) sourceLine[*i].type;
	++(*i);
	++tempBC_len;

	// Translate right expression
	ret = BAX_translate_term (sourceLine, tempBC + tempBC_len, i);
	if (ret <= 0)
		return ret;
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
int8_t BAX_translate_expression (BAX_src_token * sourceLine, uint8_t * byteCode, uint8_t * i) {
	fflush(stdout);
	// expression: <term> { <math-operator-additive> <expression> }?
	uint8_t j;
	uint8_t tempBC[BAX_EXPR_MAX_BYTES];
	uint8_t tempBC_len = 0;

	int8_t ret;

	if (sourceLine[*i].type == BAX_NONE)
		return -1;

	// Translate left term
	ret = BAX_translate_term (sourceLine, tempBC+1, i);
	if (ret <= 0)
		return ret;
	else
		tempBC_len += ret;

	// Get operator
	if (sourceLine[*i].type != BAX_M_PLUS && sourceLine[*i].type != BAX_M_MINUS) {
		
		// Simple one-term expression
		for (j = 0; j < tempBC_len; j++)
			byteCode[j] = tempBC[j+1];
		return tempBC_len;
	}

	tempBC[0] = (uint8_t) sourceLine[*i].type;
	++(*i);
	++tempBC_len;

	// Translate right expression
	ret = BAX_translate_expression (sourceLine, tempBC + tempBC_len, i);

	if (ret <= 0)
		return ret;
	else
		tempBC_len += ret;

	// Copy BC buffer into main and return
	for (j = 0; j < tempBC_len; j++)
		byteCode[j] = tempBC[j];

	return tempBC_len;
}

// LOGICAL

/* -----------------
 * function: BAX_translate_log_expression
 * -----------------
 * Translates a logical expression from BAX source
 * into BAX byte code and stores it
 * in supplied buffer. Returns number
 * of bytes generated.
 * ---
 */
int8_t BAX_translate_log_expression (BAX_src_token * sourceLine, uint8_t * byteCode, uint8_t * i) {
	// log-expression: <log-term> { <log-operator> <log-expression> }?
	uint8_t j;
	uint8_t tempBC[BAX_EXPR_MAX_BYTES];
	uint8_t tempBC_len = 0;

	if (sourceLine[*i].type == BAX_NONE)
		return -1;

	int8_t ret;

	// Translate left term
	ret = BAX_translate_log_term (sourceLine, tempBC+1, i);
	if (ret <= 0)
		return ret;
	else
		tempBC_len += ret;

	// Get operator
	if (	sourceLine[*i].type != BAX_L_AND &&
		sourceLine[*i].type != BAX_L_OR &&
		sourceLine[*i].type != BAX_C_EQUAL &&
		sourceLine[*i].type != BAX_L_XOR ) {
		
		// Simple one-term logical expression
		for (j = 0; j < tempBC_len; j++)
			byteCode[j] = tempBC[j+1];
		return tempBC_len;
	}
	switch (sourceLine[*i].type) {
		case BAX_L_AND:
			tempBC[0] = (uint8_t) BAX_L_AND;
			break;
		case BAX_L_OR:
			tempBC[0] = (uint8_t) BAX_L_OR;
			break;
		case BAX_C_EQUAL:
			tempBC[0] = (uint8_t) BAX_L_EQUIV;
			break;
		case BAX_L_XOR:
			tempBC[0] = (uint8_t) BAX_L_XOR;
			break;
		default:
			return -1;
	}
	++(*i);
	++tempBC_len;

	// Translate right term
	ret = BAX_translate_log_expression (sourceLine, tempBC + tempBC_len, i);

	if (ret <= 0)
		return ret;
	else
		tempBC_len += ret;

	// Copy BC buffer into main and return
	for (j = 0; j < tempBC_len; j++)
		byteCode[j] = tempBC[j];

	return tempBC_len;
}

/* -----------------
 * function: BAX_translate_log_term
 * -----------------
 * Translates a logical term from BAX source
 * into BAX byte code and stores it
 * in supplied buffer. Returns number
 * of bytes generated.
 * ---
 */
int8_t BAX_translate_log_term (BAX_src_token * sourceLine, uint8_t * byteCode, uint8_t * i) {
	// log-term: <expression> <math-operator-comparative> <expression>
	//       ( <log-expression> )
	//	 <NOT-operator> <log-term>

	uint8_t j;
	uint8_t tempBC[BAX_EXPR_MAX_BYTES];
	uint8_t tempBC_len = 0;
	uint8_t par = 0;	// Need to match parenthesis?

	if (sourceLine[*i].type == BAX_NONE)
		return -1;

	int8_t ret;

	// Is this negated?
	if (sourceLine[*i].type == BAX_L_NOT) {
		tempBC[0] = (uint8_t) BAX_L_NOT;
		++tempBC_len;
		++(*i);
	}

	// Is this a log-expression in parentheses?
	if (sourceLine[*i].type == BAX_SRC_LPAR) {
		par = 1;
		++(*i);
		ret = BAX_translate_log_expression (sourceLine, tempBC + 1, i);
		if (ret <= 0)
			return ret;
		else
			tempBC_len += ret;
	} else {
		// Translate left factor
		ret = BAX_translate_expression (sourceLine, tempBC + 1, i);
		if (ret < 0)
			return ret;
		else if (ret == 0)
			return -1;
		else
			tempBC_len += ret;
	}

	if (par) { 	// Match parenthesis if needed
		if (sourceLine[*i].type == BAX_SRC_RPAR) {
			par = 0;
			++(*i);
		} else
			return -1;
	}

	// Is it not a comparative operator?
	if (	sourceLine[*i].type != BAX_C_EQUAL &&
		sourceLine[*i].type != BAX_C_LESS &&
		sourceLine[*i].type != BAX_C_MORE &&
		sourceLine[*i].type != BAX_C_NEQUAL) {
		
		return -1;
	}

	tempBC[0] = (uint8_t) sourceLine[*i].type;
	++(*i);
	++tempBC_len;

	// Translate right expression
	ret = BAX_translate_expression (sourceLine, tempBC + tempBC_len, i);
	if (ret <= 0)
		return ret;
	else
		tempBC_len += ret;

	// Copy BC buffer into main and return
	for (j = 0; j < tempBC_len; j++)
		byteCode[j] = tempBC[j];

	return tempBC_len;
}


/* -----------------
 * function: BAX_translate_line
 * -----------------
 * Translates a line of tokenized BAX source
 * into BAX byte code and stores it
 * in supplied buffer. Returns number
 * of bytes generated.
 * ---
 */
int8_t BAX_translate_line (BAX_src_token * sourceLine, uint8_t * byteCode) {

	uint8_t i;
	int8_t ret = 0;

	uint8_t byteCode_buffer[256]; // Set to zero
	for (i = 0; i < 254; i++)
		byteCode_buffer[i] = (uint8_t) BAX_NONE;
	int8_t byteCode_len = 0;
	i = 0;

	
	switch (sourceLine[i].type) {
		case BAX_NONE:
			break;
		// Simple instructions			
		case BAX_CMD_DON:
		case BAX_CMD_DO:
		case BAX_CMD_ELS:
		case BAX_CMD_RTN:
		case BAX_CMD_XIT:
			byteCode_buffer[0] = (uint8_t) sourceLine[i].type;
			byteCode_len = 1;
			break;
		case BAX_CMD_EXE:
			byteCode_len = -2;
			break;
		case BAX_CMD_IF:
			// IF <log-expression>
			byteCode_buffer[0] = (uint8_t) BAX_CMD_IF;
			byteCode_len = 1;
			++i;
			// Get condition
			ret = BAX_translate_log_expression(sourceLine, byteCode_buffer + byteCode_len, &i);
			if (ret <= 0) {
				byteCode_len = ret;
				break;
			} else
				byteCode_len += ret;
			break;
		case BAX_CMD_INN:
			byteCode_len = -2;
			break;
		case BAX_CMD_IN:
			byteCode_len = -2;
			break;
		case BAX_CMD_JMP:
			byteCode_len = -2;
			break;
		case BAX_CMD_LET:
			// LET <variable> = <expression>
			byteCode_buffer[0] = (uint8_t) BAX_CMD_LET;
			byteCode_len = 1;
			++i;
			// Get var to be assigned
			if (sourceLine[i].type >= BAX_VAR_A && sourceLine[i].type <= BAX_VAR_DOT) {
				byteCode_buffer[1] = (uint8_t) sourceLine[i].type;
				++byteCode_len;
			} else {
				byteCode_len = -1;
				break;
			}
			++i;
			// Eat equals sign
			if (sourceLine[i].type != BAX_C_EQUAL) {
				byteCode_len = -1;
				break;
			}
			++i;
				
			// Get expression to assign
			ret = BAX_translate_expression(sourceLine, byteCode_buffer + byteCode_len, &i);
			if (ret <= 0) {
				byteCode_len = ret;
				break;
			} else
				byteCode_len += ret;
			break;
		case BAX_CMD_OPN:
			byteCode_len = -2;
			break;
		case BAX_CMD_OUT:
			byteCode_len = -2;
			break;
		case BAX_CMD_PRT:
			// PRT <expression>
			byteCode_buffer[0] = (uint8_t) BAX_CMD_PRT;
			byteCode_len = 1;
			++i;

			// Get expression to be printed
			ret = BAX_translate_expression(sourceLine, byteCode_buffer + byteCode_len, &i);
			if (ret <= 0) {
				byteCode_len = ret;
				break;
			} else
				byteCode_len += ret;
			break;
		case BAX_CMD_RDN:
			byteCode_len = -2;
			break;
		case BAX_CMD_RD:
			byteCode_len = -2;
			break;
		case BAX_CMD_REQ:
			byteCode_len = -2;
			break;
		case BAX_CMD_SUB:
			byteCode_len = -2;
			break;
		case BAX_CMD_TON:
			byteCode_len = -2;
			break;
		case BAX_CMD_TOS:
			byteCode_len = -2;
			break;
		case BAX_CMD_VAL:
			byteCode_len = -2;
			break;
		case BAX_CMD_VAR:
			byteCode_len = -2;
			break;
		default:
			return -1;
			break;
	}

	if (byteCode_len > 0) {
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
int16_t eval_cond(BAX_env * env, uint8_t * byteCode, uint8_t * i);
uint8_t interpret_line(BAX_env * env, uint8_t * byteCode, uint8_t * i);

int main () {
	uint8_t byteCode[256];
	char input[256];
	uint8_t i;
	int8_t ret;
	BAX_env env;
	
	do {
		for (i=0; i<254; i++)
			byteCode[i] = 0;

		printf("> ");
		i = -1;
		while (input[i++] != '\n') {
			input[i] = getchar();
		}
		input[i] = '\0';
		BAX_src_token tokens[256];
		int8_t tokc = BAX_tokenize(input, tokens);
		if (tokc < 0) {
			printf("Lexical error!\n");
			continue;
		}
		int8_t len = BAX_translate_line(tokens, byteCode);
		if (len < 0) {
			printf("Syntax error!\n");
			continue;
		} else if (len == 0) {
			continue;
		}

		uint8_t ind = 0;
		ret = interpret_line(&env, byteCode, &ind);
	} while (ret != -1);
	return 0;
}

uint8_t interpret_line(BAX_env * env, uint8_t * byteCode, uint8_t * i) {

	switch (byteCode[*i]) {
		case BAX_NONE:
			return -1;
		case BAX_CMD_IF:
			{
			*i += 1;
			int16_t result = eval_cond(env, byteCode, i);

			printf("%s\n", result == 1 ? "true" : "false");
			}
			break;

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

		case BAX_CMD_PRT:
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
	return 0;
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
			default:
				return -1;

		}
	}

}

int16_t eval_cond(BAX_env * env, uint8_t * byteCode, uint8_t * i) {

	if (byteCode[*i] >= BAX_C_EQUAL && byteCode[*i] <= BAX_C_NEQUAL) {
		BAX_TOKEN operator = byteCode[*i];
		++(*i);
		int16_t left = eval_expr(env, byteCode, i);
		int16_t right = eval_expr(env, byteCode, i);
		switch (operator) {
			case BAX_C_EQUAL:
				return (left == right) ? 1 : 0;
			case BAX_C_LESS:
				return (left < right) ? 1 : 0;
			case BAX_C_MORE:
				return (left > right) ? 1 : 0;
			case BAX_C_NEQUAL:
				return (left != right) ? 1 : 0;
			default:
				return -1;
		}
	} else if (byteCode[*i] >= BAX_L_AND && byteCode[*i] <= BAX_L_EQUIV) {
		BAX_TOKEN operator = byteCode[*i];
		++(*i);
		int16_t left = eval_cond(env, byteCode, i);
		int16_t right = eval_cond(env, byteCode, i);
		switch (operator) {
			case BAX_L_AND:
				return ((left == 1) && (right == 1)) ? 1 : 0;
			case BAX_L_OR:
				return ((left == 1) || (right == 1)) ? 1 : 0;
			case BAX_L_XOR:
				return ((left == 1) != (right == 1)) ? 1 : 0;
			case BAX_L_EQUIV:
				return ((left == 1) == (right == 1)) ? 1 : 0;
			default:
				return -1;
		}
	} else {
		return -1;
	}
}
