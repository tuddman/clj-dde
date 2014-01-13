// CriticalSection.h: CriticalSection class.
//////////////////////////////////////////////////////////////////////

#pragma once

#include <WINDOWS.H>
#include "Lockable.h"

namespace API {
namespace Win32 {


//////////////////////////////////////////////////////////////////////
// CriticalSection
//////////////////////////////////////////////////////////////////////

class CriticalSection : public Lockable
{
private:
    CRITICAL_SECTION cs;

private:
    CriticalSection(const CriticalSection &);
    CriticalSection &operator = (const CriticalSection &);

public:
    CriticalSection(void)
    {
        ::InitializeCriticalSection(&cs);
    }

    virtual ~CriticalSection(void)
    {
        ::DeleteCriticalSection(&cs);
    }

    operator CRITICAL_SECTION ()
    {
        return cs;
    }

public:
    void Enter()
    {
        ::EnterCriticalSection(&cs);
    }

#if(_WIN32_WINNT >= 0x0400)
    BOOL TryEnter()
    {
        return ::TryEnterCriticalSection(&cs);
    }
#endif

    void Leave()
    {
        ::LeaveCriticalSection(&cs);
    }

public:
    // Implements Lockable
    virtual void Lock()
    {
        Enter();
    }

    virtual void Unlock()
    {
        Leave();
    }
};

//////////////////////////////////////////////////////////////////////


}; //namespace Win32
}; //namespace API
