/*
* Copyright 2020, 2021. Heekuck Oh, all rights reserved
* 이 프로그램은 한양대학교 ERICA 소프트웨어학부 재학생을 위한 교육용으로 제작되었습니다.
*/
#include <stdio.h>
#include <stdlib.h>
#include "mRSA.h"

int main(void)
{
    uint64_t e, d, n, m, c;
    int i, count;

    /*
     * 기본시험 1: m = 0부터 19까지 암복호화 검증
     */
    mRSA_generate_key(&e, &d, &n);
    if (n < MINIMUM_N) {
        printf("Error: RSA key is not 64 bits: n = %016llx\n", n);
        exit(1);
    }
    printf("e = %016llx\nd = %016llx\nn = %016llx\n", e, d, n);
    for (i = 0; i < 20; ++i) {
        m = i;
        printf("m = %llu, ", m);
        mRSA_cipher(&m, e, n);
        printf("c = %llu, ", m);
        mRSA_cipher(&m, d, n);
        printf("v = %llu\n", m);
    }
    /*
     * 기본시험 2: 무작위로 m을 발생하여 암복호화 검증
     */
    mRSA_generate_key(&e, &d, &n);
    printf("e = %016llx\nd = %016llx\nn = %016llx\n", e, d, n);
    for (i = 0; i < 20; ++i) {
        arc4random_buf(&m, sizeof(uint64_t));
        printf("m = %016llx, ", m);
        if (mRSA_cipher(&m, d, n))
            printf("m may be too big\n");
        else {
            printf("c = %016llx, ", m);
            mRSA_cipher(&m, e, n);
            printf("v = %016llx\n", m);
        }
    }
    /*
     * RSA 키와 평문을 무작위로 선택해서 암호화와 복호화를 수행하여 원래 평문과 일치하는지 검증한다.
     * 이 과정을 여러번 반복하여 올바른지 확인한다.
     */
    printf("Random testing"); fflush(stdout);
    count = 0;
    do {
        mRSA_generate_key(&e, &d, &n);
        arc4random_buf(&m, sizeof(uint64_t)); m &= 0x7fffffffffffffff;
        c = m;
        if (mRSA_cipher(&c, e, n)) {
            printf("Error: RSA key is not 64 bits: %016llx\n", n);
            exit(1);
        };
        if (mRSA_cipher(&c, d, n)) {
            printf("Error: check your modular calculations.\n");
            exit(1);
        };
        if (m != c) {
            printf("Error: your RSA key generation may be wrong.\n");
            exit(1);
        }
        if (++count % 0xff == 0) {
            printf(".");
            fflush(stdout);
        }
    } while (count < 0xfff);
    printf("No error found!\n");
}
