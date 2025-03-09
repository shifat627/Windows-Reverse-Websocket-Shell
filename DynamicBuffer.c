#include "DynamicBuffer.h"


int DynBuf_Join_AnsiTowide(PDYNBUF_INFO wide,char * multi,char * local){
	DWORD TotalReqBytes=0;
	DWORD multiLen = strlen(multi);
	TotalReqBytes += ( multiLen * sizeof(wchar_t));
	
	TotalReqBytes += (wide->length);
	
	if (wide->capacity < TotalReqBytes) {
		PBYTE p = realloc(wide->data,TotalReqBytes);
		if (!p){
			printf("Failed To allocate Memory");
			return 0;
		}
		
		wide->data = p;
		wide->capacity = TotalReqBytes;
	}
	
	
	setlocale(LC_ALL, local);
	size_t size = mbstowcs((wchar_t *)(wide->data + wide->length),multi,multiLen);
	
	
	if (size != multiLen){
		return 0;
	}
	wide->length = TotalReqBytes ;	

	return 1;
}


int DynBuf_Init(int capacity,PDYNBUF_INFO ByteInfo){
	
	
	
	void * p = malloc(capacity);
	if (!p){
		printf("Failed To allocate Memory");
		return 0;
	}
	
	memset(p,0,capacity);
	
	
	ByteInfo->capacity = capacity;
	ByteInfo->data = (PBYTE) p;
	ByteInfo->length = 0;
	
	
	
	
	return 1;
}


int DynBuf_Join(PDYNBUF_INFO ByteInfo,PBYTE data,DWORD len){
	
	DWORD TotalReqBytes=0;
	
	
	TotalReqBytes += len;
	TotalReqBytes += ByteInfo->length;
	
	
	
	if (ByteInfo->capacity < TotalReqBytes) {
		PBYTE p = realloc(ByteInfo->data,TotalReqBytes);
		if (!p){
			printf("Failed To allocate Memory");
			return 0;
		}
		
		ByteInfo->data = p;
		ByteInfo->capacity = TotalReqBytes;
	}
	
	
	memcpy(ByteInfo->data + ByteInfo->length,data,len);
	
	ByteInfo->length = TotalReqBytes ;	

	return 1;
}


void DynBuf_Free(PDYNBUF_INFO ByteInfo){
	free(ByteInfo->data);
	memset(ByteInfo,0,sizeof(DYNBUF_INFO));
}


int DynBuf_Assign(PDYNBUF_INFO ByteInfo,PBYTE data,DWORD len){
	

    if (data == NULL){
        memset(ByteInfo->data,0,ByteInfo->capacity);
        ByteInfo->length = 0;
        return 1;
    }

	DWORD TotalReqBytes=0;
	
	
	TotalReqBytes += len;
	
	

	
	//printf("Len: %ld\n",TotalReqBytes);
	
	
	if (ByteInfo->capacity < TotalReqBytes) {
		PBYTE p = realloc(ByteInfo->data,TotalReqBytes);
		if (!p){
			printf("Failed To allocate Memory");
			return 0;
		}
		
		ByteInfo->data = p;
		ByteInfo->capacity = TotalReqBytes;
	}
	
	memset(ByteInfo->data,0,ByteInfo->capacity);
	memcpy(ByteInfo->data,data,len);
	
	ByteInfo->length = TotalReqBytes ;
	
	
	
	return 1;
}


void DynBuf_PrintInfo(PDYNBUF_INFO ByteInfo){

	printf("Capacity -> %ld\nLength -> %ld\nData -> ",ByteInfo->capacity,ByteInfo->length);
    fwrite(ByteInfo->data,ByteInfo->length,1,stdout);
	printf("\n\n\n\n\n");
}


int DynBuf_Expand(int capacity,PDYNBUF_INFO ByteInfo){
	
	
	
	void * p = realloc(ByteInfo->data,capacity + ByteInfo->capacity);
	if (!p){
		printf("Failed To allocate Memory");
		return 0;
	}
	
	memset(p + ByteInfo->capacity ,0,capacity);
	
	
	ByteInfo->capacity += capacity;
	ByteInfo->data = (PBYTE) p;
	
	
	
	
	
	return 1;
}
