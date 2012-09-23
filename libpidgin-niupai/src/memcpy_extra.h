//
//  memcpy_extra.h
//  NiupaiOnAdium
//
//  Created by Ryan Wang on 12-9-23.
//  Copyright (c) 2012年 Ryan Wang. All rights reserved.
//

#ifndef __MEMCPY_EXTRA_H__
#define __MEMCPY_EXTRA_H__

void memcpy_offset (char *dst, char *src, int doff, int soff, int len);
void memmove_offset (char *dst, char *src, int doff, int soff, int len);

#endif /* __MEMCPY_EXTRA_H__ */
