/**
 ** file BAXi.h
 ** ver: 0.9 of Mar 2017
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


#include <stdint.h>

/*
 * Configuration
 */
#define BAX_EXPR_MAX_BYTES 64	// Max. bytes per expression
#define MAX_SUBS 32 		// Max. subroutine call depth

/*
 * Type definition
 */

typedef struct BAX_env {

	// Program code
	uint16_t byteCode_length;
	uint8_t * program_byteCode;

	// Variables A-Z and . (return value)
	uint16_t vars[27];

	// Memory
	uint8_t * memory;
	uint16_t memory_length;

	// Instruction pointer
	uint16_t IP;

	// Return addresses
	uint16_t retAddr[MAX_SUBS];
#if MAX_SUBS>256
	uint16_t subCallNo;
#else
	uint8_t subCallNo;
#endif
} BAX_env;	// Running environment for the interpreted program

typedef enum {
	// Nothing
	BAX_NONE = 0,

	// Commands
	BAX_CMD_LST = 1,
	BAX_CMD_CAL,
	BAX_CMD_LET,
	BAX_CMD_IF,
	BAX_CMD_JMP,
	BAX_CMD_SUB,
	BAX_CMD_RTN,
	BAX_CMD_OUT,
	BAX_CMD_IN,
	BAX_CMD_REQ,

	// Constants
	BAX_INT8 = 70,
	BAX_INT16,
	BAX_CHAR,
	BAX_TRUE,
	BAX_FALSE,
	BAX_STRING,

	// Variables
	BAX_VAR_A = 80,
	BAX_VAR_B,
	BAX_VAR_C,
	BAX_VAR_D,
	BAX_VAR_E,
	BAX_VAR_F,
	BAX_VAR_G,
	BAX_VAR_H,
	BAX_VAR_I,
	BAX_VAR_J,
	BAX_VAR_K,
	BAX_VAR_L,
	BAX_VAR_M,
	BAX_VAR_N,
	BAX_VAR_O,
	BAX_VAR_P,
	BAX_VAR_Q,
	BAX_VAR_R,
	BAX_VAR_S,
	BAX_VAR_T,
	BAX_VAR_U,
	BAX_VAR_V,
	BAX_VAR_W,
	BAX_VAR_X,
	BAX_VAR_Y,
	BAX_VAR_Z,
	BAX_VAR_DOT,

	// Memory
	BAX_MEM_8 = 120,
	BAX_MEM_16,

	// Math operators
	//    Additive
	BAX_M_PLUS = 130,
	BAX_M_MINUS,
	//    Multiplicative
	BAX_M_MUL = 140,
	BAX_M_DIV,
	BAX_M_MOD,
	BAX_M_AND,
	BAX_M_OR,
	BAX_M_XOR,

	// Comparison operators
	BAX_C_EQUAL = 150,
	BAX_C_LESS,
	BAX_C_MORE,
	BAX_C_NEQUAL,

	// Logical operators
	BAX_L_AND = 160,
	BAX_L_OR,
	BAX_L_XOR,
	BAX_L_EQUIV,
	BAX_L_NOT,

	// Symbols
	BAX_CHAR_LPAR = 220,
	BAX_CHAR_RPAR,

	// Program end
	BAX_END = 255
	
} BAX_TOKEN;

/*
 * Compiler fsunctions
 */
int8_t BAX_translate_var (char * sourceLine, uint8_t * byteCode, uint8_t * i);
int8_t BAX_translate_term (char * sourceLine, uint8_t * byteCode, uint8_t * i);
int8_t BAX_translate_expression (char * sourceLine, uint8_t * byteCode, uint8_t * i);
int8_t BAX_translate_factor (char * sourceLine, uint8_t * byteCode, uint8_t * i);
int8_t BAX_translate_condition (char * sourceLine, uint8_t * byteCode, uint8_t * i);
