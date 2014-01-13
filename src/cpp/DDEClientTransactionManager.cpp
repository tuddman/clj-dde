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
#include "resource.h"
#include "DDEClientTransactionManager.h"
#include "DDEClientTransactionManagerDispatcher.h"

DDEClientTransactionManager::DDEClientTransactionManager(void)
    : m_env(NULL)
    , m_idInst(0)
{
    ATLTRACE( _T("DDEClientTransactionManager::DDEClientTransactionManager();\n") );
}

DDEClientTransactionManager::~DDEClientTransactionManager(void)
{
    ATLTRACE( _T("DDEClientTransactionManager::~DDEClientTransactionManager();\n") );
}

bool DDEClientTransactionManager::Initialize()
{
    ATLTRACE( _T("Initializing DDE\n") );

    UINT uRes = DdeInitialize(&m_idInst,                                                               // receives instance identifier
                             (PFNCALLBACK)DDEClientTransactionManagerDispatcher::DdeClientCallback,    // pointer to callback function
                             APPCMD_CLIENTONLY |MF_SENDMSGS | MF_POSTMSGS | APPCMD_FILTERINITS,
                             0);

#ifdef _DEBUG
    if (DMLERR_NO_ERROR != uRes)
    {
        ATLTRACE( _T("Cannot initialize DDE: 0x%X\n"), uRes );
    }
    else
    {
        ATLTRACE( _T("DDE was initialized: idInst=0x%X\n"), m_idInst );
    }
#endif // _DEBUG

    return DMLERR_NO_ERROR == uRes;
}

void DDEClientTransactionManager::Uninitialize()
{
    ATLTRACE( _T("DDE Uninitializing...\n") );
    if (m_idInst && !DdeUninitialize(m_idInst))
    {
        ATLTRACE( _T("Cannot uninitialize DDE\n") );
    }
}

void DDEClientTransactionManager::Connect(DDEClientConversation* pConversation) throw (...)
{
    CONVCONTEXT convCtx;
    ZeroMemory(&convCtx, sizeof(CONVCONTEXT));
    convCtx.cb = sizeof(CONVCONTEXT);
    convCtx.iCodePage = CSTRING_CODE_PAGE;

    CHSZ hszService, hszTopic;
    if (!pConversation->m_strService.IsEmpty()) hszService.Set(m_idInst, pConversation->m_strService);
    if (!pConversation->m_strTopic.IsEmpty()) hszTopic.Set(m_idInst, pConversation->m_strTopic);

    ATLTRACE( _T("Connect to service: '%s', topic: '%s'\n"), pConversation->m_strService, pConversation->m_strTopic );
    if (!(pConversation->m_hConv = DdeConnect(m_idInst, hszService, hszTopic, &convCtx)))
        throw new DDEMLException(DdeGetLastError(m_idInst), IDS_ERROR_CANNOT_CONNECT_TO_DDE_SERVICE);

    ATLTRACE( _T("DDE was successfully connected: hConv=0x%X\n"), pConversation->m_hConv );
    m_mapHConvToDDEClient[pConversation->m_hConv] = pConversation;
}

void DDEClientTransactionManager::Disconnect(DDEClientConversation* pConversation) throw (...)
{
    // remove from events map
    m_mapHConvToDDEClient.RemoveKey(pConversation->m_hConv);

    ATLTRACE( _T("DDE Disconnecting...\n") );
    if (pConversation->m_hConv && !DdeDisconnect(pConversation->m_hConv))
    {
        ATLTRACE( _T("Cannot disconnect service\n") );
    }
}

void DDEClientTransactionManager::Execute(DDEClientConversation* pConversation, const CString &strCommand) throw (...)
{
    HDDEDATA hCommand = StringToHDDEDATA(m_idInst, strCommand, 0, pConversation->m_textFormat);
    HDDEDATA hDdeData = DdeClientTransaction((LPBYTE)hCommand, -1, pConversation->m_hConv, 0, 0, XTYP_EXECUTE, pConversation->m_timeout, 0);
    if (!hDdeData)
        throw new DDEMLException(DdeGetLastError(m_idInst), IDS_ERROR_EXECUTE_FAILED);
}

void DDEClientTransactionManager::Execute(DDEClientConversation* pConversation, const CSimpleArray<BYTE> &arCommand) throw (...)
{
    HDDEDATA hDdeData = DdeClientTransaction(arCommand.GetData(), arCommand.GetSize(), pConversation->m_hConv, 0, 0, XTYP_EXECUTE, pConversation->m_timeout, 0);
    if (!hDdeData)
        throw new DDEMLException(DdeGetLastError(m_idInst), IDS_ERROR_EXECUTE_FAILED);
}

void DDEClientTransactionManager::Poke(DDEClientConversation* pConversation, const CString &strItem, const CString &strData) throw (...)
{
    CHSZ hszItem(m_idInst, strItem);
    HDDEDATA hData = StringToHDDEDATA(m_idInst, strData, hszItem, pConversation->m_textFormat);
    HDDEDATA hDdeData = DdeClientTransaction((LPBYTE)hData, -1, pConversation->m_hConv, hszItem, pConversation->m_textFormat, XTYP_POKE, pConversation->m_timeout, 0);
    if (!hDdeData)
        throw new DDEMLException(DdeGetLastError(m_idInst), IDS_ERROR_POKE_FAILED);
}

void DDEClientTransactionManager::Poke(DDEClientConversation* pConversation, const CString &strItem, const CSimpleArray<BYTE> &arData, UINT wFmt) throw (...)
{
    HDDEDATA hDdeData = DdeClientTransaction(arData.GetData(), arData.GetSize(), pConversation->m_hConv, CHSZ(m_idInst, strItem), wFmt, XTYP_POKE, pConversation->m_timeout, 0);
    if (!hDdeData)
        throw new DDEMLException(DdeGetLastError(m_idInst), IDS_ERROR_POKE_FAILED);
}

CString DDEClientTransactionManager::Request(DDEClientConversation* pConversation, const CString &strItem) throw (...)
{
    HDDEDATA hDdeData = DdeClientTransaction(0, 0, pConversation->m_hConv, CHSZ(m_idInst, strItem), pConversation->m_textFormat, XTYP_REQUEST, pConversation->m_timeout, 0);
    if (!hDdeData)
        throw new DDEMLException(DdeGetLastError(m_idInst), IDS_ERROR_REQUEST_FAILED);

    CString strData = HDDEDATAToString(hDdeData, pConversation->m_textFormat);

    DdeFreeDataHandle(hDdeData);

    return strData;
}

void DDEClientTransactionManager::StartAdvice(DDEClientConversation* pConversation, const CString &strItem) throw (...)
{
    HDDEDATA hDdeData = DdeClientTransaction(0, 0, pConversation->m_hConv, CHSZ(m_idInst, strItem), pConversation->m_textFormat, XTYP_ADVSTART, pConversation->m_timeout, 0);
    if (!hDdeData)
        throw new DDEMLException(DdeGetLastError(m_idInst), IDS_ERROR_ADVICE_FAILED);
}

void DDEClientTransactionManager::StopAdvice(DDEClientConversation* pConversation, const CString &strItem) throw (...)
{
    HDDEDATA hDdeData = DdeClientTransaction(0, 0, pConversation->m_hConv, CHSZ(m_idInst, strItem), pConversation->m_textFormat, XTYP_ADVSTOP, pConversation->m_timeout, 0);
    if (!hDdeData)
        throw new DDEMLException(DdeGetLastError(m_idInst), IDS_ERROR_ADVICE_FAILED);
}

HDDEDATA DDEClientTransactionManager::DdeClientCallback(   UINT uType,
                                                           UINT uFmt,
                                                           HCONV hconv,
                                                           HDDEDATA hsz1,
                                                           HDDEDATA hsz2,
                                                           HDDEDATA hdata,
                                                           HDDEDATA dwData1,
                                                           HDDEDATA dwData2
                                                           )
{
    switch (uType)
    {
    case XTYP_ADVDATA:
        ATLTRACE(_T("XTYP_ADVDATA\n"));
        {
            DDEClientConversation* pConversation = NULL;
            if (m_mapHConvToDDEClient.Lookup(hconv, pConversation) && pConversation)
            {
                CString strTopic(HSZToString(m_idInst, (HSZ)hsz1));
                CString strItem(HSZToString(m_idInst, (HSZ)hsz2));

                pConversation->OnAdvData(m_env, strTopic, strItem, hdata ? HDDEDATAToString(hdata, uFmt) : NULL);
            }
        }
        break;
    case XTYP_DISCONNECT:
        ATLTRACE(_T("XTYP_DISCONNECT\n"));
        {
            DDEClientConversation* pConversation = NULL;
            if (m_mapHConvToDDEClient.Lookup(hconv, pConversation) && pConversation)
                pConversation->OnDisconnect(m_env);
        }
        break;
    case XTYP_XACT_COMPLETE:
        ATLTRACE(_T("XTYP_XACT_COMPLETE\n"));
        break;
    case XTYP_ERROR:
        ATLTRACE(_T("XTYP_ERROR, error code = 0x%X\n"), (DWORD)LOWORD(dwData1));
        break;
    case XTYP_REGISTER:
        ATLTRACE(_T("XTYP_REGISTER\n"));
#ifdef _DEBUG
        ATLTRACE(_T("Base service name: %s\n"), HSZToString(m_idInst, (HSZ)hsz1));
        ATLTRACE(_T("Instance-specific service name: %s\n"), HSZToString(m_idInst, (HSZ)hsz2));
#endif // _DEBUG
        break;
    case XTYP_UNREGISTER:
        ATLTRACE(_T("XTYP_UNREGISTER\n"));
#ifdef _DEBUG
        ATLTRACE(_T("Base service name: %s\n"), HSZToString(m_idInst, (HSZ)hsz1));
        ATLTRACE(_T("Instance-specific service name: %s\n"), HSZToString(m_idInst, (HSZ)hsz2));
#endif // _DEBUG
        break;

    // server side callbacks
    case XTYP_ADVSTART:
        ATLTRACE(_T("XTYP_ADVSTART\n"));
        break;
    case XTYP_ADVSTOP:
        ATLTRACE(_T("XTYP_ADVSTOP\n"));
        break;
    case XTYP_CONNECT:
        ATLTRACE(_T("XTYP_CONNECT\n"));
        break;
    case XTYP_CONNECT_CONFIRM:
        ATLTRACE(_T("XTYP_CONNECT_CONFIRM\n"));
        break;
    case XTYP_EXECUTE:
        ATLTRACE(_T("XTYP_EXECUTE\n"));
        break;
    case XTYP_MASK:
        ATLTRACE(_T("XTYP_MASK\n"));
        break;
    case XTYP_MONITOR:
        ATLTRACE(_T("XTYP_MONITOR\n"));
        break;
    case XTYP_POKE:
        ATLTRACE(_T("XTYP_POKE\n"));
        break;
    case XTYP_REQUEST:
        ATLTRACE(_T("XTYP_REQUEST\n"));
        break;
    case XTYP_SHIFT:
        ATLTRACE(_T("XTYP_SHIFT\n"));
        break;
    case XTYP_WILDCONNECT:
        ATLTRACE(_T("XTYP_WILDCONNECT\n"));
        break;
    }
    return 0;
}
