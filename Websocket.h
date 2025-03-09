#ifndef WEBSOCKET_H
#define WEBSOCKET_H
#include <windows.h>
#include <winhttp.h>
#include "DynamicBuffer.h"

typedef struct _REQINFO{
	HINTERNET Session;
	HINTERNET HTTPconnection;
	HINTERNET Request;
	HINTERNET WebSocket;
} REQINFO, * LPREQINFO;


int StartWebsocket(wchar_t * host,int port,wchar_t * uri,LPREQINFO reqinfo,PDYNBUF_INFO headers);
void cleanUPReq(LPREQINFO reqinfo);
int WebSocketReadDataAsString(LPREQINFO reqinfo,PDYNBUF_INFO buffer);

#endif