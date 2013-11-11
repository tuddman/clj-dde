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

#pragma once

#include "DDEException.h"

#define UWM_INIT_DDE        (WM_USER + 555)
#define UWM_SHUTDOWN_DDE    (WM_USER + 666)
#define UWM_NAMESERVICE     (WM_USER + 700)
#define UWM_POSTADVISE      (WM_USER + 701)

class DDEServer
    : public CWindowImpl<DDEServer>
    , protected Runnable
{
private:
    jobject m_javaDDEServer;
    CString m_strService;

    WJNIEnv m_env; // JNIEnv attached to thread

    Thread   m_thread;
    Event    m_eStarted;
    Event    m_eInitializationError;

    DWORD    m_idInst;
    HSZ      m_hszService;

public:
    DDEServer(const CString &strService, WJNIEnv env, jobject globalRefObject) throw (...);
    
    void Start() throw (...);
    void Release(WJNIEnv env);

    ~DDEServer(void);

    void PostAdvise(const CString& strTopic, const CString& strItem) throw (...);

public:
    static DWORD m_dwTlsIndex;
    static BOOL SetThis(DDEServer* _this);
    static DDEServer* GetThis();

    static HDDEDATA CALLBACK DdeServerCallback( UINT uType,
                                                UINT uFmt,
                                                HCONV hconv,
                                                HSZ hsz1,
                                                HSZ hsz2,
                                                HDDEDATA hdata,
                                                HDDEDATA dwData1,
                                                HDDEDATA dwData2
                                                );


public:
    DECLARE_WND_CLASS( _T("JavaDDEServerWnd") )

protected:
    BEGIN_MSG_MAP_EX(DDEServer)
        MESSAGE_HANDLER_EX(UWM_POSTADVISE, OnPostAdvise)
        MESSAGE_HANDLER_EX(UWM_NAMESERVICE, OnNameService)
        MESSAGE_HANDLER_EX(UWM_INIT_DDE, OnInit)
        MESSAGE_HANDLER_EX(UWM_SHUTDOWN_DDE, OnShutDown)
    END_MSG_MAP()

    LRESULT OnInit(UINT, WPARAM, LPARAM);
    LRESULT OnShutDown(UINT, WPARAM, LPARAM);
    LRESULT OnNameService(UINT, WPARAM, LPARAM);
    LRESULT OnPostAdvise(UINT, WPARAM, LPARAM);

protected:
    HDDEDATA DdeCallback(   UINT uType,
                            UINT uFmt,
                            HCONV hconv,
                            HSZ hsz1,
                            HSZ hsz2,
                            HDDEDATA hdata,
                            HDDEDATA dwData1,
                            HDDEDATA dwData2
                            );
protected:
    void KillWorkingThread();

    void ReThrowDDEException(LRESULT pException);
    void ReThrowDDEException(DDEException* pException);

public:
    static const char * DDEServerFieldName;
    static DDEServer* GetDDEServer(WJNIEnv env, jobject object);

protected:
    virtual void Run();
};
