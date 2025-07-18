#pragma once
namespace DSA
{
    namespace Utils
    {

        template <typename Pair>
        struct Select1stKeyOfValue
        {
            const Pair::first_type &operator()(const Pair &v) const { return v.first; }
        };
        template <typename T>
        struct IdentityKeyOfValue
        {
            const T &operator()(const T &k) const { return k; }
        };
    }
}