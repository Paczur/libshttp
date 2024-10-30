#ifndef SHTTP_SLICE_H
#define SHTTP_SLICE_H

#include "../conf.h"
#include "../types.h"

SHTTP_CONST uint32_t shttp_slice_length(shttp_slice s);

SHTTP_CONST bool shttp_slice_empty(shttp_slice s);

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
SHTTP_UNUSED_RESULT shttp_status shttp_slice_parse_u32(uint32_t out[static 1],
                                                       shttp_slice s[static 1]);

// returns: SLICE_END, VALUE_INVALID
SHTTP_UNUSED_RESULT shttp_status shttp_slice_parse_u16(uint16_t out[static 1],
                                                       shttp_slice s[static 1]);

// returns: SLICE_END, VALUE_INVALID
SHTTP_UNUSED_RESULT shttp_status shttp_slice_parse_u8(uint8_t out[static 1],
                                                      shttp_slice s[static 1]);

// returns: SLICE_END, VALUE_INVALID
SHTTP_UNUSED_RESULT shttp_status shttp_slice_parse_i32(int32_t out[static 1],
                                                       shttp_slice s[static 1]);

// returns: SLICE_END, VALUE_INVALID
SHTTP_UNUSED_RESULT shttp_status shttp_slice_parse_i16(int16_t out[static 1],
                                                       shttp_slice s[static 1]);

// returns: SLICE_END, VALUE_INVALID
SHTTP_UNUSED_RESULT shttp_status shttp_slice_parse_i8(int8_t out[static 1],
                                                      shttp_slice s[static 1]);

/* On success moves s to the end of parsed text
 * returns: SLICE_END, VALUE_INVALID, FORMAT_INVALID
 */
SHTTP_UNUSED_RESULT shttp_status
shttp_slice_parse_format(shttp_slice s[static 1], shttp_slice format, ...);

// Same as shttp_slice_parse_format but allows to choose delimiters for specials
SHTTP_UNUSED_RESULT shttp_status shttp_slice_parse_format_delim(
  shttp_slice s[static 1], shttp_slice format, char open, char close, ...);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status shttp_slice_skip(shttp_slice s[static 1],
                                                  uint32_t x);

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
SHTTP_UNUSED_RESULT shttp_status
shttp_slice_insert_u32(shttp_mut_slice s[static 1], uint32_t val);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_slice_insert_u16(shttp_mut_slice s[static 1], uint16_t val);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_slice_insert_u8(shttp_mut_slice s[static 1], uint8_t val);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_slice_insert_i32(shttp_mut_slice s[static 1], int32_t val);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_slice_insert_i16(shttp_mut_slice s[static 1], int16_t val);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_slice_insert_i8(shttp_mut_slice s[static 1], int8_t val);

// returns: SLICE_END, FORMAT_INVALID
shttp_status shttp_slice_insert_format(shttp_mut_slice s[static 1],
                                       shttp_slice format, ...);

// Same as shttp_slice_insert_format but allows to choose delimiters for
// specials
shttp_status shttp_slice_insert_format_delim(shttp_mut_slice s[static 1],
                                             shttp_slice format, char open,
                                             char close, ...);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status shttp_slice_cpy(shttp_mut_slice msg[static 1],
                                                 shttp_slice slice);

// returns: SLICE_END
SHTTP_UNUSED_RESULT shttp_status
shttp_slice_cpy_char(shttp_mut_slice msg[static 1], char c);

#endif

