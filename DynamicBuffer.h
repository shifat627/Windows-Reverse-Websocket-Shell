#ifndef DYNAMICBUFFER_H
#define DYNAMICBUFFER_H

#include<stdio.h>
#include<string.h>
#include<malloc.h>
#include <ctype.h>
#include <locale.h>

typedef unsigned char BYTE , * PBYTE;
typedef unsigned __LONG32 DWORD , * PDWORD;
typedef struct {
	PBYTE data;
	DWORD length;
	DWORD capacity;
} DYNBUF_INFO , * PDYNBUF_INFO;

int DynBuf_Join_AnsiTowide(PDYNBUF_INFO wide,char * multi,char * local);
int DynBuf_Join(PDYNBUF_INFO ByteInfo,PBYTE data,DWORD len);
void DynBuf_Free(PDYNBUF_INFO ByteInfo);
int DynBuf_Assign(PDYNBUF_INFO ByteInfo,PBYTE data,DWORD len);
void DynBuf_PrintInfo(PDYNBUF_INFO ByteInfo);
int DynBuf_Init(int capacity,PDYNBUF_INFO ByteInfo);
int DynBuf_Expand(int capacity,PDYNBUF_INFO ByteInfo);

#endif