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

/**
 * DDEClientConversationException
 */
class DDEException
{
protected:
    CString m_strMessage;

public:
    DDEException(UINT nMessage)
    {
        m_strMessage.LoadString(nMessage);
    }

    DDEException(LPCTSTR pszMessage)
        : m_strMessage(pszMessage)
    {
    }

    virtual ~DDEException()
    {
    }

    const CString &GetMessage() const
    {
        return m_strMessage;
    }
};

/**
 * DDEMLException
 */
class DDEMLException : public DDEException
{
private:
    int m_iCode;

public:
    DDEMLException(int iCode, UINT nMessage)
        : DDEException(nMessage)
        , m_iCode(iCode)
    {
        ReplaceMessageWithDDEMLDescription(iCode);
    }

    DDEMLException(int iCode, LPCTSTR pszMessage)
        : DDEException(pszMessage)
        , m_iCode(iCode)
    {
        ReplaceMessageWithDDEMLDescription(iCode);
    }

    virtual ~DDEMLException()
    {
    }

    int GetCode() const
    {
        return m_iCode;
    }

private:
    void ReplaceMessageWithDDEMLDescription(int iCode)
    {
        if (!(m_iCode >= 0x4000 && m_iCode <= 0x4011))
            return;

        CString strDDEMLDescription;
        if (!strDDEMLDescription.LoadString(m_iCode))
            return;

        m_strMessage = strDDEMLDescription;
    }
};

void ThrowNewDDEException(WJNIEnv env, LPCTSTR pszMessage);
void ThrowNewDDEException(WJNIEnv env, const DDEException& ex);
void ThrowNewDDEMLException(WJNIEnv env, const DDEMLException& ex);

#define _TRY_       try
#define _CATCH_ALL_ catch (DDEMLException* e)\
                    {\
                        ThrowNewDDEMLException(env, *e);\
                        delete e;\
                    }\
                    catch (DDEException& e)\
                    {\
                        ThrowNewDDEException(env, e);\
                    }\
                    catch (DDEException* e)\
                    {\
                        ThrowNewDDEException(env, *e);\
                        delete e;\
                    }\
                    catch (...)\
                    {\
                        ThrowNewDDEException(env, GetString(IDS_ERROR_UNKNOWN));\
                    }

#define _CATCH_AND_RETURN_ALL_  catch (DDEMLException* e)\
                                {\
                                    ATLTRACE( _T("Exception: ") _T(__FUNCTION__) _T(" 0x%X %s\n"), e->GetCode(), e->GetMessage() );\
                                    return (LRESULT)e;\
                                }\
                                catch (DDEException& e)\
                                {\
                                    ATLTRACE( _T("Exception: ") _T(__FUNCTION__) _T(" %s\n"), e.GetMessage() );\
                                    return (LRESULT)new DDEException(e);\
                                }\
                                catch (DDEException* e)\
                                {\
                                    ATLTRACE( _T("Exception: ") _T(__FUNCTION__) _T(" %s\n"), e->GetMessage() );\
                                    return (LRESULT)e;\
                                }\
                                catch (...)\
                                {\
                                    ATLTRACE( _T("Unknown exception ") _T(__FUNCTION__) _T("\n") );\
                                    return (LRESULT)new DDEException(IDS_ERROR_UNKNOWN);\
                                }\
                                return 0;
