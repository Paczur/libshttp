#ifndef PARSE_MACROS_H
#define PARSE_MACROS_H

#include "../macros.h"
#define SHTTP_CMP(m, t) shttp_parse_token_cmp((m), (t), sizeof(t) - 1)

#endif
