%{
Use of SIMD:
- ADVANCE_TO_NON_WHITESPACE_CHAR
   - _mm_cmpistri - SSE4.2

- INIT_LOCAL_WS_CHARS
   - _mm_set1_epi32 - SSE2

- string_to_double_no_math - NO_SIMD allowed
   - _mm_cmpistri - SSE4.2



%}