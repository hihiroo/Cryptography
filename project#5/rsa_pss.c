/*
 * Copyright 2020,2021. Heekuck Oh, all rights reserved
 * 이 프로그램은 한양대학교 ERICA 소프트웨어학부 재학생을 위한 교육용으로 제작되었습니다.
 */
#include <stdlib.h>
#include <string.h>
#include <bsd/stdlib.h>
#include <gmp.h>
#include "rsa_pss.h"

#if defined(SHA224)
void (*sha)(const unsigned char *, unsigned int, unsigned char *) = sha224;
#elif defined(SHA256)
void (*sha)(const unsigned char *, unsigned int, unsigned char *) = sha256;
#elif defined(SHA384)
void (*sha)(const unsigned char *, unsigned int, unsigned char *) = sha384;
#else
void (*sha)(const unsigned char *, unsigned int, unsigned char *) = sha512;
#endif

/*
 * Copyright 2020, 2021. Heekuck Oh, all rights reserved
 * rsa_generate_key() - generates RSA keys e, d and n in octet strings.
 * If mode = 0, then e = 65537 is used. Otherwise e will be randomly selected.
 * Carmichael's totient function Lambda(n) is used.
 */
void rsa_generate_key(void *_e, void *_d, void *_n, int mode)
{
    mpz_t p, q, lambda, e, d, n, gcd;
    gmp_randstate_t state;
    
    /*
     * Initialize mpz variables
     */
    mpz_inits(p, q, lambda, e, d, n, gcd, NULL);
    gmp_randinit_default(state);
    gmp_randseed_ui(state, arc4random());
    /*
     * Generate prime p and q such that 2^(RSAKEYSIZE-1) <= p*q < 2^RSAKEYSIZE
     */
    do {
        do {
            mpz_urandomb(p, state, RSAKEYSIZE/2);
            mpz_setbit(p, 0);
            mpz_setbit(p, RSAKEYSIZE/2-1);
       } while (mpz_probab_prime_p(p, 50) == 0);
        do {
            mpz_urandomb(q, state, RSAKEYSIZE/2);
            mpz_setbit(q, 0);
            mpz_setbit(q, RSAKEYSIZE/2-1);
        } while (mpz_probab_prime_p(q, 50) == 0);
        mpz_mul(n, p, q);
    } while (!mpz_tstbit(n, RSAKEYSIZE-1));
    /*
     * Generate e and d using Lambda(n)
     */
    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_lcm(lambda, p, q);
    if (mode == 0)
        mpz_set_ui(e, 65537);
    else do {
        mpz_urandomb(e, state, RSAKEYSIZE);
        mpz_gcd(gcd, e, lambda);
    } while (mpz_cmp(e, lambda) >= 0 || mpz_cmp_ui(gcd, 1) != 0);
    mpz_invert(d, e, lambda);
    /*
     * Convert mpz_t values into octet strings
     */
    mpz_export(_e, NULL, 1, RSAKEYSIZE/8, 1, 0, e);
    mpz_export(_d, NULL, 1, RSAKEYSIZE/8, 1, 0, d);
    mpz_export(_n, NULL, 1, RSAKEYSIZE/8, 1, 0, n);
    /*
     * Free the space occupied by mpz variables
     */
    mpz_clears(p, q, lambda, e, d, n, gcd, NULL);
}

/*
 * Copyright 2020. Heekuck Oh, all rights reserved
 * rsa_cipher() - compute m^k mod n
 * If m >= n then returns EM_MSG_OUT_OF_RANGE, otherwise returns 0 for success.
 */
static int rsa_cipher(void *_m, const void *_k, const void *_n)
{
    mpz_t m, k, n;
    
    /*
     * Initialize mpz variables
     */
    mpz_inits(m, k, n, NULL);
    /*
     * Convert big-endian octets into mpz_t values
     */
    mpz_import(m, RSAKEYSIZE/8, 1, 1, 1, 0, _m);
    mpz_import(k, RSAKEYSIZE/8, 1, 1, 1, 0, _k);
    mpz_import(n, RSAKEYSIZE/8, 1, 1, 1, 0, _n);
    /*
     * Compute m^k mod n
     */
    if (mpz_cmp(m, n) >= 0) {
        mpz_clears(m, k, n, NULL);
        return EM_MSG_OUT_OF_RANGE;
    }
    mpz_powm(m, m, k, n);
    /*
     * Convert mpz_t m into the octet string _m
     */
    mpz_export(_m, NULL, 1, RSAKEYSIZE/8, 1, 0, m);
    /*
     * Free the space occupied by mpz variables
     */
    mpz_clears(m, k, n, NULL);
    return 0;
}

/*
 * Copyright 2020. Heekuck Oh, all rights reserved
 * A mask generation function based on a hash function
 */
static unsigned char *mgf(const unsigned char *mgfSeed, size_t seedLen, unsigned char *mask, size_t maskLen)
{
    uint32_t i, count, c;
    size_t hLen;
    unsigned char *mgfIn, *m;
    
    /*
     * Check if maskLen > 2^32*hLen
     */
    hLen = SHASIZE/8;
    if (maskLen > 0x0100000000*hLen)
        return NULL;
    /*
     * Generate octet string mask
     */
    if ((mgfIn = (unsigned char *)malloc(seedLen+4)) == NULL)
        return NULL;;
    memcpy(mgfIn, mgfSeed, seedLen);
    count = maskLen/hLen + (maskLen%hLen ? 1 : 0);
    if ((m = (unsigned char *)malloc(count*hLen)) == NULL)
        return NULL;
    /*
     * Convert i to an octet string C of length 4 octets
     * Concatenate the hash of the seed mgfSeed and C to the octet string T:
     *       T = T || Hash(mgfSeed || C)
     */
    for (i = 0; i < count; i++) {
        c = i;
        mgfIn[seedLen+3] = c & 0x000000ff; c >>= 8;
        mgfIn[seedLen+2] = c & 0x000000ff; c >>= 8;
        mgfIn[seedLen+1] = c & 0x000000ff; c >>= 8;
        mgfIn[seedLen] = c & 0x000000ff;
        (*sha)(mgfIn, seedLen+4, m+i*hLen);
    }
    /*
     * Copy the mask and free memory
     */
    memcpy(mask, m, maskLen);
    free(mgfIn); free(m);
    return mask;
}

/*
 * rsassa_pss_sign - RSA Signature Scheme with Appendix
 길이가 mLen 바이트인 메시지 m을 개인키 (d,n)으로 서명한 결과를 s에 저장한다.
 성공하면 0, 실패하면 오류 코드를 넘겨준다. s의 크기는 RSAKEYSIZE와 같아야 한다.
 */
int rsassa_pss_sign(const void *m, size_t mLen, const void *d, const void *n, void *s)
{
    // 해시 함수의 입력 데이터 길이가 한도 초과
    // m은 항상 2^64 비트보다 크지 않으므로 256이하에서만 확인
    if(SHASIZE <= 256 && mLen > 0x1fffffffffffffff) return EM_MSG_TOO_LONG; // mLen이 2^64비트 즉, 2^61바이트보다 크면 안됨 

    //mHash 생성
    unsigned char mHash[SHASIZE/8];
    sha(m, mLen, mHash);

    //난수 salt 생성
    unsigned salt[SHASIZE/8]; // salt의 길이는 해시 길이와 같음
    arc4random_buf(salt, SHASIZE/8);

    // 0x00 8바이트와 mHash, salt를 Prime으로 합쳐줌
    unsigned char mPrime[8+2*SHASIZE/8]; // mPrime은 8바이트(64비트) + 해시 사이즈*2
    memset(mPrime, 0x00, 8);
    memcpy(mPrime+8, mHash, SHASIZE/8);
    memcpy(mPrime+8+SHASIZE/8, salt, SHASIZE/8);

    // mPrime을 다시 해시함수 통과시켜 H생성
    unsigned char H[SHASIZE/8];
    sha(mPrime, 8+2*SHASIZE/8, H);

    //DB 생성
    int DB_SIZE = RSAKEYSIZE/8 - SHASIZE/8 - 1;
    unsigned char DB[DB_SIZE];
    
    memset(DB, 0, DB_SIZE - SHASIZE/8 -1); // ps
    DB[DB_SIZE-SHASIZE/8-1] = 0x01; // 0x01
    memcpy(DB + DB_SIZE-SHASIZE/8, salt, SHASIZE/8); // salt

    // H를 MGF 통과시켜 마스크 생성
    unsigned char H_mgf[DB_SIZE];
    mgf(H, SHASIZE/8, H_mgf, DB_SIZE);

    // H_mgf와 DB XOR 연산하여 maskedDB생성
    unsigned char maskedDB[DB_SIZE];
    for(int i=0; i<DB_SIZE; i++){
        maskedDB[i] = DB[i] ^ H_mgf[i];
    }
    
    // H의 길이가 EM에 수용 가능 길이보다 큰지 확인
    if(DB_SIZE + SHASIZE/8 + 1 > RSAKEYSIZE/8) return EM_HASH_TOO_LONG;

    //EM 생성
    unsigned char EM[RSAKEYSIZE/8];
    memcpy(EM, maskedDB, DB_SIZE);
    memcpy(EM+DB_SIZE, H, SHASIZE/8);
    EM[RSAKEYSIZE/8-1] = 0xbc;

    // EM의 첫 비트가 1이면 0으로 바꿔줌
    if((EM[0]>>7) & 1) EM[0] = 0x00;

    // 키 사용하여 암호화
    if(rsa_cipher(EM, d, n) == EM_MSG_OUT_OF_RANGE)
        return EM_MSG_OUT_OF_RANGE;
    memcpy(s, EM, RSAKEYSIZE/8);
    
    return 0;
}

/*
 * rsassa_pss_verify - RSA Signature Scheme with Appendix
 */
int rsassa_pss_verify(const void *m, size_t mLen, const void *e, const void *n, const void *s)
{
    unsigned char EM[RSAKEYSIZE/8];
    memcpy(EM, s, RSAKEYSIZE/8);

    // 키 사용하여 복호화
    if(rsa_cipher(EM, e, n) == EM_MSG_OUT_OF_RANGE)
        return EM_MSG_OUT_OF_RANGE;

    // 오류 검증
    if(EM[RSAKEYSIZE/8-1] ^ 0xbc) return EM_INVALID_LAST;
    if((EM[0] >> 7) & 1) return EM_INVALID_INIT;
    
    // maskedDB 추출
    int DB_SIZE = RSAKEYSIZE/8 - SHASIZE/8 - 1;
    unsigned char maskedDB[DB_SIZE];
    memcpy(maskedDB, EM, DB_SIZE);

    // H 추출
    unsigned char H[SHASIZE/8];
    memcpy(H, EM+DB_SIZE, SHASIZE/8);

    // H_mgf 복원
    unsigned char H_mgf[DB_SIZE];
    mgf(H, SHASIZE/8, H_mgf, DB_SIZE);

    // DB 복원
    unsigned char DB[DB_SIZE];
    DB[0] = 0x00;
    for(int i=1; i<DB_SIZE; i++){
        DB[i] = maskedDB[i] ^ H_mgf[i];
    }

    // salt 복원
    unsigned char salt[SHASIZE/8];
    memcpy(salt, DB+DB_SIZE-SHASIZE/8, SHASIZE/8);

    // DB 앞 부분이 0x0000...00||0x01인지 확인
    if(DB[DB_SIZE-SHASIZE/8-1] ^ 0x01) return EM_INVALID_PD2;
    for(int i=0; i<DB_SIZE - SHASIZE/8 - 1; i++){
        if(DB[i] ^ 0x00) return EM_INVALID_PD2;
    }

    // 주어진 m으로 mHash 생성
    unsigned char mHash[SHASIZE/8];
    sha(m, mLen, mHash);

    // mPrime 생성
    unsigned char mPrime[8+2*SHASIZE/8];
    memset(mPrime, 0x00, 8);
    memcpy(mPrime+8, mHash, SHASIZE/8);
    memcpy(mPrime+8+SHASIZE/8, salt, SHASIZE/8);

    // mPrime Hash 생성
    unsigned char mPrimeHash[SHASIZE/8];
    sha(mPrime, 8+2*SHASIZE/8, mPrimeHash);

    // mPrime Hash와 H 비교
    if(memcmp(mPrimeHash, H, SHASIZE/8) != 0) return EM_HASH_MISMATCH;
    
    return 0;
}
