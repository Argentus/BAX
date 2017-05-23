#include "BAXi.h"
#include "BAXi_lexer.h"

#include <stdio.h>

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

char isHex(char a) {
	if ( (a >= '0' && a <= '9') || (a >= 'a' && a <= 'f') || (a >= 'A' && a <= 'F') )
		return 1;
	return 0;
}

char isBin(char a) {
	if ( a == '1' || a == '0' )
		return 1;
	return 0;
}

BAX_TOKEN BAX_get_command(char * source, uint8_t * i)
{
	uint8_t j = *i;
	BAX_TOKEN ret = BAX_NONE;

	switch ( source[j] ) {
		case 'd':
		case 'D':
			if (source[j+1] == 'O' || source[j+1] == 'o') {
				if (source[j+2] == 'N' || source[j+2] == 'n') {
					ret = BAX_CMD_DON;
					*i = j + 3;
				} else {
					ret = BAX_CMD_DO;
					*i = j + 2;
				}
			}
			break;
		case 'e':
		case 'E':
			if ((source[j+1] == 'L' || source[j+1] == 'l') && (source[j+2] == 'S' || source[j+2] == 's')) {
				ret = BAX_CMD_ELS;
				*i = j + 3;
			} else if ((source[j+1] == 'X' || source[j+1] == 'x') && (source[j+2] == 'E' || source[j+2] == 'e')) {
				ret = BAX_CMD_EXE;
				*i = j + 3;
			}
			break;
		case 'i':
		case 'I':
			if (source[j+1] == 'F' || source[j+1] == 'f') {
				ret = BAX_CMD_IF;
				*i = j + 2;
			} else if (source[j+1] == 'N' || source[j+1] == 'n') {
				if (source[j+2] == 'N' || source[j+1] == 'n') {
					ret = BAX_CMD_INN;
					*i = j + 3;
				} else {
					ret = BAX_CMD_IN;
					*i = j + 2;
				}
			}
			break;
		case 'j':
		case 'J':
			if ((source[j+1] == 'M' || source[j+1] == 'm') && (source[j+2] == 'P' || source[j+2] == 'p')) {
				ret = BAX_CMD_JMP;
				*i = j + 3;
			}
			break;
		case 'l':
		case 'L':
			if ((source[j+1] == 'E' || source[j+1] == 'e') && (source[j+2] == 'T' || source[j+2] == 't')) {
				ret = BAX_CMD_LET;
				*i = j + 3;
			}
			break;
		case 'o':
		case 'O':
			if ((source[j+1] == 'P' || source[j+1] == 'p') && (source[j+2] == 'N' || source[j+2] == 'n')) {
				ret = BAX_CMD_OPN;
				*i = j + 3;
			} else if ((source[j+1] == 'U' || source[j+1] == 'u') && (source[j+2] == 'T' || source[j+2] == 't')) {
				ret = BAX_CMD_OUT;
				*i = j + 3;
			}
			break;
		case 'p':
		case 'P':
			if ((source[j+1] == 'R' || source[j+1] == 'r') && (source[j+2] == 'T' || source[j+2] == 't')) {
				ret = BAX_CMD_PRT;
				*i = j + 3;
			}
			break;
		case 'r':
		case 'R':
			if (source[j+1] == 'D' || source[j+1] == 'd') {
				if (source[j+2] == 'N' || source[j+2] == 'n') {
					ret = BAX_CMD_RDN;
					*i = j + 3;
				} else {
					ret = BAX_CMD_RD;
					*i = j + 2;
				}
			} else if ((source[j+1] == 'E' || source[j+1] == 'e') && (source[j+2] == 'Q' || source[j+2] == 'q')) {
				ret = BAX_CMD_REQ;
				*i = j + 3;
			} else if ((source[j+1] == 'T' || source[j+1] == 't') && (source[j+2] == 'N' || source[j+2] == 'n')) {
				ret = BAX_CMD_RTN;
				*i = j + 3;
			}
			break;
		case 's':
		case 'S':
			if ((source[j+1] == 'L' || source[j+1] == 'l') && (source[j+2] == 'P' || source[j+2] == 'p')) {
				ret = BAX_CMD_SLP;
				*i = j + 3;
			}
			if ((source[j+1] == 'U' || source[j+1] == 'u') && (source[j+2] == 'B' || source[j+2] == 'b')) {
				ret = BAX_CMD_SUB;
				*i = j + 3;
			}
			break;
		case 't':
		case 'T':
			if (source[j+1] == 'O' || source[j+1] == 'o') {
				if (source[j+2] == 'N' || source[j+2] == 'n') {
					ret = BAX_CMD_TON;
					*i = j + 3;
				} else if (source[j+2] == 'S' || source[j+2] == 's') {
					ret = BAX_CMD_TOS;
					*i = j + 3;
				}
			}
			break;
		case 'v':
		case 'V':
			if (source[j+1] == 'A' || source[j+1] == 'a') {
				if (source[j+2] == 'L' || source[j+2] == 'l') {
					ret = BAX_CMD_VAL;
					*i = j + 3;
				} else if (source[j+2] == 'R' || source[j+2] == 'r') {
					ret = BAX_CMD_VAR;
					*i = j + 3;
				}
			}
			break;
		case 'x':
		case 'X':
			if ((source[j+1] == 'I' || source[j+1] == 'i') && (source[j+2] == 'T' || source[j+2] == 't')) {
				ret = BAX_CMD_XIT;
				*i = j + 3;
			}
			break;
		default:
			break;
	}
	return ret;
}

// Lexer main function
BAX_src_token BAX_next_token(char * source, uint8_t * i)
{
	uint8_t j = *i;
	BAX_src_token tok;

	while (isWhitespace(source[j])) {
		j++;
	}
	const uint8_t token_start = j;

	if (isAlpha(source[j])) {
		// var or cmd
		if ( isAlpha(source[j+1]) ) {
			//cmd
			tok.type = BAX_get_command(source, &j);
			if (tok.type <= BAX_CMD_NON || tok.type >= BAX_CMD_LAST)
				tok.type = BAX_NONE;
		} else {
			//var
			if (isLower(source[j]))
				tok.type = BAX_VAR_A + source[j] - 'a';
			else
				tok.type = BAX_VAR_A + source[j] - 'A';
			*i = ++j;
		}
	} else if (isNum(source[j])) {
		if (source[j] == '0' && source[j+1] == 'x') {
			// Hex constant
			j += 2;
			while (isHex(source[j]))
				++j;
			if (j > (token_start + 2))
				tok.type = BAX_SRC_HEX_CONST;

			
		} else
		if (source[j] == '0' && source[j+1] == 'b') {
			// Binary constant
			j += 2;
			while (isBin(source[j]))
				++j;
			if (j > (token_start + 2))
				tok.type = BAX_SRC_BIN_CONST;

		} else {
			// Decimal constant
			++j;
			while (isNum(source[j]))
				++j;
			tok.type = BAX_SRC_DEC_CONST;
			
		}
	} else {
		// Symbols
		switch (source[j]) {
			case '+':
				tok.type = BAX_M_PLUS;
				break;
			case '-':
				tok.type = BAX_M_MINUS;
				break;
			case '*':
				tok.type = BAX_M_MUL;
				break;
			case '/':
				tok.type = BAX_M_DIV;
				break;
			case '%':
				tok.type = BAX_M_MOD;
				break;
			case '&':
				if (source[j+1] == '&') {
					tok.type = BAX_L_AND;
					++j;
				} else {
					tok.type = BAX_M_AND;
				}
				break;
			case '|':
				if (source[j+1] == '|') {
					tok.type = BAX_L_OR;
					++j;
				} else {
					tok.type = BAX_M_OR;
				}
				break;
			case '^':
				if (source[j+1] == '^') {
					tok.type = BAX_L_XOR;
					++j;
				} else {
					tok.type = BAX_M_XOR;
				}
				break;
			case '=':
				tok.type = BAX_C_EQUAL;
				break;
			case '<':
				tok.type = BAX_C_LESS;
				break;
			case '>':
				tok.type = BAX_C_MORE;
				break;
			case '!':
				if (source[j+1] == '=') {
					tok.type = BAX_C_NEQUAL;
					++j;
				} else {
					tok.type = BAX_L_NOT;
				}
				break;
			case '(':
				tok.type = BAX_SRC_LPAR;
				break;
			case ')':
				tok.type = BAX_SRC_RPAR;
				break;
			case '[':
				tok.type = BAX_SRC_LSQPAR;
				break;
			case ']':
				tok.type = BAX_SRC_RSQPAR;
				break;
			case '\'':
				++j;
				if (source[j] == '\\')
					++j;
				++j;
				if (source[j] == '\'')
					tok.type = BAX_CHAR;
				else
					tok.type = BAX_NONE;
				break;
			case '"':
				++j;
				while (source[j] != '"' && source[j] != '\0')
					++j;
				if (source[j] == '"')
					tok.type = BAX_STRING;
				else
					tok.type = BAX_NONE;
				break;
			case '#':
				while (source[j] != '\0' && source[j] != '\n')
					++j;
				tok.type = BAX_SRC_COMMENT;
				break;
			case ':':
				while (source[j] != '\0' && source[j] != '\n' && !isWhitespace(source[j]))
					++j;
				tok.type = BAX_SRC_LABEL;
				break;
			default:
				tok.type = BAX_NONE;
				break;
		}
		++j;
	}
	
	if (tok.type != BAX_NONE)
		*i = j;

	tok.string = source + token_start;
	tok.len = j - token_start;
	return tok;
}

int8_t BAX_tokenize(char * source, BAX_src_token * tokenized) {
	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t tokc = 0;

	do {
		tokenized[j++] = BAX_next_token(source, &i);
		++tokc;
	} while (tokenized[j-1].type != BAX_NONE);

	return tokc;
}
