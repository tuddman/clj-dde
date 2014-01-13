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
#include "DDEClientConversation.h"
#include "DDEClientTransactionManagerDispatcher.h"

const char * DDEClientConversation::DDEClientConversationFieldName = "nativeDDEClientConversation";

#define TIMEOUT_DEFAULT                      2000
#define TIMEOUT_FIELDNAME                    "timeout"
#define TEXTFORMAT_DEFAULT                   CF_TEXT
#define TEXTFORMAT_FIELDNAME                 "textFormat"
#define FIREEVENTONDISCONNECT                "fireEventOnDisconnect"
#define FIREEVENTONADVDATA                   "fireEventOnAdvData"

DDEClientConversation::DDEClientConversation(WJNIEnv env, jobject globalRefObject) throw (...)
    : m_javaDDEClientConversation(globalRefObject)
    , m_hConv(NULL)
    , m_timeout(TIMEOUT_DEFAULT)
    , m_textFormat(TEXTFORMAT_DEFAULT)
{
    ATLTRACE( _T("DDEClientConversation::DDEClientConversation()\n") );

    if ((m_timeout = env.GetIntField(globalRefObject, TIMEOUT_FIELDNAME)) <= 0)
        m_timeout = TIMEOUT_DEFAULT;

    m_textFormat = env.GetIntField(globalRefObject, TEXTFORMAT_FIELDNAME);
    if (m_textFormat != CF_TEXT && m_textFormat != CF_UNICODETEXT)
        m_textFormat = TEXTFORMAT_DEFAULT;

    m_dispatcher = DDEClientTransactionManagerDispatcher::Instance();
}

DDEClientConversation::~DDEClientConversation(void)
{
    ATLTRACE( _T("DDEClientConversation::~DDEClientConversation()\n") );
}

void DDEClientConversation::Release(WJNIEnv env)
{
    ATLTRACE( _T("DDEClientConversation::Release(env)\n") );
    try
    {
        ATLTRACE( _T("Delete Global Ref to java DDEClientConversation\n") );

        if (m_javaDDEClientConversation) env->DeleteGlobalRef(m_javaDDEClientConversation);
    }
    catch (...)
    {
        ATLTRACE( _T("Exception on DeleteGlobalRef\n") );
    }

    ATLTRACE( _T("Delete DDEClientConversation\n") );
    delete this;
}

DDEClientConversation* DDEClientConversation::GetDDEClient(WJNIEnv env, jobject object) throw (...)
{
    DDEClientConversation* pConversation = (DDEClientConversation*)env.GetLongField(object, DDEClientConversationFieldName);

    if (!pConversation)
        throw DDEException(IDS_ERROR_CLIENT_IS_NOT_CONNECTED);

    return pConversation;
}

void DDEClientConversation::Connect(const CString &strService, const CString &strTopic) throw (...)
{
    ATLTRACE( _T("DDEClientConversation::Connect('%s', '%s')\n"), strService, strTopic );
    m_strService = strService;
    m_strTopic = strTopic;

    m_dispatcher->Connect(this);
}

void DDEClientConversation::Disconnect() throw (...)
{
    ATLTRACE( _T("DDEClientConversation::Disconnect()\n") );

    m_dispatcher->Disconnect(this);
}

void DDEClientConversation::Execute(const CString &strCommand) throw (...)
{
    m_dispatcher->Execute(this, strCommand);
}

void DDEClientConversation::Execute(const CSimpleArray<BYTE> &arCommand) throw (...)
{
    m_dispatcher->Execute(this, arCommand);
}

void DDEClientConversation::Poke(const CString &strItem, const CString &strData) throw (...)
{
    m_dispatcher->Poke(this, strItem, strData);
}

void DDEClientConversation::Poke(const CString &strItem, const CSimpleArray<BYTE> &arData, UINT wFmt) throw (...)
{
    m_dispatcher->Poke(this, strItem, arData, wFmt);
}

CString DDEClientConversation::Request(const CString &strItem) throw (...)
{
    return m_dispatcher->Request(this, strItem);
}

void DDEClientConversation::StartAdvice(const CString &strItem) throw (...)
{
    m_dispatcher->StartAdvice(this, strItem);
}

void DDEClientConversation::StopAdvice(const CString &strItem) throw (...)
{
    m_dispatcher->StopAdvice(this, strItem);
}

void DDEClientConversation::OnDisconnect(WJNIEnv env)
{
    env.CallVoidMethod(m_javaDDEClientConversation, FIREEVENTONDISCONNECT);
}

void DDEClientConversation::OnAdvData(WJNIEnv env, LPCTSTR pszTopic, LPCTSTR pszItem, LPCTSTR pszData)
{
    env.CallVoidMethod(m_javaDDEClientConversation, FIREEVENTONADVDATA, pszTopic, pszItem, pszData);
}
