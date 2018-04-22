//---------------------------------------------------------------------------

#pragma once

#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

//---------------------------------------------------------------------------

#include <core/handle.h>

#include <array>
#include <vector>

//---------------------------------------------------------------------------

namespace std
{
    template<class T, class A = allocator<T>>
    struct array_list : vector<T, A>
    {
    public:
        using vector<T, A>::vector;
    };
}

namespace asd
{
    using std::array_list;

    template<typename T>
    void sort(array_list<T> & v) {
        std::sort(v.begin(), v.end());
    }

    template<typename Pred, typename T, useif<is_callable<Pred, const T &, const T &>::value>>
    void sort(array_list<T> & v) {
        std::sort(v.begin(), v.end(), Pred());
    }

    template<typename T, typename Pred, useif<is_callable<Pred, const T &, const T &>::value>>
    void sort(array_list<T> & v, Pred pred) {
        std::sort(v.begin(), v.end(), pred);
    }

    template<typename T, typename Y>
    array_list<T> & operator << (array_list<T> & a, Y && value) {
        a.push_back(std::forward<Y>(value));
        return a;
    }
}

//---------------------------------------------------------------------------
#endif
