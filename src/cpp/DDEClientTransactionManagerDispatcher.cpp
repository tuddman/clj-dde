/*
 * Copyright 2008 www.pretty-tools.com
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
#include "DDEClientTransactionManagerDispatcher.h"

#define WAIT_THREAD_INITIALIZATION 5000
#define WAIT_THREAD_TERMINATION 5000

namespace {

    template <typename T1, typename T2>
    class TPair
    {
    public:
        T1 first;
        T2 second;

        TPair(T1 f, T2 s)
            : first(f)
            , second(s)
        {
        }
    };

    template <typename T1, typename T2, typename T3>
    class TTripple
    {
    public:
        T1 first;
        T2 second;
        T3 third;

        TTripple(T1 f, T2 s, T3 t)
            : first(f)
            , second(s)
            , third(t)
        {
        }
    };

    /**
    * DDE Result or Exception.
    */
    class DDEResultOrException
    {
    private:
        CString m_strData;

        DDEException* m_pException;

    public:
        DDEResultOrException(const CString &strData)
            : m_strData(strData)
            , m_pException(NULL)
        {
        }

        DDEResultOrException(DDEException* pException)
            : m_pException(pException)
        {
        }

        bool IsException() const
        {
            return m_pException != 0;
        }

        const CString &GetData() const
        {
            return m_strData;
        }

        DDEException* GetException() const
        {
            return m_pException;
        }
    };
}

// DDEClientTransactionManagerDispatcher static definition
CriticalSection                         DDEClientTransactionManagerDispatcher::m_crsecInit;
DDEClientTransactionManagerDispatcher*  DDEClientTransactionManagerDispatcher::m_pDispatcher = NULL;

DDEClientTransactionManagerDispatcher::DDEClientTransactionManagerDispatcher()
    : m_env(NULL)
    , m_eStarted(TRUE)
    , m_eInitializationError(TRUE)
    , m_nRefCount(0)
{
    ATLTRACE( _T("DDEClientTransactionManagerDispatcher::DDEClientTransactionManagerDispatcher();\n") );
}

DDEClientTransactionManagerDispatcher::~DDEClientTransactionManagerDispatcher(void)
{
    ATLTRACE( _T("DDEClientTransactionManagerDispatcher::~DDEClientTransactionManagerDispatcher();\n") );
}

LONG DDEClientTransactionManagerDispatcher::AddRef()
{
    LOCK(m_crsecInit);

    return ++m_nRefCount;
}

LONG DDEClientTransactionManagerDispatcher::Release()
{
    LOCK(m_crsecInit);

    LONG lRefCount = --m_nRefCount;

    if (lRefCount > 0)
        return lRefCount;

    m_pDispatcher = NULL;

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

    ATLTRACE( _T("Delete DDEClientTransactionManagerDispatcher instance\n") );
    delete this;

    return lRefCount;
}

void DDEClientTransactionManagerDispatcher::KillWorkingThread()
{
    ATLTRACE( _T("Kill DDE working thread\n") );
    m_thread.Terminate();

    if (::IsWindow(m_hWnd))
        DestroyWindow();
    Detach();
}

void DDEClientTransactionManagerDispatcher::Start() throw (...)
{
    ATLTRACE( _T("Creating DDE working thread\n") );
    if (!m_thread.Create(this))
        throw DDEException(IDS_ERROR_CANNOT_CREATE_THREAD);

    ATLTRACE( _T("DDE working thread was created\n") );

    HANDLE hEvents[] = { (HANDLE)m_eStarted, (HANDLE)m_eInitializationError };

    DWORD dwEvent = WaitForMultipleObjects(sizeof(hEvents)/sizeof(hEvents[0]), hEvents, FALSE, WAIT_THREAD_INITIALIZATION);

    if (dwEvent == WAIT_TIMEOUT) // something went wrong, let's restart
    {
        KillWorkingThread();

        if (!m_thread.Create(this))
            throw DDEException(IDS_ERROR_CANNOT_CREATE_THREAD);

        ATLTRACE( _T("DDE working thread was re-created\n") );

        m_eStarted.Reset();
        m_eInitializationError.Reset();

        dwEvent = WaitForMultipleObjects(sizeof(hEvents)/sizeof(hEvents[0]), hEvents, FALSE, WAIT_THREAD_INITIALIZATION);

        if (dwEvent == WAIT_TIMEOUT) // something still wrong, let's throw exception
        {
            KillWorkingThread();

            throw DDEException(IDS_ERROR_INITIALIZATION_FAILED);
        }
    }

    if (dwEvent == (WAIT_OBJECT_0 + 1))
        throw DDEException(IDS_ERROR_INITIALIZATION_FAILED);
}

void DDEClientTransactionManagerDispatcher::Run()
{
    ATLTRACE( _T("New thread has been started\n") );

    ATLTRACE( _T("Attach thread to JavaVM\n") );
    if (g_vm->AttachCurrentThreadAsDaemon((void**)&m_env, NULL))
    {
        ATLTRACE( _T("Cannot attach current thread\n") );
        m_eInitializationError.Set();
        return;
    }

    m_TransactionManager.SetWJNIEnv(m_env);

    ATLTRACE( _T("Create helper window\n") );
    if (Create(NULL, NULL, NULL, WS_POPUP))
    {
        ATLTRACE( _T("Create helper window was created\n") );

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
}

LRESULT DDEClientTransactionManagerDispatcher::OnInit(UINT, WPARAM, LPARAM)
{
    ATLTRACE( _T("Initializing DDE\n") );

    if (m_TransactionManager.Initialize())
        m_eStarted.Set();
    else
        m_eInitializationError.Set();

    return 0;
}

LRESULT DDEClientTransactionManagerDispatcher::OnShutDown(UINT, WPARAM, LPARAM)
{
    m_TransactionManager.Uninitialize();

    ATLTRACE( _T("Posting QUIT message\n") );
    m_thread.PostMessage(WM_QUIT);

    return 0;
}

void DDEClientTransactionManagerDispatcher::Connect(DDEClientConversation* pConversation) throw (...)
{
    LOCK(m_crsecTransactionManager);

    ATLTRACE( _T("Sending connect message\n") );

    ReThrowDDEException(SendMessage(UWM_CONNECT, (WPARAM)pConversation));
}

LRESULT DDEClientTransactionManagerDispatcher::OnConnect(UINT, WPARAM wpConversation, LPARAM)
{
    _TRY_
    {
        m_TransactionManager.Connect((DDEClientConversation*)wpConversation);
    }
    _CATCH_AND_RETURN_ALL_
}

void DDEClientTransactionManagerDispatcher::Disconnect(DDEClientConversation* pConversation) throw (...)
{
    LOCK(m_crsecTransactionManager);

    ATLTRACE( _T("Sending disconnect message\n") );

    SendMessage(UWM_DISCONNECT, (WPARAM)pConversation);
}

LRESULT DDEClientTransactionManagerDispatcher::OnDisconnect(UINT, WPARAM wpConversation, LPARAM)
{
    _TRY_
    {
        m_TransactionManager.Disconnect((DDEClientConversation*)wpConversation);
    }
    _CATCH_AND_RETURN_ALL_
}

void DDEClientTransactionManagerDispatcher::Execute(DDEClientConversation* pConversation, const CString &strCommand) throw (...)
{
    LOCK(m_crsecTransactionManager);

    ATLTRACE( _T("Sending execute message\n") );

    ReThrowDDEException(SendMessage(UWM_EXECUTE_STR, (WPARAM)pConversation, (LPARAM)(LPCTSTR)strCommand));
}

LRESULT DDEClientTransactionManagerDispatcher::OnExecuteStr(UINT, WPARAM wpConversation, LPARAM command)
{
    _TRY_
    {
        m_TransactionManager.Execute((DDEClientConversation*)wpConversation, (LPCTSTR)command);
    }
    _CATCH_AND_RETURN_ALL_
}

void DDEClientTransactionManagerDispatcher::Execute(DDEClientConversation* pConversation, const CSimpleArray<BYTE> &arrCommand) throw (...)
{
    LOCK(m_crsecTransactionManager);

    ATLTRACE( _T("Sending execute message\n") );

    ReThrowDDEException(SendMessage(UWM_EXECUTE_ARR, (WPARAM)pConversation, (LPARAM)&arrCommand));
}

LRESULT DDEClientTransactionManagerDispatcher::OnExecuteArr(UINT, WPARAM wpConversation, LPARAM command)
{
    _TRY_
    {
        m_TransactionManager.Execute((DDEClientConversation*)wpConversation, *(const CSimpleArray<BYTE> *)command);
    }
    _CATCH_AND_RETURN_ALL_
}

void DDEClientTransactionManagerDispatcher::Poke(DDEClientConversation* pConversation, const CString &strItem, const CString &strData) throw (...)
{
    LOCK(m_crsecTransactionManager);

    ATLTRACE( _T("Sending poke message\n") );

    ReThrowDDEException(SendMessage(UWM_POKE_STR, (WPARAM)pConversation, (LPARAM)&TPair<const CString&, const CString&>(strItem, strData)));
}

LRESULT DDEClientTransactionManagerDispatcher::OnPokeStr(UINT, WPARAM wpConversation, LPARAM data)
{
    const CString &strItem = ((TPair<const CString&, const CString&>*)data)->first;
    const CString &strData = ((TPair<const CString&, const CString&>*)data)->second;

    _TRY_
    {
        m_TransactionManager.Poke((DDEClientConversation*)wpConversation, strItem, strData);
    }
    _CATCH_AND_RETURN_ALL_
}

void DDEClientTransactionManagerDispatcher::Poke(DDEClientConversation* pConversation, const CString &strItem, const CSimpleArray<BYTE> &arData, UINT wFmt) throw (...)
{
    LOCK(m_crsecTransactionManager);

    ATLTRACE( _T("Sending poke message\n") );

    ReThrowDDEException(SendMessage(UWM_POKE_ARR, (WPARAM)pConversation, (LPARAM)&TTripple<const CString&, const CSimpleArray<BYTE> &, UINT>(strItem, arData, wFmt)));
}

LRESULT DDEClientTransactionManagerDispatcher::OnPokeArr(UINT, WPARAM wpConversation, LPARAM data)
{
    const CString &strItem = ((TTripple<const CString&, const CSimpleArray<BYTE> &, UINT>*)data)->first;
    const CSimpleArray<BYTE> &arData = ((TTripple<const CString&, const CSimpleArray<BYTE> &, UINT>*)data)->second;
    UINT wFmt = ((TTripple<const CString&, const CSimpleArray<BYTE> &, UINT>*)data)->third;

    _TRY_
    {
        m_TransactionManager.Poke((DDEClientConversation*)wpConversation, strItem, arData, wFmt);
    }
    _CATCH_AND_RETURN_ALL_
}

CString DDEClientTransactionManagerDispatcher::Request(DDEClientConversation* pConversation, const CString &strItem) throw (...)
{
    LOCK(m_crsecTransactionManager);

    ATLTRACE( _T("Sending request message\n") );

    DDEResultOrException* pResult = (DDEResultOrException*)SendMessage(UWM_SYNC_REQUEST, (WPARAM)pConversation, (LPARAM)(LPCTSTR)strItem);
    if (!pResult)
        throw DDEException(IDS_ERROR_UNKNOWN);

    DDEResultOrException ddeResult(*pResult);

    delete pResult, pResult = 0;

    if (ddeResult.IsException())
        ReThrowDDEException(ddeResult.GetException());

    return ddeResult.GetData();
}

LRESULT DDEClientTransactionManagerDispatcher::OnRequest(UINT, WPARAM wpConversation, LPARAM item)
{
    try
    {
        CString strResult = m_TransactionManager.Request((DDEClientConversation*)wpConversation, (LPCTSTR)item);

        return (LRESULT)new DDEResultOrException(strResult);
    }
    catch (DDEException* e)
    {
        ATLTRACE( _T("Exception: ") _T(__FUNCTION__) _T(" %sn"), e->GetMessage() );
        return (LRESULT)new DDEResultOrException(e);
    }
    catch (...)
    {
        ATLTRACE( _T("Unknown exception ") _T(__FUNCTION__) _T("n") );
        return (LRESULT)new DDEResultOrException(new DDEException(IDS_ERROR_UNKNOWN));
    }

    ATLASSERT(FALSE);
}

void DDEClientTransactionManagerDispatcher::StartAdvice(DDEClientConversation* pConversation, const CString &strItem) throw (...)
{
    LOCK(m_crsecTransactionManager);

    ATLTRACE( _T("Sending StartAdvice message\n") );

    ReThrowDDEException(SendMessage(UWM_START_ADVICE, (WPARAM)pConversation, (LPARAM)(LPCTSTR)strItem));
}

LRESULT DDEClientTransactionManagerDispatcher::OnStartAdvice(UINT, WPARAM wpConversation, LPARAM item)
{
    _TRY_
    {
        m_TransactionManager.StartAdvice((DDEClientConversation*)wpConversation, (LPCTSTR)item);
    }
    _CATCH_AND_RETURN_ALL_
}

void DDEClientTransactionManagerDispatcher::StopAdvice(DDEClientConversation* pConversation, const CString &strItem) throw (...)
{
    LOCK(m_crsecTransactionManager);

    ATLTRACE( _T("Sending StopAdvice message\n") );

    ReThrowDDEException(SendMessage(UWM_STOP_ADVICE, (WPARAM)pConversation, (LPARAM)(LPCTSTR)strItem));
}

LRESULT DDEClientTransactionManagerDispatcher::OnStopAdvice(UINT, WPARAM wpConversation, LPARAM item)
{
    _TRY_
    {
        m_TransactionManager.StopAdvice((DDEClientConversation*)wpConversation, (LPCTSTR)item);
    }
    _CATCH_AND_RETURN_ALL_
}

HDDEDATA CALLBACK DDEClientTransactionManagerDispatcher::DdeClientCallback(UINT uType,
                                                                           UINT uFmt,
                                                                           HCONV hconv,
                                                                           HDDEDATA hsz1,
                                                                           HDDEDATA hsz2,
                                                                           HDDEDATA hdata,
                                                                           HDDEDATA dwData1,
                                                                           HDDEDATA dwData2
                                                                           )
{
    return m_pDispatcher ? m_pDispatcher->m_TransactionManager.DdeClientCallback(uType,uFmt,hconv,hsz1,hsz2,hdata,dwData1,dwData2) : 0;
}

void DDEClientTransactionManagerDispatcher::ReThrowDDEException(LRESULT pException)
{
    if (pException) ReThrowDDEException((DDEException*)pException);
}

void DDEClientTransactionManagerDispatcher::ReThrowDDEException(DDEException* pException)
{
    DDEMLException* pDDEMLException = dynamic_cast<DDEMLException*>(pException);
    if (pDDEMLException)
        throw pDDEMLException;
    else
        throw pException;
}
