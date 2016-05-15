// // // // // #ifdef  __AVX__
// // // // //     mexPrintf("AVX is defined\n");
// // // // // #endif
// // // // // #ifdef  __AVX2__
// // // // //     mexPrintf("AVX2 is defined\n");
// // // // // #endif    
// // // // //     
// // // // //       /* Initialize the two argument vectors */
// // // // //   __m256 evens = _mm256_set_ps(2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0);
// // // // //   __m256 odds = _mm256_set_ps(1.0, 3.0, 5.0, 7.0, 9.0, 11.0, 13.0, 15.0);
// // // // // 
// // // // //   /* Compute the difference between the two vectors */
// // // // //   __m256 result = _mm256_sub_ps(evens, odds);
// // // // //     
// // // // //         //mexPrintf("AVX2 is defined\n");