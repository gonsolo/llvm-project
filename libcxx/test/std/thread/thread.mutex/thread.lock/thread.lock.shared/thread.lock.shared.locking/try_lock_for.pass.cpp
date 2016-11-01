//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// UNSUPPORTED: libcpp-has-no-threads
// UNSUPPORTED: c++98, c++03, c++11

// <shared_mutex>

// template <class Mutex> class shared_lock;

// template <class Rep, class Period>
//   bool try_lock_for(const chrono::duration<Rep, Period>& rel_time);

#include <shared_mutex>
#include <cassert>

#include "test_macros.h"

bool try_lock_for_called = false;

typedef std::chrono::milliseconds ms;

struct mutex
{
    template <class Rep, class Period>
        bool try_lock_shared_for(const std::chrono::duration<Rep, Period>& rel_time)
    {
        assert(rel_time == ms(5));
        try_lock_for_called = !try_lock_for_called;
        return try_lock_for_called;
    }
    void unlock_shared() {}
};

mutex m;

int main()
{
    std::shared_lock<mutex> lk(m, std::defer_lock);
    assert(lk.try_lock_for(ms(5)) == true);
    assert(try_lock_for_called == true);
    assert(lk.owns_lock() == true);
#ifndef TEST_HAS_NO_EXCEPTIONS
    try
    {
        lk.try_lock_for(ms(5));
        assert(false);
    }
    catch (std::system_error& e)
    {
        assert(e.code().value() == EDEADLK);
    }
#endif
    lk.unlock();
    assert(lk.try_lock_for(ms(5)) == false);
    assert(try_lock_for_called == false);
    assert(lk.owns_lock() == false);
    lk.release();
#ifndef TEST_HAS_NO_EXCEPTIONS
    try
    {
        lk.try_lock_for(ms(5));
        assert(false);
    }
    catch (std::system_error& e)
    {
        assert(e.code().value() == EPERM);
    }
#endif
}
