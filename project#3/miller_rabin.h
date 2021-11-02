/*
 * Copyright 2020, 2021. Heekuck Oh, all rights reserved
 * 이 프로그램은 한양대학교 ERICA 소프트웨어학부 재학생을 위한 교육용으로 제작되었습니다.
 */
#ifndef MILLER_RABIN_H
#define MILLER_RABIN_H

#include <stdint.h>

#define ALEN 12
#define PRIME 1
#define COMPOSITE 0

uint64_t mod_add(uint64_t a, uint64_t b, uint64_t m);
uint64_t mod_sub(uint64_t a, uint64_t b, uint64_t m);
uint64_t mod_mul(uint64_t a, uint64_t b, uint64_t m);
uint64_t mod_pow(uint64_t a, uint64_t b, uint64_t m);
int miller_rabin(uint64_t n);

#endif
