// Event.h: Event class.
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Handle.h"

namespace API {
namespace Win32 {

//////////////////////////////////////////////////////////////////////
// Event
//////////////////////////////////////////////////////////////////////

class Event : public WaitableHandle
{
public:
    Event()
    {
    }

    Event(BOOL bManualReset, BOOL bInitialState = FALSE, LPCTSTR lpName = NULL, LPSECURITY_ATTRIBUTES lpEventAttributes = NULL)
    {
        Create(bManualReset, bInitialState, lpName, lpEventAttributes);
    }

    BOOL Create(BOOL bManualReset = FALSE, BOOL bInitialState = FALSE, LPCTSTR lpName = NULL, LPSECURITY_ATTRIBUTES lpEventAttributes = NULL)
    {
        return Create(lpEventAttributes, bManualReset, bInitialState, lpName);
    }

    BOOL Create(LPCTSTR lpName, BOOL *pIsDuplicate, BOOL bManualReset = FALSE, BOOL bInitialState = FALSE, LPSECURITY_ATTRIBUTES lpEventAttributes = NULL)
    {
        HANDLE hEvent = ::CreateEvent(lpEventAttributes, bManualReset, bInitialState, lpName);
        *pIsDuplicate = ::GetLastError() == ERROR_ALREADY_EXISTS;

        Attach(hEvent);

        return IsValid();
    }

    BOOL Create(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCTSTR lpName)
    {
        HANDLE hEvent = ::CreateEvent(lpEventAttributes, bManualReset, bInitialState, lpName);

        Attach(hEvent);

        return IsValid();
    }

    BOOL Open(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCTSTR lpName)
    {
        HANDLE hEvent = ::OpenEvent(dwDesiredAccess, bInheritHandle, lpName);

        Attach(hEvent);

        return IsValid();
    }

    void Set()
    {
        ATLASSERT(IsValid());

        ::SetEvent(handle);
    }

    void Reset()
    {
        ATLASSERT(IsValid());

        ::ResetEvent(handle);
    }

    void Set(BOOL bSet)
    {
        ATLASSERT(IsValid());

        if (bSet)
            Set();
        else
            Reset();
    }

    BOOL IsSet() const
    {
        return Wait(0) == WAIT_OBJECT_0;
    }

public:
    // static members
    static BOOL IsSet(HANDLE hEvent)
    {
        return ::WaitForSingleObject( hEvent, 0 ) == WAIT_OBJECT_0;
    }
};

//////////////////////////////////////////////////////////////////////


}; //namespace Win32
}; //namespace API
