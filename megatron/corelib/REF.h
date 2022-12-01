#ifndef ______REF__H
#define ______REF__H
#include "mutexable.h"

#ifdef _MSC_VER
#include <time.h>
#endif
#include <atomic>
/// base class and template of smart pointer with refcount, which can catch pointer many times.
///
#define PRIVATEMUTEX
template < class T > class REF_getter;
class Refcountable
{
    friend class REF_getter < Refcountable >;
    Refcountable (const Refcountable &);	// Not defined  to prevent usage
    Refcountable & operator= (const Refcountable &);	// Not defined  to prevent usage

public:
#ifdef PRIVATEMUTEX
//    Mutex __privateMuteX;
#endif
    std::atomic<int> __Ref_Count;
    Refcountable (): __Ref_Count(0) { }

    int get_ref_count () const
    {
        return __Ref_Count;
    };
    virtual ~ Refcountable ()
    {
        if(__Ref_Count!=0)
        {
            logErr2("destructor with __Ref_Count!=0");
        }
    }
};

///      Smart pointer template
template < class T > class REF_getter
{

public:
    T * ___ptr;
    void increfcount () const
    {
        if (___ptr)
        {
#ifdef PRIVATEMUTEX
//            M_LOCK (___ptr->__privateMuteX);
#endif
//            std::atomic_fe_ ___ptr->__Ref_Count.++;
            std::atomic_fetch_add(&___ptr->__Ref_Count,1);


        }
    }
    void decrefcount ()
    {
        bool need_destroy = false;
        {
            if (___ptr)
            {
#ifdef PRIVATEMUTEX
//                M_LOCK (___ptr->__privateMuteX);
#endif
//                int refc=std::atomic_load(___ptr->__Ref_Count);
                auto prev=std::atomic_fetch_sub(&___ptr->__Ref_Count,1);
                if (prev==1)
                    need_destroy = true;
//                if (___ptr->__Ref_Count > 0)
//                    ___ptr->__Ref_Count--;
            }
            if (need_destroy)
            {
                delete ___ptr;
                ___ptr = NULL;
            }
        }
    }

    bool operator < (const REF_getter & l) const
    {
        return ___ptr < l.___ptr;
    }
    bool valid() const
    {
        return ___ptr!=NULL;
    }
    REF_getter (const T * p):___ptr ((T *) p)
    {
        increfcount ();
    }
    REF_getter (const REF_getter < T > &bcg):___ptr (bcg.___ptr)
    {
        increfcount ();
    }

    ~REF_getter ()
    {
        decrefcount ();
    }
    bool operator== (const REF_getter & bcg) const
    {
        return ___ptr == bcg.___ptr;
    }
#ifndef _MSC_VER
    REF_getter & operator = (const REF_getter & bcg) const
    {
        if (this != &bcg)
        {
            bcg.increfcount ();
            decrefcount ();
            ___ptr = bcg.___ptr;
        }
        return *this;
    }
#endif
    REF_getter & operator = (const REF_getter & bcg)
    {
        if (this != &bcg)
        {
            bcg.increfcount ();
            decrefcount ();
            ___ptr = bcg.___ptr;
        }
        return *this;
    }
    inline  T* operator -> () const
    {
        return ___ptr;
    };
};


#endif
