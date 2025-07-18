#pragma once
#include "binary_search_tree.hpp"
namespace DSA
{
    namespace Tree
    {
        namespace BinaryTree
        {
            namespace BinarySearchTree
            {
                namespace RBTree
                {
                    enum RBTreeColor
                    {
                        RED = false,
                        BLACK = true
                    };
                    template <typename T, typename AugmentPolicy = DefaultNodeAugmentation>
                    struct RBTreeNodeLinked : BinarySearchTreeNodeLinked<T, RBTreeNodeLinked<T, AugmentPolicy>, AugmentPolicy>
                    {
                        using augment_policy = AugmentPolicy;
                        using node_base=BinarySearchTreeNodeLinked<T, RBTreeNodeLinked<T, AugmentPolicy>, AugmentPolicy>;
                        RBTreeColor color = BLACK;
                        using BinarySearchTreeNodeLinked<T, RBTreeNodeLinked<T, AugmentPolicy>, AugmentPolicy>::BinarySearchTreeNodeLinked;
                    };
                    template<typename T,typename AugmentPolicy>
                    void swap_node(RBTreeNodeLinked<T,AugmentPolicy>&x,RBTreeNodeLinked<T,AugmentPolicy>&y){
                        using node_base=RBTreeNodeLinked<T,AugmentPolicy>::node_base;
                        swap_node(static_cast<node_base&>(x),static_cast<node_base&>(y));
                        std::swap(x.color,y.color);
                    }
                    template <typename T, typename KeyT = T, typename Compare = std::less<KeyT>, typename KeyOfValue = IdentityKeyOfValue<T>, typename AugmentPolicy = DefaultNodeAugmentation>
                    struct RBTreeLinked : BinarySearchTreeLinked<T, KeyT, Compare, KeyOfValue, AugmentPolicy, RBTreeNodeLinked<T, AugmentPolicy>>
                    {
                        // static_assert(
                        //     std::is_base_of<RBTreeNodeLinked<T>, NodeType>::value,
                        //     "RBTree only accepts NodeType inheriting RBTreeNodeLinked");
                        using value_type = T;
                        using key_type = KeyT;
                        using NodeType = RBTreeNodeLinked<T, AugmentPolicy>;
                        using Base = BinarySearchTreeLinked<T, KeyT, Compare, KeyOfValue, AugmentPolicy, NodeType>;
                        using iterator = Base::iterator;
                        using const_iterator = Base::const_iterator;
                        using pointer = Base::pointer;
                        using const_pointer = Base::const_pointer;
                        using reference = Base::reference;
                        using const_reference = Base::const_reference;
                        using difference_type = Base::difference_type;
                        using size_type = Base::size_type;
                        using Node = typename Base::Node;
                        using BinarySearchTreeLinked<T, KeyT, Compare, KeyOfValue, AugmentPolicy, NodeType>::BinarySearchTreeLinked;
                        bool checker_unique()const override
                        {
                            if (!check_rb())
                                return false;
                            return Base::checker_unique();
                        }
                        bool checker_multi()const override
                        {
                            if (!check_rb())
                                return false;
                            return Base::checker_multi();
                        }

                    protected:
                        static bool is_red(Node *p) { return p ? (p->color == RED) : false; }
                        static bool is_black(Node *p) { return p ? (p->color == BLACK) : true; }
                        static Node *set_red(Node *p) { return (p && (p->color = RED)), p; }
                        static Node *set_black(Node *p) { return (p && (p->color = BLACK)), p; }
                        void insert_rebalance(Node *node)
                        {
                            set_red(node);
                            Node *parent = node->parent;
                            Node *grand = parent->parent;
                            while (is_red(parent)) // root and header (root's parent) and null are always black
                            {
                                // node,parent,grand are within tree, and grand is black
                                bool is_right_n = this->is_right(node);
                                bool is_right_p = this->is_right(parent);
                                Node *psibling = grand->children[!is_right_p];
                                if (is_red(psibling))
                                {
                                    set_black(parent);
                                    set_black(psibling);
                                    if (grand == this->root())
                                        break;
                                    set_red(grand);
                                    node = grand;
                                    parent = node->parent;
                                    grand = parent->parent;
                                }
                                else
                                {
                                    bool to_right;
                                    if (is_right_n != is_right_p)
                                    {
                                        this->rotate(parent, to_right = !is_right_n);
                                        std::swap(node, parent);
                                    }
                                    set_black(parent);
                                    set_red(grand);
                                    this->rotate(grand, !is_right_p);
                                    break;
                                }
                            }
                            set_black(this->root());
                            return;
                        }
                        void insert_hook(Node *p) override { return insert_rebalance(p); }
                        void erase_rebalance(Node *parent, bool at_right)
                        {
                            Node *node = parent->children[at_right];
                            if (is_red(node))
                            {
                                set_black(node);
                                return;
                            }
                            bool to_right;
                            while (node != this->root())
                            {
                                Node *sibling = parent->children[!at_right];
                                if (is_red(sibling))
                                {
                                    this->rotate(parent, to_right = at_right);
                                    set_black(sibling);
                                    set_red(parent);
                                    sibling = parent->children[!at_right]; // not null
                                    //at_right=ar_right
                                }
                                if (is_black(sibling->left()) && is_black(sibling->right()))
                                {
                                    set_red(sibling);
                                    if (is_red(parent))
                                    {
                                        set_black(parent);
                                        break;
                                    }
                                    node = parent;
                                    parent = node->parent;
                                    at_right=this->is_right(node);
                                }
                                else
                                {
                                    if (is_black(sibling->children[!at_right]))
                                    {
                                        this->rotate(sibling, to_right = !at_right);
                                        set_red(sibling);
                                        sibling = sibling->parent;
                                        set_black(sibling);
                                    }
                                    sibling->color = parent->color;
                                    this->rotate(parent, to_right = at_right);
                                    set_black(sibling->left());
                                    set_black(sibling->right());
                                    break;
                                }
                            }
                            set_black(this->root());
                            return;
                        }
                        void erase_hook(Node *p) override
                        {
                            set_black(this->root());
                            if (is_black(p))
                            {
                                bool at_right = (p->right() == p);
                                Node *c = at_right ? p->left() : p->right();
                                if (c != this->root())
                                {
                                    erase_rebalance(p->parent, at_right);
                                }
                            }
                        }
                        Node *createNodeInternal(const T &v) override
                        {
                            return set_black(Base::createNodeInternal(v));
                        }
                        bool check_rb()const
                        {
                            if (!is_black(this->root()))
                                return false;
                            int bcnt = -1;
                            std::function<bool(Node *, int)> func;
                            func = [&](Node *p, int curb) -> bool
                            {
                                if (!p)
                                {
                                    if (bcnt == -1)
                                    {
                                        return (bcnt = curb), true;
                                    }
                                    else
                                    {
                                        if (curb != bcnt)
                                            return false;
                                        else
                                            return true;
                                    }
                                }
                                if (is_red(p))
                                {
                                    if (!is_black(p->left()) || !is_black(p->right()))
                                        return false;
                                }
                                else
                                {
                                    ++curb;
                                }
                                if (!func(p->left(), curb))
                                    return false;
                                if (!func(p->right(), curb))
                                    return false;
                                return true;
                            };
                            return func(this->root(), 0);
                        }
                    };
                }
            }
        }
    }
}