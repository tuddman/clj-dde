/*
 * Copyright 2013 www.pretty-tools.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "StdAfx.h"
#include "DDEServer.h"

#define WAIT_THREAD_INITIALIZATION 5000
#define WAIT_THREAD_TERMINATION 5000

#define ISTOPICSUPPORTED                   "isTopicSupported"
#define ISITEMSUPPORTED                    "isItemSupported"
#define ONEXECUTE                          "onExecute"
#define ONREQUEST                          "onRequest"
#define ONPOKE                             "onPoke"

const char * DDEServer::DDEServerFieldName = "nativeDDEServer";

DWORD DDEServer::m_dwTlsIndex;

BOOL DDEServer::SetThis(DDEServer* _this)
{
    // Retrieve a data pointer for the current thread
    LPVOID lpvData = TlsGetValue(m_dwTlsIndex); 

    // If NULL, allocate memory for the TLS slot for this thread
    if (lpvData == NULL)
    {
        lpvData = (LPVOID) LocalAlloc(LPTR, sizeof(DDEServer*)); 
        if (lpvData == NULL) 
            return FALSE;
        if (!TlsSetValue(m_dwTlsIndex, lpvData))
            return FALSE;
    }

    *((DDEServer**)lpvData) = _this;
    return TRUE;
}

DDEServer* DDEServer::GetThis()
{
    LPVOID lpvData = TlsGetValue(m_dwTlsIndex); 
    return !lpvData ? NULL : *((DDEServer**)lpvData);
}

DDEServer::DDEServer(const CString &strService, WJNIEnv env, jobject globalRefObject) throw (...)
    : m_javaDDEServer(globalRefObject)
    , m_strService(strService)
    , m_env(NULL)
    , m_eStarted(TRUE)
    , m_eInitializationError(TRUE)
    , m_idInst(0)
    , m_hszService(NULL)
{
    ATLTRACE( _T("DDEServer::DDEServer(%s)\n"), strService );
}

DDEServer::~DDEServer(void)
{
    ATLTRACE( _T("DDEServer::~DDEServer(%s)\n"), m_strService );
}

void DDEServer::KillWorkingThread()
{
    ATLTRACE( _T("Kill DDE working thread\n") );
    m_thread.Terminate();

    if (::IsWindow(m_hWnd))
        DestroyWindow();
    Detach();
}

void DDEServer::Start() throw (...)
{
    ATLTRACE( _T("Creating DDE working thread\n") );

    if (!m_thread.Create(this))
        throw DDEException(IDS_ERROR_CANNOT_CREATE_THREAD);

    ATLTRACE( _T("DDE working thread was created for %s\n"), m_strService );

    HANDLE hEvents[] = { (HANDLE)m_eStarted, (HANDLE)m_eInitializationError };

    DWORD dwEvent = WaitForMultipleObjects(sizeof(hEvents)/sizeof(hEvents[0]), hEvents, FALSE, WAIT_THREAD_INITIALIZATION);

    if (dwEvent == WAIT_TIMEOUT) // something went wrong, let's restart
    {
        KillWorkingThread();

        if (!m_thread.Create(this))
            throw DDEException(IDS_ERROR_CANNOT_CREATE_THREAD);

        ATLTRACE( _T("DDE working thread was re-created for %s\n"), m_strService );

        m_eStarted.Reset();
        m_eInitializationError.Reset();

        dwEvent = WaitForMultipleObjects(sizeof(hEvents)/sizeof(hEvents[0]), hEvents, FALSE, WAIT_THREAD_INITIALIZATION);

        if (dwEvent == WAIT_TIMEOUT) // something still wrong, throw exception
        {
            KillWorkingThread();

            throw DDEException(IDS_ERROR_INITIALIZATION_FAILED);
        }
    }

    if (dwEvent == (WAIT_OBJECT_0 + 1))
        throw DDEException(IDS_ERROR_INITIALIZATION_FAILED);

    ReThrowDDEException(SendMessage(UWM_NAMESERVICE));
}

void DDEServer::Run()
{
    ATLTRACE( _T("New thread has been started for %s\n"), m_strService );

    DDEServer::SetThis(this);

    ATLTRACE( _T("Attach thread to JavaVM\n") );
    if (g_vm->AttachCurrentThreadAsDaemon((void**)&m_env, NULL))
    {
        ATLTRACE( _T("Cannot attach current thread\n") );
        m_eInitializationError.Set();
        return;
    }

    ATLTRACE( _T("Create helper window for %s\n"), m_strService );
    if (Create(NULL, NULL, NULL, WS_POPUP))
    {
        ATLTRACE( _T("Create helper window was created for %s\n"), m_strService );

        ATLTRACE( _T("Posting init message\n") );
        PostMessage(UWM_INIT_DDE);

        for (MSG msg; GetMessage(&msg, NULL, 0, 0); )
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        ATLTRACE( _T("Destroy Helper Window\n") );
        DestroyWindow(), m_hWnd = NULL;
    }
    else
    {
        ATLTRACE( _T("Cannot create helper window\n") );
        m_eInitializationError.Set();
    }

    ATLTRACE( _T("Detach thread to JavaVM\n") );
    if (g_vm->DetachCurrentThread())
    {
        ATLTRACE( _T("Cannot detach current thread\n") );
    }

    DDEServer::SetThis(NULL);
}

void DDEServer::Release(WJNIEnv env)
{
    ATLTRACE( _T("DDEServer::Release(env)\n") );

    if (m_hWnd)
    {
        ATLTRACE( _T("Send SHUTDOWN to DDEClientTransactionManagerDispatcher\n") );
        SendMessage(UWM_SHUTDOWN_DDE);
    }

    if (m_thread)
    {
        ATLTRACE( _T("Wait for DDEClientTransactionManagerDispatcher thread termination\n") );
        if (m_thread.Wait(WAIT_THREAD_TERMINATION) != WAIT_OBJECT_0)
        {
            ATLTRACE( _T("DDEClientTransactionManagerDispatcher thread is not terminated\n") );
        }
    }

    try
    {
        ATLTRACE( _T("Delete Global Ref to java DDEServer(%s)\n"), m_strService );

        if (m_javaDDEServer) env->DeleteGlobalRef(m_javaDDEServer);
    }
    catch (...)
    {
        ATLTRACE( _T("Exception on DeleteGlobalRef\n") );
    }

    ATLTRACE( _T("Delete DDEServer(%s)\n"), m_strService );
    delete this;
}

DDEServer* DDEServer::GetDDEServer(WJNIEnv env, jobject object) throw (...)
{
    DDEServer* pDDEServer = (DDEServer*)env.GetLongField(object, DDEServerFieldName);

    if (!pDDEServer)
        throw DDEException(IDS_ERROR_SERVER_IS_NOT_STARTED);

    return pDDEServer;
}

void DDEServer::ReThrowDDEException(LRESULT pException)
{
    if (pException) ReThrowDDEException((DDEException*)pException);
}

void DDEServer::ReThrowDDEException(DDEException* pException)
{
    DDEMLException* pDDEMLException = dynamic_cast<DDEMLException*>(pException);
    if (pDDEMLException)
        throw pDDEMLException;
    else
        throw pException;
}

LRESULT DDEServer::OnInit(UINT, WPARAM, LPARAM)
{
    ATLTRACE( _T("Initializing DDE for %s\n"), m_strService );

    UINT uRes = DdeInitialize(&m_idInst,                                   // receives instance identifier
                             (PFNCALLBACK)DDEServer::DdeServerCallback,    // pointer to callback function
                              APPCLASS_STANDARD,
                              0);

#ifdef _DEBUG
    if (DMLERR_NO_ERROR != uRes)
    {
        ATLTRACE( _T("Cannot initialize DDE: 0x%X for %s\n"), uRes, m_strService );
    }
    else
    {
        ATLTRACE( _T("DDE was initialized: idInst=0x%X for %s\n"), m_idInst, m_strService );
    }
#endif // _DEBUG

    if (DMLERR_NO_ERROR == uRes)
    {
        m_hszService = DdeCreateStringHandle(m_idInst, m_strService, NULL);

        m_eStarted.Set();
    }
    else
    {
        m_eInitializationError.Set();
    }
    return 0;
}

LRESULT DDEServer::OnShutDown(UINT, WPARAM, LPARAM)
{
    ATLTRACE( _T("DDE Uninitializing for %s...\n"), m_strService );
    if (m_idInst)
    {
        if(!DdeNameService(m_idInst, m_hszService, NULL, DNS_UNREGISTER))
        {
            ATLTRACE( _T("Cannot unregister service for %s\n"), m_strService );
        }

        if (m_hszService)
            DdeFreeStringHandle(m_idInst, m_hszService);

        if (!DdeUninitialize(m_idInst))
        {
            ATLTRACE( _T("Cannot uninitialize DDE for %s\n"), m_strService );
        }
    }

    ATLTRACE( _T("Posting QUIT message for %s\n"), m_strService );
    m_thread.PostMessage(WM_QUIT);

    return 0;
}

LRESULT DDEServer::OnNameService(UINT, WPARAM, LPARAM)
{
    _TRY_
    {
        if(!DdeNameService(m_idInst, m_hszService, NULL, DNS_REGISTER))
            throw new DDEMLException(DdeGetLastError(m_idInst), IDS_ERROR_NAME_SERVICE);
    }
    _CATCH_AND_RETURN_ALL_
}

void DDEServer::PostAdvise(const CString& strTopic, const CString& strItem) throw (...)
{
    ReThrowDDEException(SendMessage(UWM_POSTADVISE, (WPARAM)(LPCTSTR)strTopic, (LPARAM)(LPCTSTR)strItem));
}

LRESULT DDEServer::OnPostAdvise(UINT, WPARAM pTopic, LPARAM pItem)
{
    _TRY_
    {
        if(!DdePostAdvise(m_idInst, CHSZ(m_idInst, (LPCTSTR)pTopic), CHSZ(m_idInst, (LPCTSTR)pItem)))
            throw new DDEMLException(DdeGetLastError(m_idInst), IDS_ERROR_POSTADVISE);
    }
    _CATCH_AND_RETURN_ALL_
}

HDDEDATA CALLBACK DDEServer::DdeServerCallback(UINT uType,
                                               UINT uFmt,
                                               HCONV hconv,
                                               HSZ hsz1,
                                               HSZ hsz2,
                                               HDDEDATA hdata,
                                               HDDEDATA dwData1,
                                               HDDEDATA dwData2
                                               )
{
    DDEServer* _this = GetThis();
    return _this ? _this->DdeCallback(uType,uFmt,hconv,hsz1,hsz2,hdata,dwData1,dwData2) : 0;
}

HDDEDATA DDEServer::DdeCallback(   UINT uType,
                                   UINT uFmt,
                                   HCONV hconv,
                                   HSZ hsz1,
                                   HSZ hsz2,
                                   HDDEDATA hdata,
                                   HDDEDATA dwData1,
                                   HDDEDATA dwData2
                                   )
{
    switch (uType)
    {
    // server side callbacks
    case XTYP_ADVSTART:
        ATLTRACE(_T("XTYP_ADVSTART\n"));
        // Client is trying to advise.
        if (JNI_TRUE == m_env.CallBooleanMethod(m_javaDDEServer, ISITEMSUPPORTED, HSZToString(m_idInst, hsz1), HSZToString(m_idInst, hsz2), uFmt))
            return (HDDEDATA)TRUE; // SERVER supports Topic|Service
        else
            return FALSE; // SERVER does not support Topic|Service
    case XTYP_ADVSTOP:
        ATLTRACE(_T("XTYP_ADVSTOP\n"));
        break;
    case XTYP_CONNECT:
        ATLTRACE(_T("XTYP_CONNECT\n"));
        // Client is trying to connect.
        if (!DdeCmpStringHandles(hsz2, m_hszService)
            && (JNI_TRUE == m_env.CallBooleanMethod(m_javaDDEServer, ISTOPICSUPPORTED, HSZToString(m_idInst, hsz1))))
            return (HDDEDATA)TRUE; // SERVER supports Topic|Service
        else
            return FALSE; // SERVER does not support Topic|Service
    case XTYP_CONNECT_CONFIRM:
        ATLTRACE(_T("XTYP_CONNECT_CONFIRM\n"));
        break;
    case XTYP_EXECUTE:
        ATLTRACE(_T("XTYP_EXECUTE\n"));
        {
            CString strCommand;
            TDDEData<wchar_t> data(hdata);
            // we need try to detect format of the string here
            if ((data.GetSize()%1) == 1 || ((wchar_t*)data)[data.GetSize()/sizeof(wchar_t) - 1] != 0)
            {
                CStringA strData;
                memcpy(strData.GetBuffer(data.GetSize()), (char*)((wchar_t*)data), data.GetSize());
                strData.ReleaseBuffer(data.GetSize());
                strCommand = strData;
            }
            else
            {
                CStringW strData;
                memcpy(strData.GetBuffer(data.GetSize()/sizeof(wchar_t)), (wchar_t*)data, data.GetSize());
                strData.ReleaseBuffer(data.GetSize()/sizeof(wchar_t));
                strCommand = strData;
            }
 
            return (JNI_TRUE == m_env.CallBooleanMethod(m_javaDDEServer, ONEXECUTE, strCommand)) ? (HDDEDATA)DDE_FACK : (HDDEDATA)DDE_FNOTPROCESSED;
         }
    case XTYP_MASK:
        ATLTRACE(_T("XTYP_MASK\n"));
        break;
    case XTYP_MONITOR:
        ATLTRACE(_T("XTYP_MONITOR\n"));
        break;
    case XTYP_POKE:
        ATLTRACE(_T("XTYP_POKE\n"));
        if (uFmt == CF_TEXT || uFmt == CF_UNICODETEXT)
            return (JNI_TRUE == m_env.CallBooleanMethod(m_javaDDEServer, ONPOKE, HSZToString(m_idInst, hsz1), HSZToString(m_idInst, hsz2), HDDEDATAToString(hdata, uFmt))) ? (HDDEDATA)DDE_FACK : (HDDEDATA)DDE_FNOTPROCESSED;
        else
            return (JNI_TRUE == m_env.CallBooleanMethod(m_javaDDEServer, ONPOKE, HSZToString(m_idInst, hsz1), HSZToString(m_idInst, hsz2), TDDEData<BYTE>(hdata).ToArray(), uFmt)) ? (HDDEDATA)DDE_FACK : (HDDEDATA)DDE_FNOTPROCESSED;
    case XTYP_ADVREQ:
    case XTYP_REQUEST:
        ATLTRACE( (XTYP_REQUEST == uType) ? _T("XTYP_REQUEST\n") : _T("XTYP_ADVREQ\n"));
        if (uFmt == CF_TEXT || uFmt == CF_UNICODETEXT)
        {
            CString strResult = m_env.CallStringMethod(m_javaDDEServer, ONREQUEST, HSZToString(m_idInst, hsz1), HSZToString(m_idInst, hsz2));

            return StringToHDDEDATA(m_idInst, strResult, hsz2, uFmt);
        }
        else
        {
            CSimpleArray<BYTE> arResult = m_env.CallBytesMethod(m_javaDDEServer, ONREQUEST, HSZToString(m_idInst, hsz1), HSZToString(m_idInst, hsz2), uFmt);

            return DdeCreateDataHandle(m_idInst, (LPBYTE)arResult.GetData(), arResult.GetSize(), 0, hsz2, uFmt, 0);
        }
    case XTYP_SHIFT:
        ATLTRACE(_T("XTYP_SHIFT\n"));
        break;
    case XTYP_WILDCONNECT:
        ATLTRACE(_T("XTYP_WILDCONNECT\n"));
        break;
    case XTYP_ADVDATA:
        ATLTRACE(_T("XTYP_ADVDATA\n"));
        break;
    case XTYP_DISCONNECT:
        ATLTRACE(_T("XTYP_DISCONNECT\n"));
        break;
    case XTYP_XACT_COMPLETE:
        ATLTRACE(_T("XTYP_XACT_COMPLETE\n"));
        break;
    case XTYP_ERROR:
        ATLTRACE(_T("XTYP_ERROR, error code = 0x%X\n"), (DWORD)LOWORD(dwData1));
        break;
    case XTYP_REGISTER:
        ATLTRACE(_T("XTYP_REGISTER\n"));
        break;
    case XTYP_UNREGISTER:
        ATLTRACE(_T("XTYP_UNREGISTER\n"));
        break;
    }
    return 0;
}