/*
 * $Revision: 355 $ $Date: 2010-07-14 16:47:38 -0700 (Wed, 14 Jul 2010) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see 
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#ifndef _VESTA_OBJECT_H_
#define _VESTA_OBJECT_H_

#include <cassert>

namespace vesta
{

class Object
{
public:
    Object() :
        m_refCount(0)
    {
    }

    virtual ~Object()
    {
    }

public:
    int addRef()
    {
        return ++m_refCount;
    }

    int release()
    {
        assert(m_refCount > 0);

        // TODO: Need atomic decrement/test to ensure thread safety
        --m_refCount;
        if (m_refCount == 0)
        {
            delete this;
        }

        return m_refCount;
    }

    int refCount() const
    {
        return m_refCount;
    }

private:
    mutable int m_refCount;
};

template <class T> class counted_ptr
{
public:
    typedef T element_type;

    explicit counted_ptr(T* p = 0) :
        m_p(p)
    {
        if (m_p)
            m_p->addRef();
    }

    ~counted_ptr()
    {
        if (m_p)
            m_p->release();
    }

    counted_ptr(const counted_ptr& cp) :
        m_p(cp.m_p)
    {
        if (m_p)
            m_p->addRef();
    }

    counted_ptr& operator=(T* p)
    {
        if (m_p != p)
        {
            if (m_p)
                m_p->release();
            m_p = p;
            if (m_p)
                m_p->addRef();
        }

        return *this;
    }

    counted_ptr& operator=(const counted_ptr& cp)
    {
        if (this != &cp)
        {
            if (m_p)
                m_p->release();
            m_p = cp.m_p;
            if (m_p)
                m_p->addRef();
        }

        return *this;
    }

    T& operator*() const
    {
        return *m_p;
    }

    T* operator->() const
    {
        return m_p;
    }

    bool isNull() const
    {
        return !m_p;
    }

    bool isValid() const
    {
        return m_p;
    }

    T* ptr() const
    {
        return m_p;
    }

private:
    T* m_p;
};

}

#endif // _VESTA_OBJECT_H_
