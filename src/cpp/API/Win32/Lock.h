// Lock.h: Lock class.
//////////////////////////////////////////////////////////////////////

#pragma once

#include "lockable.h"

namespace API {
namespace Win32 {

//////////////////////////////////////////////////////////////////////
// Lock
//////////////////////////////////////////////////////////////////////

class Lock
{
private:
    mutable Lockable *sync;

public:
    Lock() : sync(NULL)
    {
    }

    Lock(Lockable *pSync)
        : sync(pSync)
    {
        sync->Lock();
    }

    Lock(Lockable &rSync)
        : sync(&rSync)
    {
        sync->Lock();
    }

    // Give up a lock
    Lock(const Lock &lock)
        : sync(lock.sync)
    {
        lock.sync = NULL;
    }

    ~Lock(void)
    {
        if (sync) sync->Unlock();
    }
};

typedef Lock CLock;

#define LOCK(locable) CLock lock##locable(locable);

//////////////////////////////////////////////////////////////////////
// SafeRefference
//////////////////////////////////////////////////////////////////////

template<class T>
class SafeReference
{
private:
    mutable Lock lock;
    mutable T*     p;
public:
    SafeReference() : p(NULL)
    {
    }
    SafeReference(const Lock &l, T &ref)
        : lock(l), p(&ref)
    {
    }
    SafeReference(const Lock &l, T *pRef)
        : lock(l), p(pRef)
    {
    }
    SafeReference(const SafeReference &safe_ref) : lock(safe_ref.lock), p(safe_ref.p)
    {
        safe_ref.p = NULL;
    }
public:
    T *operator -> () const
    {
        return p;
    }
    bool operator ! ()
    {
        return p == NULL;
    }
    operator bool ()
    {
        return p != NULL;
    }
};

//////////////////////////////////////////////////////////////////////


}; //namespace Win32
}; //namespace API
