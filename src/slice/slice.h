#ifndef SHTTP_SLICE_H
#define SHTTP_SLICE_H

#include "../conf.h"
#include "../types.h"

// returns: SLICE_END, NEWLINE_EXPECTED
SHTTP_UNUSED_RESULT shttp_status
shttp_slice_parse_newline(shttp_slice s[static 1]);

// returns: SLICE_END, SPACE_EXPECTED
SHTTP_UNUSED_RESULT shttp_status
shttp_slice_parse_space(shttp_slice s[static 1]);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_slice_parse_space_optional(shttp_slice s[static 1]);

// returns: SLICE_END, VALUE_INVALID
SHTTP_UNUSED_RESULT shttp_status
shttp_slice_parse_number(shttp_u32 out[static 1], shttp_slice s[static 1]);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status shttp_slice_skip(shttp_slice s[static 1],
                                                  shttp_u16 x);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status shttp_slice_skip_until(shttp_slice s[static 1],
                                                        char mark);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_slice_skip_until_or(shttp_slice s[static 1], char marks[static 3]);

// returns: NEWLINE_EXPECTED
SHTTP_UNUSED_RESULT shttp_status
shttp_slice_skip_until_newline(shttp_slice s[static 1]);

// returns: SPACE_EXPECTED
SHTTP_UNUSED_RESULT shttp_status
shttp_slice_skip_until_space(shttp_slice s[static 1]);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status shttp_slice_skip_past(shttp_slice s[static 1],
                                                       char mark);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_slice_skip_past_or(shttp_slice s[static 1], char marks[static 3]);

// returns: NEWLINE_EXPECTED
SHTTP_UNUSED_RESULT shttp_status
shttp_slice_skip_past_newline(shttp_slice s[static 1]);

// returns: SPACE_EXPECTED
SHTTP_UNUSED_RESULT shttp_status
shttp_slice_skip_past_space(shttp_slice s[static 1]);

SHTTP_PURE SHTTP_UNUSED_RESULT bool shttp_slice_starts_with(shttp_slice s1,
                                                            shttp_slice s2);

SHTTP_PURE SHTTP_UNUSED_RESULT bool shttp_slice_eq(shttp_slice s1,
                                                   shttp_slice s2);

SHTTP_PURE SHTTP_UNUSED_RESULT bool shttp_slice_eq_until(shttp_slice s1,
                                                         shttp_slice s2,
                                                         char mark);

// returns: SLICE_END NEWLINE_EXPECTED
SHTTP_UNUSED_RESULT shttp_status shttp_header_next(shttp_slice out[static 1],
                                                   shttp_slice s[static 1]);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status shttp_header_key(shttp_slice out[static 1],
                                                  shttp_slice s[static 1]);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status shttp_header_value(shttp_slice out[static 1],
                                                    shttp_slice s[static 1]);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_slice_insert_newline(shttp_mut_slice msg[static 1]);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status shttp_slice_cpy(shttp_mut_slice msg[static 1],
                                                 shttp_slice slice);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_slice_cpy_char(shttp_mut_slice msg[static 1], char c);

#endif
