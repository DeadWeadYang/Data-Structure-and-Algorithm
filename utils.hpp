#pragma once
#include <memory>
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
        struct ICloneable
        {
            virtual ~ICloneable() = default;
            std::unique_ptr<ICloneable> clone() const { return std::unique_ptr<ICloneable>(clone_unsafe()); }

        protected:
            [[nodiscard]] virtual ICloneable *clone_unsafe() const = 0;
        };
    }
}