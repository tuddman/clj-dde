#pragma once

#include <dde.h>
#include <ddeml.h>

namespace API {
namespace Win32 {

/**
 * DDE Data Access Helper.
 */
template <typename T>
class TDDEData
{
protected:
    T*       m_pT;
    DWORD    m_dwSize;
    HDDEDATA m_hDdeData;

public:
    TDDEData(HDDEDATA hDdeData)
        : m_hDdeData(hDdeData)
    {
        m_pT = (T*)DdeAccessData(hDdeData, &m_dwSize);
    }

    ~TDDEData()
    {
        if (m_pT) DdeUnaccessData(m_hDdeData);
    }

    void UnaccessData()
    {
        if (m_pT) DdeUnaccessData(m_hDdeData), m_pT = 0;
    }

    DWORD GetSize() const
    {
        return m_dwSize;
    }

    operator T* () const
    {
        return m_pT;
    }


    T &operator * () const
    {
        return *m_pT;
    }

    operator HDDEDATA () const
    {
        return m_hDdeData;
    }

    BOOL operator ! ()
    {
        return !m_pT;
    }

    CSimpleArray<T> ToArray() const
    {
        CSimpleArray<T> arData;

        for (DWORD i = 0; i < GetSize() / sizeof(T); ++i)
            arData.Add(m_pT[i]);

        return arData;
    }
};

/**
 * DDE Data Access Helper.
 */
template <typename T>
class TDDESmartPtr : public TDDEData<T>
{
public:
    TDDESmartPtr(HDDEDATA hDdeData)
        : TDDEData<T>(hDdeData)
    {
    }

    T* operator ->() const
    {
        return m_pT;
    }
};

// String encoding depending on project chracter set
#ifdef _UNICODE
    #define CSTRING_CODE_PAGE CP_WINUNICODE
#else
    #define CSTRING_CODE_PAGE CP_WINANSI
#endif // _UNICODE

/**
 * Convert HSZ To String.
 */
inline CString HSZToString(DWORD idInst, HSZ hsz)
{
    if (!hsz)
        return CString();

    DWORD cb = DdeQueryString(idInst, hsz, (LPTSTR)NULL, 0, CSTRING_CODE_PAGE); 
    if (!cb)
        return CString();

    CString strValue;

    if (!(cb = DdeQueryString(idInst, hsz, strValue.GetBuffer(cb), cb+1, CSTRING_CODE_PAGE)))
        return CString();

    strValue.ReleaseBuffer(cb);
    return strValue;
}

/**
 * Convert HDDEDATA To String.
 */
inline CString HDDEDATAToString(HDDEDATA hDdeData, UINT wFmt)
{
    if (!hDdeData)
        return CString();

    if (CF_TEXT == wFmt)
    {
        TDDEData<char> data(hDdeData);

        CStringA strData;
        memcpy(strData.GetBuffer(data.GetSize()), (char*)data, data.GetSize());
        strData.ReleaseBuffer(data.GetSize());

#ifdef _UNICODE
        return (CString)strData;
#else
        return strData;
#endif // _UNICODE
    }
    else if (CF_UNICODETEXT == wFmt)
    {
        TDDEData<wchar_t> data(hDdeData);

        CStringW strData;
        memcpy(strData.GetBuffer(data.GetSize()/sizeof(wchar_t)), (wchar_t*)data, data.GetSize());
        strData.ReleaseBuffer(data.GetSize()/sizeof(wchar_t));

#ifdef _UNICODE
        return strData;
#else
        return (CString)strData;
#endif // _UNICODE
    }

    return CString();
}

/**
 * Convert String to HDDEDATA.
 */
inline HDDEDATA StringToHDDEDATA(DWORD idInst, const CString &strData, HSZ hszItem, UINT wFmt)
{
    if (CF_TEXT == wFmt)
    {
#ifdef _UNICODE
        CStringA strDataA(strData);
#else
        const CStringA &strDataA(strData);
#endif // _UNICODE
        return DdeCreateDataHandle(idInst, (LPBYTE)(LPCSTR)strDataA, strDataA.GetLength()+1, 0, hszItem, CF_TEXT, 0);
    }
    else if (CF_UNICODETEXT == wFmt)
    {
#ifdef _UNICODE
        const CStringW &strDataW(strData);
#else
        CStringW strDataW(strData);
#endif // _UNICODE
        return DdeCreateDataHandle(idInst, (LPBYTE)(LPCWSTR)strDataW, (strDataW.GetLength()+1) * sizeof(WCHAR), 0, hszItem, CF_UNICODETEXT, 0);
    }

    return NULL;
}

/**
 * DDE String handle.
 */
class CHSZ
{
private:
    HSZ m_hsz;
    DWORD m_idInst;

public:
    CHSZ()
        : m_hsz(NULL)
        , m_idInst(0)
    {
    }

    CHSZ(DWORD idInst, LPCTSTR pszText)
        : m_hsz(NULL)
        , m_idInst(idInst)
    {
        m_hsz = DdeCreateStringHandle(m_idInst, pszText, CSTRING_CODE_PAGE);
    }

    CHSZ &Set(DWORD idInst, LPCTSTR pszText)
    {
        ATLASSERT(!m_idInst);
        ATLASSERT(!m_hsz);

        m_hsz = DdeCreateStringHandle(m_idInst = idInst, pszText, CSTRING_CODE_PAGE);

        return *this;
    }

    ~CHSZ()
    {
        if (m_hsz) DdeFreeStringHandle(m_idInst, m_hsz);
    }

    operator HSZ () const
    {
        return m_hsz;
    }

    BOOL operator ! () const
    {
        return !m_hsz;
    }

private:
    CHSZ(const CHSZ &);
    void operator = (const CHSZ &);
};

}; //namespace Win32
}; //namespace API
