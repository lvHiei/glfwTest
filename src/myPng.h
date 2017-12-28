/*
 * myPng.h
 *
 *  Created on: 2017年12月28日
 *      Author: mj
 */

#ifndef MYPNG_H_
#define MYPNG_H_

#include <stdint.h>
#include <stdio.h>
#include <png.h>

typedef struct PNGHandle{
	uint8_t* data;
	uint32_t length;
	uint32_t width;
	uint32_t height;
}PNGHandle;

PNGHandle* mallocPngHandle();
void freePngHandle(PNGHandle*);
int decodePng(const char*, PNGHandle*);

#endif /* MYPNG_H_ */
