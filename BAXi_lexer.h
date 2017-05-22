
#ifndef BAXI_LEXER_H
#define BAXI_LEXER_H

#include <stdint.h>
#include "BAXi.h"

int8_t BAX_tokenize(char * source, BAX_src_token * tokenized);
BAX_src_token BAX_next_token(char * source, uint8_t * i);

#endif // BAXI_LEXER_H
