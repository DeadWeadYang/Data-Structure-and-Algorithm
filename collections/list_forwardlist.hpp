#pragma once
#include <memory>
#include <cstddef>
#include <functional>
#include <limits>
#include <random>
#include <list>
#include <string>
#include <iostream>
#include <exception>
#include <sstream>
namespace DSA
{
    namespace Collections
    {
        namespace ListOrForwardlist
        {
            namespace detail
            {
                template <typename T>
                struct ListBase;

                template <typename T>
                struct ListNode
                {
                    T value;
                    ListNode *n_next = nullptr;
                    ListNode *n_prev = nullptr;
                    ListNode(const T &v = T{}) : value{v} {}
                    void link(ListNode *p)
                    {
                        this->n_next = p;
                        this->n_prev = p->n_prev;
                        p->n_prev->n_next = this;
                        p->n_prev = this;
                    }
                    void link_back(ListNode *p)
                    {
                        this->n_prev = p;
                        this->n_next = p->n_next;
                        p->n_next->n_prev = this;
                        p->n_next = this;
                    }
                    void unlink()
                    {
                        this->n_prev->n_next = this->n_next;
                        this->n_next->n_prev = this->n_prev;
                    }
                };

                template <typename T>
                class ConstListIterator;

                template <typename T>
                class ListIterator
                {

                public:
                    using value_type = T;
                    using reference = T &;
                    using pointer = T *;
                    using difference_type = std::ptrdiff_t;
                    using iterator_category = std::bidirectional_iterator_tag;
                    using Node = ListNode<T>;

                    ListIterator() : node_ptr_(nullptr) {}
                    explicit ListIterator(Node *p) : node_ptr_(p) {}

                    reference operator*() const { return node_ptr_->value; }
                    pointer operator->() const { return &node_ptr_->value; }

                    ListIterator &operator++()
                    {
                        node_ptr_ = node_ptr_->n_next;
                        return *this;
                    }
                    ListIterator operator++(int)
                    {
                        auto tmp = *this;
                        ++(*this);
                        return tmp;
                    }
                    ListIterator &operator--()
                    {
                        node_ptr_ = node_ptr_->n_prev;
                        return *this;
                    }
                    ListIterator operator--(int)
                    {
                        auto tmp = *this;
                        --(*this);
                        return tmp;
                    }

                    bool operator==(const ListIterator &other) const { return node_ptr_ == other.node_ptr_; }
                    bool operator!=(const ListIterator &other) const { return !(*this == other); }

                    // Allow conversion to const_iterator
                    friend class ConstListIterator<T>;
                    // ListNode *get_node() const { return node_ptr_; } // Needed for const_iterator conversion

                protected:
                    Node *node_ptr_;

                    friend struct ListBase<T>;
                };

                template <typename T>
                class ConstListIterator
                {
                public:
                    using value_type = T;
                    using reference = const T &;
                    using pointer = const T *;
                    using difference_type = std::ptrdiff_t;
                    using iterator_category = std::bidirectional_iterator_tag;
                    using Node = ListNode<T>;

                    ConstListIterator() : node_ptr_(nullptr) {}
                    explicit ConstListIterator(const Node *p) : node_ptr_(p) {}
                    ConstListIterator(const ListIterator<T> &other) : node_ptr_(other.node_ptr_) {}

                    reference operator*() const { return node_ptr_->value; }
                    pointer operator->() const { return &node_ptr_->value; }

                    ConstListIterator &operator++()
                    {
                        node_ptr_ = node_ptr_->n_next;
                        return *this;
                    }
                    ConstListIterator operator++(int)
                    {
                        auto tmp = *this;
                        ++(*this);
                        return tmp;
                    }
                    ConstListIterator &operator--()
                    {
                        node_ptr_ = node_ptr_->n_prev;
                        return *this;
                    }
                    ConstListIterator operator--(int)
                    {
                        auto tmp = *this;
                        --(*this);
                        return tmp;
                    }

                    bool operator==(const ConstListIterator &other) const { return node_ptr_ == other.node_ptr_; }
                    bool operator!=(const ConstListIterator &other) const { return !(*this == other); }

                protected:
                    const Node *node_ptr_;
                    friend struct ListBase<T>;
                };
                template <typename T>
                struct ListBase
                {

                    using value_type = T;
                    using reference = T &;
                    using const_reference = const T &;
                    using pointer = T *;
                    using const_pointer = const T *;
                    using iterator = ListIterator<T>;
                    using const_iterator = ConstListIterator<T>;
                    using size_type = size_t;
                    using difference_type = std::ptrdiff_t;
                    using Node = ListNode<T>;
                    ListBase()
                    {
                        header = createNodeInternal();
                        header->n_prev = header->n_next = header;
                    }
                    void clear()
                    {
                        unlink_range(begin_ptr(), end_ptr()->n_prev);
                        destroyNodeInternal(begin_ptr(), end_ptr());
                    }
                    ~ListBase()
                    {
                        clear();
                        destroyNodeInternal(end_ptr());
                    }

                protected:
                    Node *header;
                    Node *&begin_ptr() { return header->n_next; }
                    Node *begin_ptr() const { return header->n_next; }
                    Node *end_ptr() const { return header; }
                    Node *createNodeInternal(const T &v = T{})
                    {
                        return new Node{v};
                    }
                    void destroyNodeInternal(Node *p) { delete p; }
                    void destroyNodeInternal(Node *first, Node *last)
                    {
                        for (Node *p = first; first != last; p = first)
                        {
                            first = first->n_next;
                            destroyNodeInternal(p);
                        }
                    }
                    // note prelast=prev(last),last=next(prelast) ,that is ,[first,prelast]=[first,last)
                    static void link_range(Node *pos, Node *first, Node *prelast)
                    {
                        first->n_prev = pos->n_prev;
                        prelast->n_next = pos;
                        pos->n_prev->n_next = first;
                        pos->n_prev = prelast;
                    }
                    static void link_range_back(Node *pos, Node *first, Node *prelast)
                    {
                        first->n_prev = pos;
                        prelast->n_next = pos->n_next;
                        pos->n_next->n_prev = prelast;
                        pos->n_next = first;
                    }
                    static void unlink_range(Node *first, Node *prelast)
                    {
                        first->n_prev->n_next = prelast->n_next;
                        prelast->n_next->n_prev = first->n_prev;
                    }
                    static Node *getNode(const_iterator iter)
                    {
                        return const_cast<Node *>(iter.node_ptr_);
                    }
                };
            }
            template <typename T>
            struct List final
                : detail::ListBase<T>
            { // types:
                using Base = detail::ListBase<T>;
                using value_type = Base::value_type;
                using reference = Base::reference;
                using const_reference = Base::const_reference;
                using pointer = Base::pointer;
                using const_pointer = Base::const_pointer;
                using iterator = Base::iterator;
                using const_iterator = Base::const_iterator;
                using size_type = Base::size_type;
                using difference_type = Base::difference_type;
                using reverse_iterator = std::reverse_iterator<iterator>;
                using const_reverse_iterator = std::reverse_iterator<const_iterator>;
                using Node = Base::Node;
                List() = default;
                ~List() = default;
                explicit List(size_type n, const T &v = T{})
                    : detail::ListBase<T>()
                {
                    for (; n > 0; --n)
                        push_back(v);
                }
                template <typename InputIt>
                explicit List(InputIt first, InputIt last)
                    : detail::ListBase<T>()
                {
                    for (; first != last; ++first)
                        push_back(*first);
                }
                void assign(size_type n, const T &v)
                {
                    List tmp(n, v);
                }
                template <typename InputIt>
                void assign(InputIt first, InputIt last)
                {
                    List tmp(first, last);
                    this->swap(tmp);
                }

                iterator begin() { return iterator{begin_ptr()}; }
                const_iterator begin() const { return const_iterator{begin_ptr()}; }
                iterator end() { return iterator{end_ptr()}; }
                const_iterator end() const { return const_iterator{end_ptr()}; }
                reverse_iterator rbegin() { return reverse_iterator{end()}; }
                const_reverse_iterator rbegin() const { return const_reverse_iterator{end()}; }
                reverse_iterator rend() { return reverse_iterator{begin()}; }
                const_reverse_iterator rend() const { return const_reverse_iterator{begin()}; }
                const_iterator cbegin() const { return begin(); }
                const_iterator cend() const { return end(); };
                const_reverse_iterator crbegin() const { return rbegin(); };
                const_reverse_iterator crend() const { return rend(); };

                bool empty() const { return begin_ptr() == end_ptr(); }
                void clear()
                {
                    size_r = 0;
                    Base::clear();
                }
                size_type size() const { return size_r; }
                reference front() { return begin_ptr()->value; }
                const_reference front() const { return begin_ptr()->value; }
                reference back() { return end_ptr()->n_prev->value; }
                const_reference back() const { return end_ptr()->n_prev->value; }

                iterator insert(const_iterator pos, const T &v)
                {
                    return iterator{insert_pointer(getNode(pos), v)};
                }
                iterator insert(const_iterator pos, size_type n, const T &v)
                {
                    if (n)
                    {
                        List tmp(n, v);
                        iterator res = tmp.begin();
                        this->splice(pos, tmp);
                        return res;
                    }
                    return pos;
                }
                template <typename InputIt>
                iterator insert(const_iterator pos, InputIt first, InputIt last)
                {
                    if (first != last)
                    {
                        List tmp(first, last);
                        iterator res = tmp.begin();
                        this->splice(pos, tmp);
                        return res;
                    }
                    return pos;
                }
                iterator erase(const_iterator pos) { return iterator{erase_pointer(getNode(pos))}; }
                iterator erase(const_iterator first, const_iterator last)
                {
                    while (first != last)
                        first = erase(first);
                    return iterator{getNode(last)};
                }
                void push_back(const T &v) { insert_pointer(end_ptr(), v); }
                void pop_back() { erase_pointer(end_ptr()->n_prev); }
                void push_front(const T &v) { insert_pointer(begin_ptr(), v); }
                void pop_front() { erase_pointer(end_ptr()->n_next); }
                void splice(const_iterator pos, List &other)
                {
                    splice_pointer(getNode(pos), other);
                }
                void splice(const_iterator pos, List &&other) { splice(pos, other); }
                void splice(const_iterator pos, List &other, const_iterator it)
                {
                    splice_pointer(getNode(pos), other, getNode(it));
                }
                // note: pos shall not be in [first,prelast]
                static void splice_range(const_iterator pos, const_iterator first, const_iterator prelast, size_type &size_change1, size_type &size_change2)
                {
                    Node *n_first = getNode(first);
                    Node *n_prelast = getNode(prelast);
                    size_type change_size = std::distance(first, prelast) + 1;
                    size_change1 += change_size;
                    size_change2 -= change_size;
                    unlink_range(n_first, n_prelast);
                    link_range(getNode(pos), n_first, n_prelast);
                }
                static size_type splice_range(const_iterator pos, const_iterator first, const_iterator prelast)
                {
                    size_type res = 0, tmp;
                    splice_range(pos, first, prelast, res, tmp);
                    return res;
                }
                static void splice_range_back(const_iterator pos, const_iterator first, const_iterator prelast, size_type &size_change1, size_type &size_change2)
                {
                    Node *n_first = getNode(first);
                    Node *n_prelast = getNode(prelast);
                    size_type change_size = std::distance(first, prelast) + 1;
                    size_change1 += change_size;
                    size_change2 -= change_size;
                    unlink_range(n_first, n_prelast);
                    link_range_back(getNode(pos), n_first, n_prelast);
                }
                static size_type splice_range_back(const_iterator pos, const_iterator first, const_iterator prelast)
                {
                    size_type res = 0, tmp;
                    splice_range_back(pos, first, prelast, res, tmp);
                    return res;
                }
                void splice(const_iterator pos, List &other, const_iterator first, const_iterator last)
                {
                    if (first != last)
                    {
                        splice_range(pos, first, std::prev(last), this->size_r, other.size_r);
                    }
                }
                void resize(size_type new_size)
                {
                    while (new_size < size_r)
                        pop_back();
                    if (new_size > size_r)
                        insert(end(), new_size - size_r, T{});
                }
                void swap(List &other)
                {
                    if (this != std::addressof(other))
                    {
                        std::swap(this->header, other.header);
                        std::swap(this->size_r, other.size_r);
                    }
                }

                // try merge [first2,prelast2] into [first1,prelast1]
                // note: this function assum that both ranges are ordered and no overlap
                template <typename Compare>
                static void merge_range(const_iterator first1, const_iterator prelast1, const_iterator first2, const_iterator prelast2, size_type &size_change1, size_type &size_change2, Compare comp)
                {
                    while (true)
                    {
                        if (comp(*first2, *first1))
                        {
                            size_type change_size = 1;
                            const_iterator pos2 = first2;
                            const_iterator peek2 = std::next(pos2);
                            while (pos2 != prelast2 && comp(*peek2, *first1))
                            {
                                pos2 = peek2;
                                ++peek2;
                                ++change_size;
                            };
                            size_change1 += change_size;
                            size_change2 -= change_size;
                            Node *first = getNode(first2);
                            Node *prelast = getNode(pos2);
                            unlink_range(first, prelast);
                            link_range(getNode(first1), first, prelast);
                            if (pos2 == prelast2)
                                return;
                            first2 = peek2;
                        }
                        if (first1 == prelast1)
                        {
                            splice_range_back(prelast1, first2, prelast2, size_change1, size_change2);
                            return;
                        }
                        ++first1;
                    }
                }
                static void merge_range(const_iterator first1, const_iterator prelast1, const_iterator first2, const_iterator prelast2, size_type &size_change1, size_type &size_change2)
                {
                    merge_range(first1, prelast1, first2, prelast2, size_change1, size_change2, std::less<T>{});
                }

                template <typename Compare>
                static size_type merge_range(const_iterator first1, const_iterator prelast1, const_iterator first2, const_iterator prelast2, Compare comp)
                {
                    size_type res = 0, tmp;
                    merge_range(first1, prelast1, first2, prelast2, res, tmp, comp);
                    return res;
                }
                static size_type merge_range(const_iterator first1, const_iterator prelast1, const_iterator first2, const_iterator prelast2)
                {
                    return merge_range(first1, prelast1, first2, prelast2, std::less<T>{});
                }
                template <typename Compare>
                void merge(List &other, Compare comp)
                {
                    if (this != std::addressof(other) && !other.empty())
                    {
                        if (this->empty())
                            this->swap(other);
                        else
                            merge_range(this->begin(), std::prev(this->end()), other.begin(), std::prev(other.end()), this->size_r, other.size_r);
                    }
                }
                template <typename Compare>
                void merge(List &&other, Compare comp) { merge(other, comp); }

                void merge(List &other) { merge(other, std::less<T>{}); }
                void merge(List &&other) { merge(other); }

                size_type remove(const T &v)
                {
                    List to_remove{};
                    for (iterator i = begin(), ed = end(); i != ed;)
                    {
                        if (*i == v)
                        {
                            iterator j = std::next(i);
                            for (; j != ed && *j == v; ++j)
                                ;
                            to_remove.splice(to_remove.end(), *this, i, j);
                            i = (j == ed) ? j : ++j;
                        }
                        else
                            ++i;
                    }
                    return to_remove.size();
                }
                template <typename UnaryPredicate>
                size_type remove_if(UnaryPredicate pred)
                {

                    List to_remove{};
                    for (iterator i = begin(), ed = end(); i != ed;)
                    {
                        if (pred(*i))
                        {
                            iterator j = std::next(i);
                            for (; j != ed && pred(*j); ++j)
                                ;
                            to_remove.splice(to_remove.end(), *this, i, j);
                            i = (j == ed) ? j : ++j;
                        }
                        else
                            ++i;
                    }
                    return to_remove.size();
                }
                void reverse()
                {
                    Node *p = begin_ptr();
                    while (p != end_ptr())
                    {
                        std::swap(p->n_next, p->n_prev);
                        p = p->n_prev;
                    }
                    std::swap(p->n_next, p->n_prev);
                }
                template <typename BinaryPredicate>
                size_type unique(BinaryPredicate pred)
                {

                    List to_remove{};
                    for (iterator i = begin(), ed = end(); i != ed;)
                    {
                        iterator j = std::next(i);
                        for (; j != ed && pred(*i, *j); ++j)
                            ;
                        if ((++i) != j)
                        {
                            to_remove.splice(to_remove.end(), *this, i, j);
                            i = j;
                        }
                    }
                    return to_remove.size();
                }
                size_type unique()
                {
                    return unique(std::equal_to<T>{});
                }
                template <typename Compare>
                void sort(Compare comp)
                {
                    constexpr int size_t_bits = std::numeric_limits<size_type>::digits;
                    std::pair<iterator, iterator> sorted_ranges[size_t_bits], carry, empty_range; // ranges all in form [first,prelast]
                    for (iterator it = this->begin(), ed = this->end(); it != ed;)
                    {
                        carry.first = carry.second = it, ++it;
                        for (int i = 0; i < size_t_bits; ++i)
                        {
                            if (sorted_ranges[i] == empty_range)
                            {
                                std::swap(sorted_ranges[i], carry);
                                break;
                            }
                            iterator first_anchor = std::prev(sorted_ranges[i].first);
                            merge_range(sorted_ranges[i].first, sorted_ranges[i].second, carry.first, carry.second);
                            carry.first = std::next(first_anchor);
                            carry.second = std::prev(it);
                            sorted_ranges[i] = empty_range;
                        }
                    }
                    for (int i = 0; i < size_t_bits; ++i)
                        if (sorted_ranges[i] != empty_range)
                        {
                            if (carry == empty_range)
                            {
                                std::swap(sorted_ranges[i], carry);
                                continue;
                            }

                            iterator first_anchor = std::prev(sorted_ranges[i].first);
                            merge_range(sorted_ranges[i].first, sorted_ranges[i].second, carry.first, carry.second);
                            carry.first = std::next(first_anchor);
                            carry.second = std::prev(this->end());
                            sorted_ranges[i] = empty_range;
                        }
                    return;
                }
                void sort()
                {
                    sort(std::less<T>{});
                }

            protected:
                size_type size_r = 0;
                using Base::begin_ptr;
                using Base::createNodeInternal;
                using Base::destroyNodeInternal;
                using Base::end_ptr;
                using Base::getNode;
                using Base::link_range;
                using Base::link_range_back;
                using Base::unlink_range;

                Node *insert_pointer(Node *pos, const T &v)
                {
                    Node *node = createNodeInternal(v);
                    node->link(pos);
                    ++size_r;
                    return node;
                }
                Node *erase_pointer(Node *pos)
                {
                    Node *res = pos->n_next;
                    pos->unlink();
                    destroyNodeInternal(pos);
                    --size_r;
                    return res;
                }
                void splice_pointer(Node *pos, List &other)
                {
                    if (this != std::addressof(other) && !other.empty())
                    {
                        Node *first = other.begin_ptr();
                        Node *prelast = other.end_ptr()->n_prev;
                        unlink_range(first, prelast);
                        link_range(pos, first, prelast);
                        this->size_r += other.size_r;
                        other.size_r = 0;
                    }
                }
                void splice_pointer(Node *pos, List &other, Node *it)
                {
                    if (pos != it && pos != it->n_prev && it != other.end_ptr())
                    {
                        it->unlink();
                        it->link(pos);
                        --other.size_r;
                        ++this->size_r;
                    }
                }
            };

            
        }
    }
}