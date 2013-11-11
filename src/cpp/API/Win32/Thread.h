// Thread.h: Thread class.
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Handle.h"
#include "Runnable.h"

namespace API {
namespace Win32 {

//////////////////////////////////////////////////////////////////////
// Thread
//////////////////////////////////////////////////////////////////////

class Thread : public WaitableHandle
{
protected:
    DWORD m_dwThreadID;

public:
    Thread():m_dwThreadID(0) {}

    BOOL Create(LPTHREAD_START_ROUTINE lpStartAddress,    // thread function
                LPVOID lpParameter,                       // thread argument
                DWORD dwCreationFlags = 0                 // creation option
            )
    {
        return Create(NULL, 0, lpStartAddress, lpParameter, dwCreationFlags);
    }

    BOOL Create(LPSECURITY_ATTRIBUTES lpThreadAttributes, // SD
                DWORD dwStackSize,                        // initial stack size
                LPTHREAD_START_ROUTINE lpStartAddress,    // thread function
                LPVOID lpParameter,                       // thread argument
                DWORD dwCreationFlags                     // creation option
            )
    {
        HANDLE hThread = ::CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, &m_dwThreadID);

        WaitableHandle::Attach(hThread);

        return IsValid();
    }

    BOOL Create(Runnable *runnable, DWORD dwCreationFlags = 0/*creation option*/)
    {
        return Create(NULL, 0, Runner::Run, runnable, dwCreationFlags);
    }

    BOOL CreateCOM(Runnable *runnable, DWORD dwCreationFlags = 0/*creation option*/)
    {
        return Create(NULL, 0, Runner::RunCOM, runnable, dwCreationFlags);
    }

    void Attach(HANDLE hThread, DWORD dwThreadID)
    {
        WaitableHandle::Attach(hThread);
        m_dwThreadID = dwThreadID;
    }

    DWORD Resume()
    {
        return ::ResumeThread(handle);
    }

    DWORD Suspend()
    {
        return ::SuspendThread(handle);
    }

    DWORD GetExitCode() const
    {
        ATLASSERT(IsValid());

        DWORD dwExitCode = 0;
        if (!::GetExitCodeThread(handle, &dwExitCode))
            return -1;

        return dwExitCode;
    }

    BOOL GetExitCode(LPDWORD lpExitCode) const
    {
        ATLASSERT(IsValid());

        return ::GetExitCodeThread(handle, lpExitCode);
    }

    BOOL IsStillActive() const
    {
        DWORD dwExitCode = 0;
        if (!GetExitCode(&dwExitCode))
            return FALSE;

        return dwExitCode == STILL_ACTIVE;
    }

    BOOL Terminate(DWORD dwExitCode = 0)
    {
        ATLASSERT(IsValid());

        return ::TerminateThread(handle, dwExitCode);
    }

    int GetPriority() const
    {
        ATLASSERT(IsValid());

        return ::GetThreadPriority(handle);
    }

    BOOL SetPriority(int nPriority)
    {
        ATLASSERT(IsValid());

        return ::SetThreadPriority(handle, nPriority);
    }

    DWORD GetID()
    {
        ATLASSERT(IsValid());

        return m_dwThreadID;
    }

    BOOL PostMessage(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0)
    {
        return ::PostThreadMessage(GetID(), Msg, wParam, lParam);
    }

public:
    static DWORD GetCurrentThreadId()
    {
        return ::GetCurrentThreadId();
    }
private:
    class Runner
    {
    public:
        static DWORD WINAPI Run(LPVOID lpRunnable)
        {
            try
            {
                ((Runnable*)lpRunnable)->Run();
            }
            catch (...)
            {
                ATLASSERT(FALSE);
            }
            return 0;
        }
        // run with COM initializing
        static DWORD WINAPI RunCOM(LPVOID lpRunnable)
        {
            CoInitialize(NULL);
            try
            {
                ((Runnable*)lpRunnable)->Run();
            }
            catch (...)
            {
                ATLASSERT(FALSE);
            }
            CoUninitialize();
            return 0;
        }
    };
};

//////////////////////////////////////////////////////////////////////


}; //namespace Win32
}; //namespace API
