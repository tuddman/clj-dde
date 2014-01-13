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
#pragma once

#include "DDEClientTransactionManager.h"

#define UWM_INIT_DDE        (WM_USER + 555)
#define UWM_SHUTDOWN_DDE    (WM_USER + 666)
#define UWM_CONNECT         (WM_USER + 600)
#define UWM_DISCONNECT      (WM_USER + 601)
#define UWM_EXECUTE_STR     (WM_USER + 602)
#define UWM_EXECUTE_ARR     (WM_USER + 603)
#define UWM_POKE_STR        (WM_USER + 604)
#define UWM_POKE_ARR        (WM_USER + 605)
#define UWM_SYNC_REQUEST    (WM_USER + 606)
#define UWM_START_ADVICE    (WM_USER + 607)
#define UWM_STOP_ADVICE     (WM_USER + 608)

/**
 * Dispatches calls to DDEClientTransactionManager to working thread.
 *
 * @author Alexander Kozlov (alex@pretty-tools.com)
 */
class DDEClientTransactionManagerDispatcher
    : public CWindowImpl<DDEClientTransactionManagerDispatcher>
    , protected Runnable
{
private:
    WJNIEnv m_env;

    Thread   m_thread;
    Event    m_eStarted;
    Event    m_eInitializationError;

    CriticalSection             m_crsecTransactionManager;
    DDEClientTransactionManager m_TransactionManager;

protected:
    DDEClientTransactionManagerDispatcher(void);

public:
    ~DDEClientTransactionManagerDispatcher(void);

    void Connect(DDEClientConversation* pConversation) throw (...);
    void Disconnect(DDEClientConversation* pConversation) throw (...);

    void Execute(DDEClientConversation* pConversation, const CString &strCommand) throw (...);
    void Execute(DDEClientConversation* pConversation, const CSimpleArray<BYTE> &arCommand) throw (...);
    void Poke(DDEClientConversation* pConversation, const CString &strItem, const CString &strData) throw (...);
    void Poke(DDEClientConversation* pConversation, const CString &strItem, const CSimpleArray<BYTE> &arData, UINT wFmt) throw (...);

    CString Request(DDEClientConversation* pConversation, const CString &strItem) throw (...);

    void StartAdvice(DDEClientConversation* pConversation, const CString &strItem) throw (...);
    void StopAdvice(DDEClientConversation* pConversation, const CString &strItem) throw (...);

public:
    static HDDEDATA CALLBACK DdeClientCallback( UINT uType,
                                                UINT uFmt,
                                                HCONV hconv,
                                                HDDEDATA hsz1,
                                                HDDEDATA hsz2,
                                                HDDEDATA hdata,
                                                HDDEDATA dwData1,
                                                HDDEDATA dwData2
                                                );

protected: friend class SmartPtr<DDEClientTransactionManagerDispatcher>;
    LONG AddRef();
    LONG Release();

private:
    LONG volatile m_nRefCount;

public:
    static SmartPtr<DDEClientTransactionManagerDispatcher> Instance() throw (...)
    {
        LOCK(m_crsecInit);

        if (m_pDispatcher)
            return m_pDispatcher;

        SmartPtr<DDEClientTransactionManagerDispatcher> dispatcher = new DDEClientTransactionManagerDispatcher();
        // start thread
        dispatcher->Start();
        // if started successfully save it
        m_pDispatcher = dispatcher;

        return dispatcher;
    }

private:
    static CriticalSection                           m_crsecInit;
    static DDEClientTransactionManagerDispatcher*    m_pDispatcher;

private:
    void Start() throw (...);
    void KillWorkingThread();

public:
    DECLARE_WND_CLASS( _T("JavaDDEProcessorWnd") )

protected:
    BEGIN_MSG_MAP_EX(DDEClientTransactionManager)
        MESSAGE_HANDLER_EX(UWM_SYNC_REQUEST, OnRequest)
        MESSAGE_HANDLER_EX(UWM_EXECUTE_STR, OnExecuteStr)
        MESSAGE_HANDLER_EX(UWM_EXECUTE_ARR, OnExecuteArr)
        MESSAGE_HANDLER_EX(UWM_POKE_STR, OnPokeStr)
        MESSAGE_HANDLER_EX(UWM_POKE_ARR, OnPokeArr)
        MESSAGE_HANDLER_EX(UWM_CONNECT, OnConnect)
        MESSAGE_HANDLER_EX(UWM_DISCONNECT, OnDisconnect)
        MESSAGE_HANDLER_EX(UWM_START_ADVICE, OnStartAdvice)
        MESSAGE_HANDLER_EX(UWM_STOP_ADVICE, OnStopAdvice)
        MESSAGE_HANDLER_EX(UWM_INIT_DDE, OnInit)
        MESSAGE_HANDLER_EX(UWM_SHUTDOWN_DDE, OnShutDown)
    END_MSG_MAP()

    LRESULT OnInit(UINT, WPARAM, LPARAM);
    LRESULT OnShutDown(UINT, WPARAM, LPARAM);
    LRESULT OnConnect(UINT, WPARAM, LPARAM);
    LRESULT OnDisconnect(UINT, WPARAM, LPARAM);
    LRESULT OnExecuteStr(UINT, WPARAM, LPARAM);
    LRESULT OnExecuteArr(UINT, WPARAM, LPARAM);
    LRESULT OnPokeStr(UINT, WPARAM, LPARAM);
    LRESULT OnPokeArr(UINT, WPARAM, LPARAM);
    LRESULT OnRequest(UINT, WPARAM, LPARAM);
    LRESULT OnStartAdvice(UINT, WPARAM, LPARAM);
    LRESULT OnStopAdvice(UINT, WPARAM, LPARAM);

protected:
    void ReThrowDDEException(LRESULT pException);
    void ReThrowDDEException(DDEException* pException);

protected:
    virtual void Run();
};
