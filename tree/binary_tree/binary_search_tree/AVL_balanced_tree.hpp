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
                namespace AVLTree
                {
                    template <typename OtherAugment = DefaultNodeAugmentation>
                    struct SubtreeHeightAugment
                    {
                        struct Data : OtherAugment::Data
                        {
                            int height = 0;
                        };
                        static void update(Data *node_data, const Data *left_data, const Data *right_data)
                        {
                            set_height(node_data, std::max(get_height(left_data), get_height(right_data)) + 1);
                            if constexpr (requires { OtherAugment::update(node_data, left_data, right_data); })
                            {
                                OtherAugment::update(node_data, left_data, right_data);
                            }
                        }
                        static int get_height(const Data *node_data) { return node_data ? node_data->height : 0; }
                        static int set_height(Data *node_data, int h) { return node_data ? node_data->height = h : 0; }
                        static constexpr bool require_path_update = true;
                    };
                    template <typename T, typename AugmentPolicy = DefaultNodeAugmentation>
                    struct AVLTreeNodeLinked : BinarySearchTreeNodeLinked<T, AVLTreeNodeLinked<T, AugmentPolicy>, SubtreeHeightAugment<AugmentPolicy>>
                    {
                        using node_base=BinarySearchTreeNodeLinked<T, AVLTreeNodeLinked<T, AugmentPolicy>, SubtreeHeightAugment<AugmentPolicy>>;
                        using BinarySearchTreeNodeLinked<T, AVLTreeNodeLinked<T, AugmentPolicy>, SubtreeHeightAugment<AugmentPolicy>>::BinarySearchTreeNodeLinked;
                    };
                    template<typename T,typename AugmentPolicy>
                    void swap_node(AVLTreeNodeLinked<T,AugmentPolicy>&x,AVLTreeNodeLinked<T,AugmentPolicy>&y){
                        using node_base=AVLTreeNodeLinked<T,AugmentPolicy>::node_base;
                        swap_node(static_cast<node_base&>(x),static_cast<node_base&>(y));
                    }
                    template <typename T, typename KeyT = T, typename Compare = std::less<KeyT>, typename KeyOfValue = IdentityKeyOfValue<T>, typename OtherAugment = DefaultNodeAugmentation>
                    struct AVLTreeLinked : BinarySearchTreeLinked<T, KeyT, Compare, KeyOfValue, SubtreeHeightAugment<OtherAugment>, AVLTreeNodeLinked<T, OtherAugment>>
                    {
                        // static_assert(
                        //     std::is_base_of<RBTreeNodeLinked<T>, NodeType>::value,
                        //     "RBTree only accepts NodeType inheriting RBTreeNodeLinked");
                        using value_type = T;
                        using key_type = KeyT;
                        using AugmentPolicy = SubtreeHeightAugment<OtherAugment>;
                        using NodeType = AVLTreeNodeLinked<T, OtherAugment>;
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
                            if (!check_avl())
                                return false;
                            return Base::checker_unique();
                        }
                        bool checker_multi()const override
                        {
                            if (!check_avl())
                                return false;
                            return Base::checker_multi();
                        }

                    protected:
                        static int get_height(Node *node) { return AugmentPolicy::get_height(node); }
                        static Node *set_height(Node *node, int h) { return AugmentPolicy::set_height(node, h), node; }
                        static int get_balance_factor(Node *node) { return node ? (get_height(node->right()) - get_height(node->right())) : 0; }

                        void height_relance(Node *parent)
                        {
                            // balance_factor = right_height - left_heght
                            if (!parent)
                                return;
                            bool to_right;
                            while (parent != this->end_ptr())
                            {
                                int pbf = get_balance_factor(parent);
                                if (std::abs(pbf) <= 1)
                                    break;
                                Node *node = (pbf < 0 ? parent->left() : parent->right()); // the higher child
                                int nbf = get_balance_factor(node);                        // can only be -1 or 0 or 1
                                if (nbf * pbf < 0)
                                {
                                    // here, balance_factor of node can only be -1 or 1
                                    // in this case, the higher child of node and the higher child (aka node) of parent are in different direction
                                    this->rotate(node, to_right = (nbf < 0));
                                    node = node->parent;
                                }
                                // now the higher child of (new)node and the higher child (aka (new)node) of parent are in the same direction
                                this->rotate(parent, to_right = (pbf < 0));
                                node = parent->parent;
                                parent = node->parent;
                            }
                        }

                        void insert_hook(Node *p) override { return height_relance(p->parent); }
                        void erase_hook(Node *p) override { return height_relance(p->parent); }
                        Node *createNodeInternal(const T &v) override
                        {
                            return set_height(Base::createNodeInternal(v), 0);
                        }
                        bool check_avl()const
                        {
                            std::function<bool(Node *)> func;
                            func = [&](Node *p) -> bool
                            {
                                if (!p)
                                    return true;
                                if (std::abs(get_balance_factor(p)) > 1)
                                    return false;
                                if (!func(p->left()))
                                    return false;
                                if (!func(p->right()))
                                    return false;
                                return true;
                            };
                            return func(this->root());
                        }
                    };

                }
            }
        }
    }
}