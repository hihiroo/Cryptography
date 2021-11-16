/*
 * Copyright 2020, 2021. Heekuck Oh, all rights reserved
 * 이 프로그램은 한양대학교 ERICA 소프트웨어학부 재학생을 위한 교육용으로 제작되었습니다.
 */
#include <stdlib.h>
#include <bsd/stdlib.h>
#include "mRSA.h"

#define swap(a,b,type) do{type tmp=a; a=b; b=tmp;}while(0);
const uint64_t A[ALEN] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};

// a와 b의 최대공약수
static uint64_t gcd(uint64_t a, uint64_t b){
    uint64_t tmp;
    while(b > 0){
        tmp = b;
        b = a % b;
        a = tmp;
    } 
    return a;
}


// computes a+b mod m
static uint64_t mod_add(uint64_t a, uint64_t b, uint64_t m){
    a = a % m;
    b = b % m;
    if(a >= m-b) return a-(m-b); // if(a+b >= m) return (a+b)-m
    return a+b;
}

// computes ab mod m
static uint64_t mod_mul(uint64_t a, uint64_t b, uint64_t m){
    uint64_t r = 0;
    while(b > 0){
        if(b & 1) r = mod_add(r, a, m);
        b = b >> 1;
        a = mod_add(a, a, m);
    }
    return r;
}

// computes a^b mod m
static uint64_t mod_pow(uint64_t a, uint64_t b, uint64_t m){
    uint64_t r = 1;
    while(b > 0){
        if(b & 1) r = mod_mul(r, a, m);
        b = b >> 1;
        a = mod_mul(a, a, m);
    }
    return r;
}

// computes a^-1 mod m
static uint64_t mul_inv(uint64_t a, uint64_t m){
    uint64_t d0 = a, d1 = m, q; 
	long long x0 = 1, x1 = 0; 

	while(d1 > 1){
		q = d0/d1;
		d0 = d0 - q * d1; swap(d0,d1,uint64_t); // (d1, d0 % d1)
		x0 = x0 - (long long)q * x1; swap(x0,x1,long long); // (x1, x0-q*x1)
	}

	if(d1 == 1) return x1>0 ? (uint64_t)x1 : m-(uint64_t)(-x1);
	return 0;
}

static int isComposite(uint64_t ai, uint64_t q, uint64_t k, uint64_t n){
    uint64_t tmp = mod_pow(ai, q, n);
    if(tmp == 1 || tmp == n-1) return PRIME; // a^q % n == 1이면 소수일 확률이 큼

    for(uint64_t j=1; j<k; j++){
        tmp = mod_mul(tmp,tmp,n);
        if(tmp == n-1) return PRIME;
    }
    return COMPOSITE;
}

static int miller_rabin(uint64_t n){
    if(n == 2) return PRIME;
    else if(n < 2 || n % 2 == 0) return COMPOSITE;

    uint64_t k = 0, q = n-1;
    while(!(q & 1)){ // (n-1) = (2^k)q 인 k, q 찾기
        k++;
        q = q >> 1;
    }

    for(int i=0; (i<ALEN && A[i]<n-1); i++){
        if(isComposite(A[i], q, k, n) == COMPOSITE) return COMPOSITE;
    }
    return PRIME;
}

/*
 * mRSA_generate_key() - generates mini RSA keys e, d and n
 * Carmichael's totient function Lambda(n) is used.
 */
void mRSA_generate_key(uint64_t *e, uint64_t *d, uint64_t *n)
{
    uint64_t p = (1 << 31), q = (1 << 31); // 32번째 비트는 항상 1

    while(1){
        p += (uint64_t)arc4random_uniform(1 << 30); // [2^31, 2^32) 난수 생성
        q += (uint64_t)arc4random_uniform(1 << 30);
        q |= 1, p |= 1; // 홀수 만들기
        if(miller_rabin(p) != PRIME || miller_rabin(q) != PRIME || p*q < MINIMUM_N) continue;
        break;
    } 
    *n = p * q;

    uint64_t lambda = (p-1) * (q-1) / gcd(p-1, q-1);
    *e = (1 << 16) + 1; // 65537
    *d = mul_inv(*e, lambda); // ed mod lambda = 1
}

/*
 * mRSA_cipher() - compute m^k mod n
 * If data >= n then returns 1 (error), otherwise 0 (success).
 */
int mRSA_cipher(uint64_t *m, uint64_t k, uint64_t n){
    if(*m >= n) return 1;
    *m = mod_pow(*m, k, n);
    return 0;
}
