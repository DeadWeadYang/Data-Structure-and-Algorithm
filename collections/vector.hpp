#pragma once
#include <memory>
#include <exception>
#include <string>
#include <sstream>
namespace DSA
{
    namespace Collections
    {
        namespace ArrayLike
        {
            namespace detail
            {
                template <typename T>
                struct VectorBase;

                template <typename T>
                class ConstArrayLikeIterator;

                template <typename T>
                class ArrayLikeIterator
                {

                public:
                    using value_type = T;
                    using reference = T &;
                    using pointer = T *;
                    using difference_type = std::ptrdiff_t;
                    using iterator_category = std::random_access_iterator_tag;

                    ArrayLikeIterator() : it_ptr_(nullptr) {}
                    explicit ArrayLikeIterator(pointer p) : it_ptr_(p) {}

                    reference operator*() const { return *it_ptr_; }
                    pointer operator->() const { return it_ptr_; }

                    ArrayLikeIterator &operator++()
                    {
                        ++it_ptr_;
                        return *(this);
                    }
                    ArrayLikeIterator operator++(int)
                    {
                        auto tmp = *this;
                        ++it_ptr_;
                        return tmp;
                    }
                    ArrayLikeIterator &operator--()
                    {
                        --it_ptr_;
                        return *this;
                    }
                    ArrayLikeIterator operator--(int)
                    {
                        auto tmp = *this;
                        --it_ptr_;
                        return tmp;
                    }
                    ArrayLikeIterator &operator+=(difference_type n)
                    {
                        it_ptr_ += n;
                        return *this;
                    }
                    friend ArrayLikeIterator operator+(ArrayLikeIterator it, difference_type n)
                    {
                        it += n;
                        return it;
                    }
                    ArrayLikeIterator &operator-=(difference_type n)
                    {
                        it_ptr_ -= n;
                        return *this;
                    }
                    friend ArrayLikeIterator operator-(ArrayLikeIterator it, difference_type n)
                    {
                        it -= n;
                        return it;
                    }
                    friend difference_type operator-(const ArrayLikeIterator &lhs, const ArrayLikeIterator &rhs) { return lhs.it_ptr_ - rhs.it_ptr_; }
                    bool operator<(const ArrayLikeIterator &other) const { return it_ptr_ < other.it_ptr_; }
                    bool operator<=(const ArrayLikeIterator &other) const { return it_ptr_ <= other.it_ptr_; }
                    bool operator>(const ArrayLikeIterator &other) const { return it_ptr_ > other.it_ptr_; }
                    bool operator>=(const ArrayLikeIterator &other) const { return it_ptr_ >= other.it_ptr_; }
                    bool operator==(const ArrayLikeIterator &other) const { return it_ptr_ == other.it_ptr_; }
                    bool operator!=(const ArrayLikeIterator &other) const { return it_ptr_ != other.it_ptr_; }

                    // Allow conversion to const_iterator
                    friend class ConstArrayLikeIterator<T>;

                protected:
                    pointer it_ptr_;
                };

                template <typename T>
                class ConstArrayLikeIterator
                {
                public:
                    using value_type = T;
                    using reference = const T &;
                    using pointer = const T *;
                    using difference_type = std::ptrdiff_t;
                    using iterator_category = std::random_access_iterator_tag;

                    ConstArrayLikeIterator() : it_ptr_(nullptr) {}
                    explicit ConstArrayLikeIterator(pointer p) : it_ptr_(p) {}
                    ConstArrayLikeIterator(const ArrayLikeIterator<T> &other) : it_ptr_(other.it_ptr_) {}

                    reference operator*() const { return *it_ptr_; }
                    pointer operator->() const { return it_ptr_; }

                    ConstArrayLikeIterator &operator++()
                    {
                        ++it_ptr_;
                        ;
                        return *this;
                    }
                    ConstArrayLikeIterator operator++(int)
                    {
                        auto tmp = *this;
                        ++(*this);
                        return tmp;
                    }
                    ConstArrayLikeIterator &operator--()
                    {
                        --it_ptr_;
                        return *this;
                    }
                    ConstArrayLikeIterator operator--(int)
                    {
                        auto tmp = *this;
                        --(*this);
                        return tmp;
                    }

                    ConstArrayLikeIterator &operator+=(difference_type n)
                    {
                        it_ptr_ += n;
                        return *this;
                    }
                    friend ConstArrayLikeIterator operator+(ConstArrayLikeIterator it, difference_type n)
                    {
                        it += n;
                        return it;
                    }
                    ConstArrayLikeIterator &operator-=(difference_type n)
                    {
                        it_ptr_ -= n;
                        return *this;
                    }
                    friend ConstArrayLikeIterator operator-(ConstArrayLikeIterator it, difference_type n)
                    {
                        it -= n;
                        return it;
                    }
                    friend difference_type operator-(const ConstArrayLikeIterator &lhs, const ConstArrayLikeIterator &rhs) { return lhs.it_ptr_ - rhs.it_ptr_; }
                    bool operator<(const ConstArrayLikeIterator &other) const { return it_ptr_ < other.it_ptr_; }
                    bool operator<=(const ConstArrayLikeIterator &other) const { return it_ptr_ <= other.it_ptr_; }
                    bool operator>(const ConstArrayLikeIterator &other) const { return it_ptr_ > other.it_ptr_; }
                    bool operator>=(const ConstArrayLikeIterator &other) const { return it_ptr_ >= other.it_ptr_; }
                    bool operator==(const ConstArrayLikeIterator &other) const { return it_ptr_ == other.it_ptr_; }
                    bool operator!=(const ConstArrayLikeIterator &other) const { return it_ptr_ != other.it_ptr_; }

                protected:
                    pointer it_ptr_;
                };

                template <typename T>
                struct VectorMemoryBase
                {

                    using value_type = T;
                    using reference = T &;
                    using const_reference = const T &;
                    using pointer = T *;
                    using const_pointer = const T *;
                    using size_type = size_t;
                    using difference_type = std::ptrdiff_t;
                    using allocator_type = std::allocator<T>;

                    VectorMemoryBase() = default;
                    explicit VectorMemoryBase(size_type n)
                    {
                        if (n)
                        {
                            p_start = alloc.allocate(n);
                            p_end_of_storage = p_start + n;
                        }
                    }
                    ~VectorMemoryBase()
                    {
                        if (p_start)
                        {
                            alloc.deallocate(p_start, p_end_of_storage - p_start);
                        }
                    }
                    VectorMemoryBase(const VectorMemoryBase &) = delete;
                    VectorMemoryBase &operator=(const VectorMemoryBase &) = delete;
                    VectorMemoryBase(VectorMemoryBase &&other) noexcept
                        : p_start(other.p_start), p_end_of_storage(other.p_end_of_storage), alloc(std::move(other.alloc))
                    {
                        other.p_start = other.p_end_of_storage = nullptr;
                    }

                protected:
                    T *p_start = nullptr;
                    T *p_end_of_storage = nullptr;
                    allocator_type alloc;
                };
                template <typename T>
                struct VectorGuard;
                template <typename T>
                struct VectorBase : VectorMemoryBase<T>
                {

                    using Base = VectorMemoryBase<T>;
                    using value_type = Base::value_type;
                    using reference = Base::reference;
                    using const_reference = Base::const_reference;
                    using pointer = Base::pointer;
                    using const_pointer = Base::const_pointer;
                    using size_type = Base::size_type;
                    using difference_type = Base::difference_type;
                    using allocator_type = Base::allocator_type;
                    using traits = std::allocator_traits<allocator_type>;
                    friend struct VectorGuard<T>;

                    VectorBase() { this->p_finish = this->p_start; }
                    explicit VectorBase(size_type n) : VectorMemoryBase<T>(n)
                    {
                        this->p_finish = this->p_start;
                    }
                    void clear()
                    {
                        while (this->p_finish != this->p_start)
                            destroy_at_end();
                    }
                    ~VectorBase() { clear(); }
                    void swap(VectorBase &other) noexcept
                    {
                        std::swap(this->p_start, other.p_start);
                        std::swap(this->p_finish, other.p_finish);
                        std::swap(this->p_end_of_storage, other.p_end_of_storage);
                        // Allocator aware swap
                        if constexpr (traits::propagate_on_container_swap::value)
                        {
                            std::swap(this->alloc, other.alloc);
                        }
                    }

                protected:
                    T *p_finish = nullptr;
                    void construct_at_end(const T &v) { construct_at_end_impl(v); }
                    void construct_at_end(T &&v) { construct_at_end_impl(std::move(v)); }
                    void destroy_at_end()
                    {
                        --this->p_finish;
                        traits::destroy(this->alloc, p_finish);
                    }
                    void default_construct_at_end() { construct_at_end_impl(); }
                    void construct_at_end_range(T *first, T *last)
                    {
                        for (; first != last; ++first)
                            construct_at_end_impl(std::move_if_noexcept(*first));
                    }
                    void construct_at_end_multi(size_type n, const T &v)
                    {
                        for (; n > 0; --n)
                            construct_at_end_impl(v);
                    }
                    void default_construct_at_end_multi(size_type n)
                    {
                        for (; n > 0; --n)
                            construct_at_end_impl();
                    }

                private:
                    template <typename... Args>
                    void construct_at_end_impl(Args &&...args)
                    {
                        traits::construct(this->alloc, this->p_finish, std::forward<Args>(args)...);
                        ++this->p_finish;
                    }
                };
                template <typename T>
                struct VectorGuard
                {
                    using pointer = VectorBase<T>::pointer;
                    VectorBase<T> *guarded_instance;
                    pointer guarded_pos = nullptr;
                    bool commited = false;
                    explicit VectorGuard(VectorBase<T> *base_instance)
                    {
                        guarded_instance = base_instance;
                        guarded_pos = base_instance->p_finish;
                    }
                    void commit() { commited = true; }
                    ~VectorGuard()
                    {
                        if (!commited)
                        {
                            while (guarded_instance->p_finish != guarded_pos)
                                guarded_instance->destroy_at_end();
                        }
                    }
                };
            }
            template <typename T>
            struct Vector final
                : detail::VectorBase<T>
            {
                static constexpr double grow_factor = 2.0;
                using Base = detail::VectorBase<T>;
                using allocator_type = Base::allocator_type;
                using value_type = Base::value_type;
                using reference = Base::reference;
                using const_reference = Base::const_reference;
                using pointer = Base::pointer;
                using const_pointer = Base::const_pointer;
                using iterator = detail::ArrayLikeIterator<T>;
                using const_iterator = detail::ConstArrayLikeIterator<T>;
                using size_type = Base::size_type;
                using difference_type = Base::difference_type;
                using reverse_iterator = std::reverse_iterator<iterator>;
                using const_reverse_iterator = std::reverse_iterator<const_iterator>;
                using Guard = detail::VectorGuard<T>;
                Vector() = default;
                explicit Vector(size_type n)
                    : detail::VectorBase<T>(n)
                {
                    this->default_construct_at_end_multi(n);
                }
                explicit Vector(size_type n, const T &v)
                    : detail::VectorBase<T>(n)
                {
                    this->construct_at_end_multi(n, v);
                }
                template <class InputIt>
                explicit Vector(InputIt first, InputIt last)
                {
                    insert(end(), first, last);
                }
                void clear() { Base::clear(); }
                ~Vector() = default;
                size_type size() const { return this->p_finish - this->p_start; }
                bool empty() const { return this->p_finish == this->p_start; }
                size_type capacity() const { return this->p_end_of_storage - this->p_start; }
                pointer data() { return this->p_start; }
                const_pointer data() const { return this->p_start; }
                reference at(size_type index)
                {
                    if (index >= size())
                    {
                        std::ostringstream ss;
                        ss << "Vector::at(index): index (" << index << ") >= this->size() (" << size() << ") ";
                        throw std::out_of_range(ss.str());
                    }
                    return this->p_start[index];
                }
                const_reference at(size_type index) const
                {
                    if (index >= size())
                    {
                        std::ostringstream ss;
                        ss << "Vector::at(index): index (" << index << ") >= this->size() (" << size() << ") ";
                        throw std::out_of_range(ss.str());
                    }
                    return this->p_start[index];
                }
                reference operator[](size_type index) { return this->p_start[index]; }
                const_reference operator[](size_type index) const { return this->p_start[index]; }
                reference front() { return *(this->p_start); }
                const_reference front() const { return *(this->p_start); }
                reference back() { return *(this->p_finish - 1); }
                const_reference back() const { return *(this->p_finish - 1); }
                void assign(size_type n, const T &v)
                {
                    Vector tmp(n, v);
                    this->swap(tmp);
                }
                template <class InputIt>
                void assign(InputIt first, InputIt last)
                {
                    Vector tmp(first, last);
                    this->swap(tmp);
                }

                iterator begin() { return iterator{this->p_start}; }
                const_iterator begin() const { return const_iterator{this->p_start}; }
                iterator end() { return iterator{this->p_finish}; }
                const_iterator end() const { return const_iterator{this->p_finish}; }
                reverse_iterator rbegin() { return reverse_iterator{end()}; }
                const_reverse_iterator rbegin() const { return const_reverse_iterator{end()}; }
                reverse_iterator rend() { return reverse_iterator{begin()}; }
                const_reverse_iterator rend() const { return const_reverse_iterator{begin()}; }
                const_iterator cbegin() const { return begin(); }
                const_iterator cend() const { return end(); };
                const_reverse_iterator crbegin() const { return rbegin(); };
                const_reverse_iterator crend() const { return rend(); };

                void reserve(size_type least_cap)
                {
                    if (least_cap > capacity())
                        reallocate(least_cap);
                }
                void shrink_to_fit()
                {
                    if (size() < capacity())
                        reallocate(size());
                }
                void resize(size_type n)
                {
                    if (capacity() < n)
                        reallocate(calc_new_capacity(n));
                    if (size() < n)
                    {
                        Guard guard_tmp(this);
                        this->default_construct_at_end_multi(n - size());
                        guard_tmp.commit();
                    }
                    while (size() > n)
                        this->destroy_at_end();
                }
                void resize(size_type n, const T &v)
                {
                    if (capacity() < n)
                        reallocate(calc_new_capacity(n));
                    if (size() < n)
                    {
                        Guard guard_tmp(this);
                        this->construct_at_end_multi(n - size(), v);
                        guard_tmp.commit();
                    }
                    while (size() > n)
                        this->destroy_at_end();
                }
                void push_back(const T &v)
                {
                    if (size() == capacity())
                    {
                        size_type new_cap = calc_new_capacity(size() + 1);
                        reallocate(new_cap);
                    }
                    this->construct_at_end(v);
                }
                void pop_back() { this->destroy_at_end(); }
                iterator insert(const_iterator pos, const T &v)
                {
                    size_type pos_index = pos - cbegin();
                    if (size() == capacity())
                    {
                        size_type new_cap = calc_new_capacity(size() + 1);
                        reallocate(new_cap);
                    }
                    this->construct_at_end(v);
                    std::rotate(begin() + pos_index, end() - 1, end());
                    return begin() + pos_index;
                }
                iterator insert(const_iterator pos, size_type n, const T &v)
                {
                    size_type pos_index = pos - cbegin();
                    if (!n)
                        return begin() + pos_index;
                    if (capacity()-size()<n)
                    {
                        size_type new_cap = calc_new_capacity(size() + n);
                        Base tmp(new_cap);
                        tmp.construct_at_end_range(this->p_start, this->p_start + pos_index);
                        tmp.construct_at_end_multi(n, v);
                        tmp.construct_at_end_range(this->p_start + pos_index, this->p_finish);
                        this->Base::swap(tmp);
                    }
                    else
                    {
                        Guard guard_tmp(this);
                        this->construct_at_end_multi(n, v);
                        guard_tmp.commit();
                        std::rotate(begin() + pos_index, end() - n, end());
                    }
                    return begin() + pos_index;
                }
                template <typename InputIt>
                iterator insert(const_iterator pos, InputIt first, InputIt last)
                {
                    using Category = typename std::iterator_traits<InputIt>::iterator_category;
                    size_type pos_index = pos - cbegin();
                    if (first == last)
                        return begin() + pos_index;
                    insert_dispatch(pos_index, first, last, Category());
                    return begin() + pos_index;
                }
                iterator erase(const_iterator pos)
                {
                    iterator nonconst_pos = begin() + (pos - cbegin());
                    std::move(nonconst_pos + 1, end(), nonconst_pos);
                    pop_back();
                    return nonconst_pos;
                }
                iterator erase(const_iterator first, const_iterator last)
                {
                    iterator nonconst_first = begin() + (first - cbegin());
                    iterator nonconst_last = begin() + (last - cbegin());
                    if (first == last)
                        return nonconst_last;
                    size_type pos_index = first - cbegin();
                    iterator new_end = std::move(nonconst_last, end(), nonconst_first);
                    while (end() != new_end)
                        pop_back();
                    return nonconst_first;
                }

                void swap(Vector &other)
                {
                    if (this != std::addressof(other))
                        this->Base::swap(other);
                }

            protected:
                // assuming new_cap>=size()
                void reallocate(size_type new_cap)
                {
                    Base vec_base(new_cap);
                    vec_base.construct_at_end_range(this->p_start, this->p_finish);
                    this->Base::swap(vec_base);
                }
                template <typename ForwardIt>
                void insert_dispatch(size_type pos_index, ForwardIt first, ForwardIt last,
                                     std::forward_iterator_tag)
                {
                    auto n = std::distance(first, last);
                    if (!n)
                        return;

                    if (capacity() - size() >= n)
                    {
                        Guard guard_tmp(this);
                        for (auto it = first; it != last; ++it)
                        {
                            this->construct_at_end(*it);
                        }
                        guard_tmp.commit();
                        std::rotate(begin() + pos_index, end() - n, end());
                    }
                    else
                    {
                        size_type new_cap = calc_new_capacity(size() + n);
                        Base tmp(new_cap);
                        tmp.construct_at_end_range(this->p_start, this->p_start + pos_index);
                        for (auto it = first; it != last; ++it)
                            tmp.construct_at_end(*it);
                        tmp.construct_at_end_range(this->p_start + pos_index, this->p_finish);
                        this->Base::swap(tmp);
                    }
                }
                template <typename InputIt>
                void insert_dispatch(size_type pos_index, InputIt first, InputIt last,
                                     std::input_iterator_tag)
                {
                    Vector tmp;
                    tmp.reserve(size());
                    tmp.construct_at_end_range(this->p_start, this->p_start + pos_index);
                    for (; first != last; ++first)
                        tmp.push_back(*first);
                    for (auto it = begin() + pos_index; it != end(); ++it)
                    {
                        tmp.push_back(std::move_if_noexcept(*it));
                    }
                    this->swap(tmp);
                }
                size_type calc_new_capacity(size_type new_size)
                {
                    size_type new_cap = capacity() * grow_factor;
                    return std::max(new_cap, new_size);
                }
            };

        }
    }

} // namespace DSA
