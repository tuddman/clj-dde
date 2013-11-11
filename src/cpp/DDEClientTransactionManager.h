#pragma once

#include "DDEClientConversation.h"

/**
 * DDE Client Transaction Manager.
 *
 * @author Alexander Kozlov (alex@pretty-tools.com)
 */
class DDEClientTransactionManager
{
private:
    WJNIEnv m_env;

    DWORD m_idInst;

    CAtlMap<HCONV, DDEClientConversation*> m_mapHConvToDDEClient;

public:
    DDEClientTransactionManager(void);
    ~DDEClientTransactionManager(void);

    bool Initialize();
    void Uninitialize();

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
    void SetWJNIEnv(WJNIEnv env)
    {
        m_env = env;
    }

    HDDEDATA DdeClientCallback( UINT uType,
                                UINT uFmt,
                                HCONV hconv,
                                HDDEDATA hsz1,
                                HDDEDATA hsz2,
                                HDDEDATA hdata,
                                HDDEDATA dwData1,
                                HDDEDATA dwData2
                                );
};
