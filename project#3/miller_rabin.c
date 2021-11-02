/*
 * Copyright 2020, 2021. Heekuck Oh, all rights reserved
 * 이 프로그램은 한양대학교 ERICA 소프트웨어학부 재학생을 위한 교육용으로 제작되었습니다.
 */
#include "miller_rabin.h"

/*
 * Miller-Rabin Primality Testing against small sets of bases
 *
 * if n < 2^64,
 * it is enough to test a = 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, and 37.
 *
 * if n < 3,317,044,064,679,887,385,961,981,
 * it is enough to test a = 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, and 41.
 */
const uint64_t a[ALEN] = {2,3,5,7,11,13,17,19,23,29,31,37};


// ai로 n이 합성수인지 테스트. 합성수이면 1, 소수이면 0 return
int isComposite(uint64_t ai, uint64_t q, uint64_t k, uint64_t n){
    uint64_t tmp = mod_pow(ai, q, n);
    if(tmp == 1 || tmp == n-1) return 0; // a^q % n == 1이면 소수일 확률이 큼

    for(uint64_t j=1; j<k; j++){
        tmp = mod_mul(tmp,tmp,n);
        if(tmp == n-1) return 0;
    }
    return 1;
}

/*
 * miller_rabin() - Miller-Rabin Primality Test (deterministic version)
 *
 * n > 3, an odd integer to be tested for primality
 * It returns 1 if n is prime, 0 otherwise.
 */
int miller_rabin(uint64_t n)
{
    if(n == 2) return 1;
    else if(n < 2 || n % 2 == 0) return 0;

    uint64_t k = 0, q = n-1;
    while(!(q & 1)){ // (n-1) = (2^k)q 인 k, q 찾기
        k++;
        q = q >> 1;
    }

    for(int i=0; (i<ALEN && a[i]<n-1); i++){
        if(isComposite(a[i], q, k, n)) return 0;
    }
    return 1;
}
