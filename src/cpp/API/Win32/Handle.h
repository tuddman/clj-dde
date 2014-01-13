// Handle.h: Handle class.
//////////////////////////////////////////////////////////////////////

#pragma once

namespace API {
namespace Win32 {

//////////////////////////////////////////////////////////////////////
// THandle
//////////////////////////////////////////////////////////////////////

template<class THANDLE = HANDLE>
class THandle
{
protected:
    THANDLE handle;

public:
    THandle() : handle(NULL) {}

    virtual ~THandle()
    {
    }

public:
    virtual void Close() = 0;

public:
    void Attach(THANDLE h)
    {
        if (handle == h)
            return;

        if (IsValid())
            Close();

        handle = h;
    }

    THANDLE Detach()
    {
        THANDLE h = handle;
        handle = NULL;
        return h;
    }

    THANDLE GetSafeHandle( ) const
    {
        return this == NULL ? NULL : handle;
    }

    virtual BOOL IsValid() const
    {
        return IsValid(handle);
    }

    THANDLE* operator & ()
    {
        return &handle;
    }

    // operators
    operator THANDLE () const { return handle; }
    BOOL operator ! () const { return !IsValid(); }

public:
    static BOOL IsValid(THANDLE handle)
    {
        return handle && handle != INVALID_HANDLE_VALUE;
    }

    // TODO: Implement DuplicateHandle
protected:
    THandle(const THandle &);

    THandle &operator = (const THandle &);
};

//////////////////////////////////////////////////////////////////////
// Handle
//////////////////////////////////////////////////////////////////////

class Handle : public THandle<HANDLE>
{
public:
    Handle()
    {
    }

    virtual ~Handle()
    {
        Close();
    }

    virtual void Close()
    {
        if (IsValid()) ::CloseHandle(handle), handle = NULL;
    }

public:
    // static members
    static void Close(HANDLE handle)
    {
        if (IsValid(handle)) ::CloseHandle(handle);
    }
};

//////////////////////////////////////////////////////////////////////
// WaitableHandle
//////////////////////////////////////////////////////////////////////

class WaitableHandle : public Handle
{
public:
    WaitableHandle() {}

    // Implements Waitable
    DWORD Wait(DWORD dwMilliseconds = INFINITE) const
    {
        ATLASSERT(IsValid());

        return ::WaitForSingleObject( handle, dwMilliseconds );
    }
};

//////////////////////////////////////////////////////////////////////


}; //namespace Win32
}; //namespace API
