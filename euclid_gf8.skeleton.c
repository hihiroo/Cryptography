/*
 * Copyright 2020, 2021. Heekuck Oh, all rights reserved
 * 이 프로그램은 한양대학교 ERICA 소프트웨어학부 재학생을 위한 교육용으로 제작되었습니다.
 */
#include <stdio.h>
#include <stdlib.h>

/*
 * gcd() - Euclidean algorithm
 *
 * 유클리드 알고리즘 gcd(a,b) = gcd(b,a mod b)를 사용하여 최대공약수를 계산한다.
 * 만일 a가 0이면 b가 최대공약수가 된다. 그 반대도 마찬가지이다.
 * a, b가 모두 음이 아닌 정수라고 가정한다.
 * 재귀함수 호출을 사용하지 말고 while 루프를 사용하여 구현하는 것이 빠르고 좋다.
 */
int gcd(int a, int b)
{
    // 여기를 완성하세요
}

/*
 * xgcd() - Extended Euclidean algorithm
 *
 * 확장유클리드 알고리즘은 두 수의 최대공약수와 gcd(a,b) = ax + by 식을 만족하는
 * x와 y를 계산하는 알고리즘이다. 강의노트를 참조하여 구현한다.
 * a, b가 모두 음이 아닌 정수라고 가정한다.
 */
int xgcd(int a, int b, int *x, int *y)
{
    // 여기를 완성하세요
}

/*
 * mul_inv() - computes multiplicative inverse a^-1 mod m
 *
 * 모듈로 m에서 a의 곱의 역인 a^-1 mod m을 구한다.
 * 만일 역이 존재하지 않는다면 0을 리턴해야 한다.
 * 확장유클리드 알고리즘을 변형하면 구할 수 있다. 강의노트를 참조한다.
 */
int mul_inv(int a, int m)
{
    // 여기를 완성하세요
}

/*
 * umul_inv() - computes multiplicative inverse a^-1 mod m
 *
 * 입력이 unsigned 64 비트 정수로 되어 있는 특수한 경우에
 * 모듈로 m에서 a의 곱의 역인 a^-1 mod m을 구한다.
 * 만일 역이 존재하지 않는다면 0을 리턴해야 한다.
 * 확장유클리드 알고리즘을 변형하면 구할 수 있다.
 * 입출력 모두가 unsigned 64 비트 정수임을 고려해서 구현한다.
 */
uint64_t umul_inv(uint64_t a, uint64_t m)
{
    // 여기를 완성하세요
}

/*
 * gf8_mul(a, b) - a * b mod x^8+x^4+x^3+x+1
 *
 * 7차식 다항식 a와 b를 곱하고 결과를 8차식 x^8+x^4+x^3+x+1로 나눈 나머지를 계산한다.
 * x^8 = x^4+x^3+x+1 (mod x^8+x^4+x^3+x+1) 특성을 이용한다.
 */
uint8_t gf8_mul(uint8_t a, uint8_t b)
{
    // 여기를 완성하세요
}

/*
 * gf8_pow(a,b) - a^b mod x^8+x^4+x^3+x+1
 *
 * 7차식 다항식 a을 b번 지수승한 결과를 8차식 x^8+x^4+x^3+x+1로 나눈 나머지를 계산한다.
 * gf8_mul()과 "Square Multiplication" 알고리즘을 사용하여 구현한다.
 */
uint8_t gf8_pow(uint8_t a, uint8_t b)
{
    // 여기를 완성하세요
}

/*
 * gf8_inv(a) - a^-1 mod x^8+x^4+x^3+x+1
 *
 * 모둘러 x^8+x^4+x^3+x+1에서 a의 역을 구한다.
 * 역을 구하는 가장 효율적인 방법은 다항식 확장유클리드 알고리즘을 사용하는 것이다.
 * 다만 여기서는 복잡성을 피하기 위해 느리지만 알기 쉬운 지수를 사용하여 구현하였다.
 */
uint8_t gf8_inv(uint8_t a)
{
    return gf8_pow(a, 0xfe);
}

/*
 * 함수가 올르게 동작하는지 검증하기 위한 메인 함수로 수정해서는 안 된다.
 */
int main(void)
{
    int a, b, x, y, d, count;
    uint64_t m, ai;
    
    /*
     * 기본 gcd 시험
     */
    printf("--- 기본 gcd 시험 ---\n");
    a = 28; b = 0;
    printf("gcd(%d,%d) = %d\n", a, b, gcd(a,b));
    a = 0; b = 32;
    printf("gcd(%d,%d) = %d\n", a, b, gcd(a,b));
    a = 41370; b = 22386;
    printf("gcd(%d,%d) = %d\n", a, b, gcd(a,b));
    a = 22386; b = 41371;
    printf("gcd(%d,%d) = %d\n", a, b, gcd(a,b));
    
    /*
     * 기본 xgcd, mul_inv 시험
     */
    printf("--- 기본 xgcd, mul_inv 시험 ---\n");
    a = 41370; b = 22386;
    d = xgcd(a, b, &x, &y);
    printf("%d = %d * %d + %d * %d\n", d, a, x, b, y);
    printf("%d^-1 mod %d = %d, %d^-1 mod %d = %d\n", a, b, mul_inv(a,b), b, a, mul_inv(b,a));
    a = 41371; b = 22386;
    d = xgcd(a, b, &x, &y);
    printf("%d = %d * %d + %d * %d\n", d, a, x, b, y);
    printf("%d^-1 mod %d = %d, %d^-1 mod %d = %d\n", a, b, mul_inv(a,b), b, a, mul_inv(b,a));
    
    /*
     * 난수 a와 b를 발생시켜 xgcd를 계산하고, 최대공약수가 1이면 역이 존재하므로
     * 여기서 얻은 a^-1 mod b와 b^-1 mod a를 mul_inv를 통해 확인한다.
     * 이 과정을 8백만번 이상 반복하여 올바른지 확인한다.
     */
    printf("--- 무작위 mul_inv 시험 ---\n"); fflush(stdout);
    count = 0;
    do {
        arc4random_buf(&a, sizeof(int)); a &= 0x7fffffff;
        arc4random_buf(&b, sizeof(int)); b &= 0x7fffffff;
        d = xgcd(a, b, &x, &y);
        if (d == 1) {
            if (x < 0)
                x = x + b;
            else
                y = y + a;
            if (x != mul_inv(a, b) || y != mul_inv(b, a)) {
                printf("Inversion error\n");
                exit(1);
            }
        }
        if (++count % 0xffff == 0) {
            printf(".");
            fflush(stdout);
        }
    } while (count < 0xfffff);
    printf("No error found\n");
    
    /*
     * GF(2^8)에서 기본 a*b 시험
     */
    printf("--- GF(2^8)에서 기본 a*b 시험 ---\n");
    a = 28; b = 7;
    printf("%d * %d = %d\n", a, b, gf8_mul(a,b));
    a = 127; b = 68;
    printf("%d * %d = %d\n", a, b, gf8_mul(a,b));

    /*
     * GF(2^8)에서 a를 1부터 255까지 a^-1를 구하고 a * a^-1 = 1인지 확인한다.
     */
    printf("--- GF(2^8)에서 전체 a*b 시험 ---\n");
    for (a = 1; a < 256; ++a) {
        if (a == 0) continue;
        b = gf8_inv(a);
        if (gf8_mul(a,b) != 1) {
            printf("Logic error\n");
            exit(1);
        }
        else {
            printf(".");
            fflush(stdout);
        }
    }
    printf("No error found\n");

    /*
     * umul_inv 시험
     */
    printf("--- 기본 umul_inv 시험 ---\n");
    a = 5; m = 9223372036854775808u;
    ai = umul_inv(a, m);
    printf("a = %d, m = %llu, a^-1 mod m = %llu", a, m, ai);
    if (ai != 5534023222112865485u) {
        printf(" <- inversion error\n");
        exit(1);
    }
    else
        printf(" OK\n");
    a = 17; m = 9223372036854775808u;
    ai = umul_inv(a, m);
    printf("a = %d, m = %llu, a^-1 mod m = %llu", a, m, ai);
    if (ai != 8138269444283625713u) {
        printf(" <- inversion error\n");
        exit(1);
    }
    else
        printf(" OK\n");
    a = 85; m = 9223372036854775808u;
    ai = umul_inv(a, m);
    printf("a = %d, m = %llu, a^-1 mod m = %llu", a, m, ai);
    if (ai != 9006351518340545789u) {
        printf(" <- inversion error\n");
        exit(1);
    }
    else
        printf(" OK\n");

    printf("Congratulations!\n");
    return 0;
}
