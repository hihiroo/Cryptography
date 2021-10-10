/*
 * Copyright 2020. Heekuck Oh, all rights reserved
 * 이 프로그램은 한양대학교 ERICA 소프트웨어학부 재학생을 위한 교육용으로 제작되었습니다.
 */
#include <stdio.h>
#include <string.h>
#include "aes.h"

static const uint8_t sbox[256] = {
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

static const uint8_t isbox[256] = {
  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };

static const uint8_t Rcon[11] = {0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};

static const uint8_t M[16] = {2, 3, 1, 1, 1, 2, 3, 1, 1, 1, 2, 3, 3, 1, 1, 2};

static const uint8_t IM[16] = {0x0e, 0x0b, 0x0d, 0x09, 0x09, 0x0e, 0x0b, 0x0d, 0x0d, 0x09, 0x0e, 0x0b, 0x0b, 0x0d, 0x09, 0x0e};

/*
 * Generate an AES key schedule
 */
void uint8_to_uint32(const uint8_t *a, uint32_t *res){
  *res = ((uint32_t)a[3] << 24) | ((uint32_t)a[2] << 16) | ((uint32_t)a[1] << 8) | (uint32_t)a[0];
}

void LRotWord(uint8_t *array){ // 한 칸 왼쪽으로 이동
  uint8_t rot0 = array[0];
  for(int i=0; i<3; i++) array[i] = array[i+1];
  array[3] = rot0;
}

uint32_t g_func(const uint32_t *rkey, int roundNum){
  uint8_t tmp[4];
  memcpy(tmp, rkey, 4);

  // LRotWord and S-Box
  LRotWord(tmp);
  for(int i=0; i<4; i++) tmp[i] = sbox[tmp[i]];

  // casting uint8 to uint32
  uint32_t res;
  uint8_to_uint32(tmp, &res);

  return res ^ Rcon[roundNum];
}

void KeyExpansion(const uint8_t *key, uint32_t *roundKey)
{
  // key가 주어지면 각 라운드에 쓰일 roundkey 계산
  // key[0:3] = w0, key[4:7] = w1, ..., key[KEYLEN-4:KEYLEN-1] = w_NK-1
  // 즉, 각 key는 1바이트 단위, round key는 4바이트 단위

  for(int i=0; i<Nk; i++){ // 첫 라운드는 사용자 키를 사용
    uint8_to_uint32(key+i*4, &roundKey[i]);
  }

  for(int i=Nk; i<RNDKEYSIZE; i++){ // key expansion
    if(i % Nk == 0) roundKey[i] = g_func(roundKey+i-1, i/Nk) ^ roundKey[i-Nk];
    else roundKey[i] = roundKey[i-1] ^ roundKey[i-Nk];
  }
}

static void AddRoundKey(uint8_t *state, const uint32_t *roundKey){
  uint8_t *tmp = (uint8_t *)roundKey;
  for(int i=0; i<BLOCKLEN; i++) state[i] ^= tmp[i];
}

static void SubBytes(uint8_t *state, int mode){
  if(mode == ENCRYPT){
    for(int i=0; i<BLOCKLEN; i++) state[i] = sbox[state[i]];
  }
  else{
    for(int i=0; i<BLOCKLEN; i++) state[i] = isbox[state[i]];
  }
}

static void ShiftRows(uint8_t *state, int mode){
  // k번째 행의 각 원소를 k번 왼쪽으로 이동시킴
  // 복호화 모드에서는 오른쪽으로 k번 이동
  for(int k=0; k<4; k++){ // k번째 행 
    uint8_t tmp[Nb];
    for(int i=0; i<Nb; i++) *(tmp+i) = *(state+i*4+k);

    if(mode == ENCRYPT){
      for(int i=0; i<k; i++) LRotWord(tmp);
    }
    else{// 오른쪽으로 k번 이동 == 왼쪽으로 Nb-k번 이동
      for(int i=0; i<Nb-k; i++) LRotWord(tmp); 
    }

    for(int i=0; i<Nb; i++) state[i*4+k] = tmp[i]; 
  }
}

uint8_t gf8_mul(uint8_t a, uint8_t b){
    //gf(2^8)에서의 a*b 연산을 계산한다.
    uint8_t r = 0; 
    
    while(b>0){
        if(b&1) r=r^a; 
        b = b >> 1;
        a = XTIME(a);
    }
    
    return r;
}


static void MixColumns(uint8_t *state, int mode){
  // gf(2^8)에서의 행렬곱 연산
  // 암호화 모드에서는 M행렬과, 복호화 모드에서는 IM행렬과 곱해준다.
  // state = M*state, state = IM*state
  uint8_t tmp[16], res[4][4];

  if(mode == ENCRYPT) memcpy(tmp,M,sizeof(M));
  else memcpy(tmp,IM,sizeof(IM));

  for(int i=0; i<4; i++){ //rows
    for(int j=0; j<4; j++){
      res[i][j] = 0;

      for(int k=0; k<4; k++){ //tmp[i][k]*state[k][j]
        res[i][j] ^= gf8_mul(tmp[4*i+k],state[4*j+k]);
      }
    }
  }

  for(int i=0; i<4; i++){
    for(int j=0; j<4; j++){
      state[4*j+i] = res[i][j];
    }
  }
}

/*
 * AES cipher function
 * If mode is nonzero, then do encryption, otherwise do decryption.
 */

void Cipher(uint8_t *state, const uint32_t *roundKey, int mode)
{
  // 처음 state는 plaintext or ciphertext
  // 마지막은 mix columns이 빠진 incomplete round
  // 암호화와 복호화 모드일 때 라운드가 거꾸로 진행됨 주의
  if(mode == ENCRYPT){
    AddRoundKey(state, roundKey);

    for(int i=1; i<Nr; i++){
      SubBytes(state,mode);
      ShiftRows(state,mode);
      MixColumns(state,mode);
      AddRoundKey(state,roundKey+Nb*i);
    }
    SubBytes(state,mode);
    ShiftRows(state,mode);
    AddRoundKey(state,roundKey+Nb*Nr);
  }
  else{
    AddRoundKey(state, roundKey+Nr*Nb);

    for(int i=Nr-1; i>0; i--){
      ShiftRows(state,mode);
      SubBytes(state,mode);
      AddRoundKey(state,roundKey+Nb*i);
      MixColumns(state,mode);
    }
    ShiftRows(state,mode);
    SubBytes(state,mode);
    AddRoundKey(state,roundKey);
  }
}