// JavaDDE.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "DDEServer.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
        ::DefWindowProc(NULL, 0, 0, 0L);
        // Allocate a TLS index.
        if ((DDEServer::m_dwTlsIndex = TlsAlloc()) == TLS_OUT_OF_INDEXES) 
            return FALSE; 

        // No break: Initialize the index for first thread.
        // The attached process creates a new thread.
    case DLL_THREAD_ATTACH: 
        {
            // Initialize the TLS index for this thread.
            LPVOID lpvData = (LPVOID) LocalAlloc(LPTR, sizeof(DDEServer*)); 
            if (lpvData != NULL) 
                TlsSetValue(DDEServer::m_dwTlsIndex, lpvData); 
        }
        break; 

        // The thread of the attached process terminates.

    case DLL_THREAD_DETACH: 
        {
            // Release the allocated memory for this thread.
            LPVOID lpvData = TlsGetValue(DDEServer::m_dwTlsIndex); 
            if (lpvData != NULL) 
                LocalFree((HLOCAL)lpvData); 
        }
        break; 

        // DLL unload due to process termination or FreeLibrary. 

    case DLL_PROCESS_DETACH: 
        {
            // Release the allocated memory for this thread.
            LPVOID lpvData = TlsGetValue(DDEServer::m_dwTlsIndex); 
            if (lpvData != NULL) 
                LocalFree((HLOCAL)lpvData); 
        }
        // Release the TLS index.
        TlsFree(DDEServer::m_dwTlsIndex); 
        break; 

    default: 
        break; 
    }
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

JavaVM *g_vm = NULL;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    g_vm = vm;

    return JNI_VERSION_1_4;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
    g_vm = NULL;
}
