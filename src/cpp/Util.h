#pragma once

/**
 * Loads string.
 */
inline CString GetString(UINT nID)
{
    CString value;
    value.LoadString(nID);
    return value;
}

/**
 * Loads string. If it`s unavailable pszDefault will be returned.
 */
inline CString GetString(UINT nID, LPCTSTR pszDefault)
{
    CString value;
    if (!value.LoadString(nID))
        return pszDefault;
    return value;
}

/**
 * Returns formatted string
 */
inline CString FormatString(UINT nFormatID, ...)
{
    CString strFormat;
    if (!strFormat.LoadString(nFormatID))
    {
        ATLASSERT(FALSE);
        return CString();
    }

    CString strResult;

    va_list argList;
    va_start( argList, nFormatID );
    strResult.FormatV( strFormat, argList );
    va_end( argList );

    return strResult;
}

/**
 * Returns formatted string
 */
inline CString FormatString(LPCTSTR pszFormatID, ...)
{
    CString strResult;

    va_list argList;
    va_start( argList, pszFormatID );
    strResult.FormatV( pszFormatID, argList );
    va_end( argList );

    return strResult;
}
