#ifndef PARSE_H
#define PARSE_H

#include "../types.h"

/* Fills request and makes "s" point to request body
 * returns: SLICE_END, PREFIX_INVALID, VALUE_INVALID, NEWLINE_EXPECTED,
 * SPACE_EXPECTED
 */
shttp_status shttp_parse_request(shttp_request req[static 1],
                                 shttp_slice s[static 1]);

#endif
