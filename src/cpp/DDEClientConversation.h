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

#include "DDEException.h"

class DDEClientTransactionManagerDispatcher;

/**
 * DDEClientConversation
 *
 * @author Alexander Kozlov
 */
class DDEClientConversation
{
private: friend class DDEClientTransactionManager;
    jobject m_javaDDEClientConversation;

    CString m_strService;
    CString m_strTopic;
    int     m_timeout;
    int     m_textFormat;

    HCONV m_hConv;
    SmartPtr<DDEClientTransactionManagerDispatcher> m_dispatcher;

public:
    DDEClientConversation(WJNIEnv env, jobject globalRefObject) throw (...);

    void Release(WJNIEnv env);

    ~DDEClientConversation(void);

    void Connect(const CString &strService, const CString &strTopic) throw (...);
    void Disconnect() throw (...);

    void Execute(const CString &strCommand) throw (...);
    void Execute(const CSimpleArray<BYTE> &arCommand) throw (...);
    void Poke(const CString &strItem, const CString &strData) throw (...);
    void Poke(const CString &strItem, const CSimpleArray<BYTE> &arData, UINT wFmt) throw (...);

    CString Request(const CString &strItem) throw (...);

    void StartAdvice(const CString &strItem) throw (...);
    void StopAdvice(const CString &strItem) throw (...);

public: // callbacks
    void OnDisconnect(WJNIEnv env);
    void OnAdvData(WJNIEnv env, LPCTSTR pszTopic, LPCTSTR pszItem, LPCTSTR pszData);

public:
    static const char * DDEClientConversationFieldName;
    static DDEClientConversation* GetDDEClient(WJNIEnv env, jobject object);
};
