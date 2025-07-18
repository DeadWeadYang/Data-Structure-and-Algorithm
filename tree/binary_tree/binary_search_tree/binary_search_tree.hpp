#pragma once
#include <functional>
#include <algorithm>
#include <type_traits>
#include <concepts>
#include "../binary_tree_basic.hpp"
#include "../../../utils.hpp"
namespace DSA
{
    namespace Tree
    {
        namespace BinaryTree
        {
            namespace BinarySearchTree
            {
                using DSA::Utils::IdentityKeyOfValue;
                struct DefaultNodeAugmentation
                {
                    struct Data
                    {
                    };
                    static constexpr bool require_path_update = false;
                    static constexpr bool subtree_size_implemented = false;
                };
                template <typename T, typename NodeType, typename AugmentPolicy = DefaultNodeAugmentation>
                struct BinarySearchTreeNodeLinked : AugmentPolicy::Data, BinaryTreeNodeLinked<T, NodeType>
                {

                    using augment_policy = AugmentPolicy;
                    using node_base=BinaryTreeNodeLinked<T, NodeType>;
                    using BinaryTreeNodeLinked<T, NodeType>::BinaryTreeNodeLinked;
                    virtual ~BinarySearchTreeNodeLinked()
                    {
                        static_assert(
                            std::is_base_of<BinarySearchTreeNodeLinked<T, NodeType, AugmentPolicy>, NodeType>::value,
                            "NodeType must inherit from BinarySearchTreeNodeLinked<T, NodeType, AugmentPolicy>");
                    }
                };
                template<typename AugmentData>
                void swap_augment_data(AugmentData&x,AugmentData&y){
                    std::swap(x,y);
                }
                template <typename T, typename NodeType, typename AugmentPolicy>
                void swap_node(BinarySearchTreeNodeLinked<T,NodeType,AugmentPolicy>&x,BinarySearchTreeNodeLinked<T,NodeType,AugmentPolicy>&y){
                    using node_base=BinarySearchTreeNodeLinked<T,NodeType,AugmentPolicy>::node_base;
                    swap_node(static_cast<node_base&>(x),static_cast<node_base&>(y));
                    using AugmentData=AugmentPolicy::Data;
                    swap_augment_data(static_cast<AugmentData&>(x),static_cast<AugmentData&>(y));
                }
                template <typename T, typename AugmentPolicy = DefaultNodeAugmentation>
                struct BinarySearchTreeNodeLinkedBasic : BinarySearchTreeNodeLinked<T, BinarySearchTreeNodeLinkedBasic<T, AugmentPolicy>, AugmentPolicy>
                {
                    using BinarySearchTreeNodeLinked<T, BinarySearchTreeNodeLinkedBasic<T, AugmentPolicy>, AugmentPolicy>::BinarySearchTreeNodeLinked;
                };
                template <typename T, typename KeyT = T, typename Compare = std::less<KeyT>, typename KeyOfValue = IdentityKeyOfValue<T>, typename AugmentPolicy = DefaultNodeAugmentation, typename NodeType = BinarySearchTreeNodeLinkedBasic<T, AugmentPolicy>>
                struct BinarySearchTreeLinked : BinaryTreeLinked<T, NodeType>
                {
                    using Base = BinaryTreeLinked<T, NodeType>;
                    using value_type = T;
                    using key_type = KeyT;
                    using size_type = Base::size_type;
                    using extracted_key_t =
                        std::invoke_result_t<KeyOfValue, const value_type &>;

                    static_assert(
                        std::is_same<const key_type &, extracted_key_t>::value,
                        "KeyOfValue::operator()(Value) must return Key");
                    static_assert(
                        std::is_base_of<BinarySearchTreeNodeLinked<T, NodeType, AugmentPolicy>, NodeType>::value,
                        "NodeType must inherit from BinarySearchTreeNodeLinked<T,NodeType,AugmentPolicy>");

                    using iterator = Base::iterator;
                    using const_iterator = Base::const_iterator;
                    using pointer = Base::pointer;
                    using const_pointer = Base::const_pointer;
                    using reference = Base::reference;
                    using const_reference = Base::const_reference;
                    using difference_type = Base::difference_type;
                    using Node = typename Base::Node;
                    using CNode = const Node;
                    using BinaryTreeLinked<T, NodeType>::BinaryTreeLinked;
                    explicit BinarySearchTreeLinked(const Compare &cmp = Compare{}) :BinaryTreeLinked<T, NodeType>(), comp{cmp} {}
                    // template <class InputIterator>
                    // BinarySearchTreeLinked(InputIterator first, InputIterator last,
                    //     const Compare &cmp = Compare{}):comp{cmp}{
                    //         for(;first!=last;)
                    //     };

                    virtual bool checker_unique() const
                    {
                        return check_unique(this->root(), nullptr, nullptr);
                    }
                    virtual bool checker_multi() const
                    {
                        return check_multi(this->root(), nullptr, nullptr);
                    }

                    size_type size() const { return size_r; }
                    iterator find(const key_type &k)
                    {
                        return iterator{find_pointer(k)};
                    }
                    const_iterator find(const key_type &k) const
                    {
                        return const_iterator{find_pointer(k)};
                    }
                    size_type count_unique(const key_type &k) const { return count_unique_pointer(k); }
                    size_type count_multi(const key_type &k) const { return count_multi_pointer(k); }
                    iterator lower_bound(const key_type &k)
                    {
                        return iterator{lower_bound_pointer(k)};
                    }
                    const_iterator lower_bound(const key_type &k) const
                    {
                        return const_iterator{lower_bound_pointer(k)};
                    }
                    iterator upper_bound(const key_type &k)
                    {
                        return iterator{upper_bound_pointer(k)};
                    }
                    const_iterator upper_bound(const key_type &k) const
                    {
                        return const_iterator{upper_bound_pointer(k)};
                    }
                    std::pair<iterator, bool> insert_unique(const T &v)
                    {
                        std::pair<Node *, bool> res = insert_unique_pointer(v);
                        return {iterator{res.first}, res.second};
                    }
                    iterator insert_multi(const T &v)
                    {
                        return iterator{insert_multi_pointer(v)};
                    }
                    iterator erase(const_iterator iter)
                    {
                        if (iter == this->end_ptr())
                            return this->end_ptr();
                        Node *p = this->getNode(iter);
                        return iterator{erase_pointer(p)};
                    }
                    size_type erase_unique(const key_type &k)
                    {
                        return erase_unique_pointer(k);
                    }
                    size_type erase_multi(const key_type &k)
                    {
                        return erase_multi_pointer(k);
                    }
                    std::pair<iterator, iterator> equal_range_unique(const key_type &k)
                    {
                        std::pair<Node *, Node *> res = equal_range_unique_pointer(k);
                        return {iterator{res.first}, iterator{res.second}};
                    }
                    std::pair<const_iterator, const_iterator> equal_range_unique(const key_type &k) const
                    {
                        std::pair<Node *, Node *> res = equal_range_unique_pointer(k);
                        return {const_iterator{res.first}, const_iterator{res.second}};
                    }
                    std::pair<iterator, iterator> equal_range_multi(const key_type &k)
                    {
                        std::pair<Node *, Node *> res = equal_range_multi_pointer(k);
                        return {iterator{res.first}, iterator{res.second}};
                    }
                    std::pair<const_iterator, const_iterator> equal_range_multi(const key_type &k) const
                    {
                        std::pair<Node *, Node *> res = equal_range_multi_pointer(k);
                        return {const_iterator{res.first}, const_iterator{res.second}};
                    }
                    Compare &comparer() { return comp; }
                    const Compare &comparer() const { return comp; }

                protected:
                    Compare comp{};
                    size_type size_r = 0;
                    static const key_type &skey(const Node &node) { return KeyOfValue{}(node.value); }
                    static const key_type &skey(Node *node) { return KeyOfValue{}(node->value); }
                    static const key_type &skey(const T &v) { return KeyOfValue{}(v); }
                    virtual Node *find_pointer(const key_type &k) const
                    {
                        Node *p = this->root();
                        while (p)
                        {
                            const key_type &pk = skey(p);
                            if (this->comp(k, pk))
                                p = p->left();
                            else if (this->comp(pk, k))
                                p = p->right();
                            else
                                return p;
                        }
                        return p ? p : this->end_ptr();
                    }
                    virtual Node *lower_bound_pointer(const key_type &k, Node *node, Node *res) const
                    {
                        while (node)
                        {
                            if (!comp(skey(node), k))
                            {
                                res = node;
                                node = node->left();
                            }
                            else
                                node = node->right();
                        }
                        return res;
                    }
                    Node *lower_bound_pointer(const key_type &k) const
                    {
                        return lower_bound_pointer(k, this->root(), this->end_ptr());
                    }
                    virtual Node *upper_bound_pointer(const key_type &k, Node *node, Node *res) const
                    {
                        while (node)
                        {
                            if (comp(k, skey(node)))
                            {
                                res = node;
                                node = node->left();
                            }
                            else
                                node = node->right();
                        }
                        return res;
                    }
                    Node *upper_bound_pointer(const key_type &k) const
                    {
                        return upper_bound_pointer(k, this->root(), this->end_ptr());
                    }
                    virtual std::pair<Node *, Node *> equal_range_unique_pointer(const key_type &k) const
                    {
                        Node *node = this->root(), *res = this->end_ptr();
                        while (node)
                        {
                            if (comp(k, skey(node)))
                            {
                                res = node;
                                node = node->left();
                            }
                            else if (comp(skey(node), k))
                            {
                                node = node->right();
                            }
                            else
                                return {node, (node->right() ? binary_leftmost(node->right()) : res)};
                        }
                        return {res, res};
                    }
                    virtual std::pair<Node *, Node *> equal_range_multi_pointer(const key_type &k) const
                    {

                        Node *node = this->root(), *res = this->end_ptr();
                        while (node)
                        {
                            if (comp(k, skey(node)))
                            {
                                res = node;
                                node = node->left();
                            }
                            else if (comp(skey(node), k))
                            {
                                node = node->right();
                            }
                            else
                                return {lower_bound_pointer(k, node->left(), node),
                                        upper_bound_pointer(k, node->right(), res)};
                        }
                        return {res, res};
                    }
                    size_type count_unique_pointer(const key_type &k) const
                    {
                        return size_t(find_pointer(k) != this->end_ptr());
                    }
                    size_type count_multi_pointer(const key_type &k) const
                    {
                        if (this->empty())
                            return 0;
                        Node *node = this->root();
                        // if constexpr ((AugmentPolicy::subtree_size_implemented) && (requires { node.size; }))
                        // {
                        //     size_type res=0;
                        //     throw std::logic_error("fast count not implementd yet");
                        // }
                        // else
                        // {
                        auto rg = equal_range_multi_pointer(k);
                        return std::distance(iterator{rg.first}, iterator{rg.second});
                        // }
                    }
                    virtual std::pair<Node *, bool> insert_unique_pointer(const T &v)
                    {
                        if (this->empty())
                        {
                            return {insert_root(v), true};
                        }
                        const key_type &vk = skey(v);
                        Node *node = this->root(), *p = nullptr;
                        bool insert_right = true;
                        while (node)
                        {
                            p = node;
                            const key_type &pk = skey(p);
                            if (insert_right = this->comp(pk, vk))
                                node = p->right();
                            else if (this->comp(vk, pk))
                                node = p->left();
                            else
                                return {p, false};
                        }
                        node = insert_to(insert_right, v, p);
                        may_update_path(node);
                        insert_hook(node);
                        return {node, true};
                    }
                    virtual Node *insert_multi_pointer(const T &v)
                    {
                        if (this->empty())
                        {
                            return insert_root(v);
                        }
                        const key_type &vk = skey(v);
                        Node *node = this->root(), *p = nullptr;
                        bool insert_right = true;
                        while (node)
                        {
                            p = node;
                            const key_type &pk = skey(p);
                            if (insert_right = !this->comp(vk, pk))
                                node = p->right();
                            else
                                node = p->left();
                        }
                        node = insert_to(insert_right, v, p);
                        may_update_path(node);
                        insert_hook(node);
                        return node;
                    }
                    Node *insert_root(const T &v)
                    {
                        size_r = 1;
                        Node *p = this->createNodeInternal(v);
                        this->solo_root(p);
                        return p;
                    }
                    Node *insert_to(const bool insert_right, const T &v, Node *p)
                    {
                        ++size_r;
                        Node *node = this->createNodeInternal(v);
                        this->link_to(insert_right, node, p);
                        if (insert_right)
                        {
                            if (p == this->rightmost())
                                this->rightmost() = node;
                        }
                        else
                        {
                            if (p == this->leftmost())
                                this->leftmost() = node;
                        }
                        return node;
                    }
                    Node *erase_pointer(Node *p)
                    {
                        if (!p || p == this->end_ptr())
                            return this->end_ptr();
                        Node *np = tree_remove(p);
                        may_update_path(p);
                        erase_hook(p);
                        this->destroyNodeInternal(p);
                        return np;
                    }
                    size_type erase_unique_pointer(const key_type &k)
                    {
                        Node *p = find_pointer(k);
                        if (p == this->end_ptr())
                            return 0;
                        erase_pointer(p);
                        return 1;
                    }
                    size_type erase_multi_pointer(const key_type &k)
                    {
                        size_type res = 0;
                        for (auto pr = equal_range_multi_pointer(k);
                             pr.first != pr.second; ++res)
                        {
                            pr.first = erase_pointer(pr.first);
                        }
                        return res;
                    }
                    void may_update_path(Node *p)
                    {

                        if constexpr (AugmentPolicy::require_path_update)
                        {

                            while (p && p != this->end_ptr())
                            {
                                if constexpr (requires { AugmentPolicy::update(p, p->left(), p->right()); })
                                {
                                    AugmentPolicy::update(p, p->left(), p->right());
                                }
                                p = p->parent;
                            }
                        }
                    }
                    Node *tree_remove(Node *p)
                    {
                        if (!p || p == this->end_ptr())
                            return this->end_ptr();
                        --size_r;
                        Node *res = this->next(p);
                        if (this->only_root())
                        {
                            this->remove_root();
                            return res;
                        }
                        if (p == this->leftmost())
                            this->leftmost() = res;
                        if (p == this->rightmost())
                            this->rightmost() = this->prev(p);
                        if (p->left() && p->right())
                        {
                            this->tree_swap(p, res);
                        }
                        // now p only has at most one child
                        Node *t = p->left() ? p->left() : p->right();
                        bool link_right = this->is_right(p);
                        p->children[link_right] = p; // record minimum info for rebalance
                        p->children[!link_right] = t;
                        if (p == this->root())
                        {
                            t ? this->link_root(t) : this->remove_root();
                        }
                        else
                        {
                            this->link_to(link_right, t, p->parent);
                        }
                        return res;
                    }
                    void rotate_left(Node *node)
                    {
                        Node *nparent = node->parent;
                        Node *nright = node->right();
                        bool link_right;
                        this->link_to(link_right = true, nright->left(), node);
                        this->link_to(link_right = false, node, nright);
                        if (node == this->root())
                        {
                            this->link_root(nright);
                        }
                        else
                        {
                            link_right = (node == nparent->right());
                            this->link_to(link_right, nright, nparent);
                        }
                        if constexpr (requires { AugmentPolicy::update(node, node->left(), node->right()); })
                        {
                            AugmentPolicy::update(node, node->left(), node->right());
                            AugmentPolicy::update(nright, nright->left(), nright->right());
                        }
                    }
                    void rotate_right(Node *node)
                    {
                        Node *nparent = node->parent;
                        Node *nleft = node->left();
                        bool link_right;
                        this->link_to(link_right = false, nleft->right(), node);
                        this->link_to(link_right = true, node, nleft);
                        if (node == this->root())
                        {
                            this->link_root(nleft);
                        }
                        else
                        {
                            link_right = (node == nparent->right());
                            this->link_to(link_right, nleft, nparent);
                        }

                        if constexpr (requires { AugmentPolicy::update(node, node->left(), node->right()); })
                        {
                            AugmentPolicy::update(node, node->left(), node->right());
                            AugmentPolicy::update(nleft, nleft->left(), nleft->right());
                        }
                    }
                    void rotate(Node *node, const bool to_right)
                    {
                        return to_right ? rotate_right(node) : rotate_left(node);
                    }
                    virtual void insert_hook(Node *p) {}
                    virtual void erase_hook(Node *p) {}
                    bool check_multi(Node *node, Node *liml, Node *limr) const
                    {
                        if (!node)
                            return true;
                        if (liml && comp(skey(node), skey(liml)))
                            return false;
                        if (limr && comp(skey(limr), skey(node)))
                            return false;
                        if (!check_multi(node->left(), liml, node))
                            return false;
                        if (!check_multi(node->right(), node, limr))
                            return false;
                        return true;
                    }
                    bool check_unique(Node *node, Node *liml, Node *limr) const
                    {
                        if (!node)
                            return true;
                        if (liml && !comp(skey(liml), skey(node)))
                            return false;
                        if (limr && !comp(skey(node), skey(limr)))
                            return false;
                        if (!check_unique(node->left(), liml, node))
                            return false;
                        if (!check_unique(node->right(), node, limr))
                            return false;
                        return true;
                    }
                };
            }
        }
    }
}