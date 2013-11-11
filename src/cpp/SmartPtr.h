#pragma once

/**
 * Smart pointer with AddRef() and Release() calls.
 */
template <class T>
class SmartPtr
{
public:
    SmartPtr() throw()
        : p(NULL)
    {
    }

    SmartPtr(T* lp) throw()
        : p(lp)
    {
        if (p) p->AddRef();
    }

    SmartPtr(const SmartPtr<T> &ptr) throw()
        : p(ptr.p)
    {
        if (p) p->AddRef();
    }

    ~SmartPtr() throw()
    {
        if (p) p->Release();
    }

    operator T*() const throw()
    {
        return p;
    }

    T* operator->() const throw()
    {
        ATLASSERT(p!=NULL);
        return p;
    }

    bool operator!() const throw()
    {
        return !p;
    }

    T* operator=(T* lp) throw()
    {
        if (p != lp)
        {
            if (lp != NULL) lp->AddRef();
            if (p) p->Release();

            p = lp;
        }

        return p;
    }

    const SmartPtr<T> &operator=(const SmartPtr<T> &ptr) throw()
    {
        operator=((T*)ptr);

        return *this;
    }

    void Release() throw()
    {
        T* pTemp = p;
        if (!pTemp)
            return;

        p = NULL;
        pTemp->Release();
    }

private:
    T* p;
};
