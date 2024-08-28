#ifndef PARSE_H
#define PARSE_H

#include "../types.h"

// returns: SLICE_END, NEWLINE_EXPECTED
shttp_status shttp_parse_slice_newline(shttp_slice s[static 1]);

// returns: SLICE_END, SPACE_EXPECTED
shttp_status shttp_parse_slice_space(shttp_slice s[static 1]);

// returns: SLICE_END
shttp_status shttp_parse_slice_space_optional(shttp_slice s[static 1]);

// returns: SLICE_END, VALUE_INVALID
shttp_status shttp_parse_slice_number(shttp_u16 out[static 1],
                                      shttp_slice s[static 1]);

// returns: SLICE_END
shttp_status shttp_parse_slice_skip(shttp_slice s[static 1], shttp_u16 x);

// returns: SLICE_END
shttp_status shttp_parse_slice_skip_until(shttp_slice s[static 1], char mark);

// returns: SLICE_END
shttp_status shttp_parse_slice_skip_until_or(shttp_slice s[static 1],
                                             char marks[static 3]);

// returns: NEWLINE_EXPECTED
shttp_status shttp_parse_slice_skip_until_newline(shttp_slice s[static 1]);

// returns: SPACE_EXPECTED
shttp_status shttp_parse_slice_skip_until_space(shttp_slice s[static 1]);

// returns: SLICE_END
shttp_status shttp_parse_slice_skip_past(shttp_slice s[static 1], char mark);

// returns: SLICE_END
shttp_status shttp_parse_slice_skip_past_or(shttp_slice s[static 1],
                                            char marks[static 3]);

// returns: NEWLINE_EXPECTED
shttp_status shttp_parse_slice_skip_past_newline(shttp_slice s[static 1]);

// returns: SPACE_EXPECTED
shttp_status shttp_parse_slice_skip_past_space(shttp_slice s[static 1]);

// Slices cannot point to overlapping memory
bool shttp_parse_slice_cmp(shttp_slice s1, shttp_slice s2);

// Slices cannot point to overlapping memory
bool shttp_parse_slice_cmp_until(shttp_slice s1, shttp_slice s2, char mark);

// returns: SLICE_END NEWLINE_EXPECTED
shttp_status shttp_parse_header_next(shttp_slice out[static 1],
                                     shttp_slice s[static 1]);

// returns: SLICE_END
shttp_status shttp_parse_header_key(shttp_slice out[static 1],
                                    shttp_slice s[static 1]);

// returns: SLICE_END
shttp_status shttp_parse_header_value(shttp_slice out[static 1],
                                      shttp_slice s[static 1]);

/* Fills request and makes "s" point to request body
 * returns: SLICE_END, PREFIX_INVALID, VALUE_INVALID, NEWLINE_EXPECTED,
 * SPACE_EXPECTED
 */
shttp_status shttp_parse_request(shttp_request req[static 1],
                                 shttp_slice s[static 1]);

#endif
