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
                    // 定义红黑树节点的两种颜色
                    enum RBTreeColor
                    {
                        RED = false,
                        BLACK = true
                    };
                    // 红黑树的节点类型，为基础BST节点添加了`color`属性。
                    template <typename T, typename AugmentPolicy = DefaultNodeAugmentation>
                    struct RBTreeNodeLinked : BinarySearchTreeNodeLinked<T, RBTreeNodeLinked<T, AugmentPolicy>, AugmentPolicy>
                    {
                        using augment_policy = AugmentPolicy;
                        using node_base = BinarySearchTreeNodeLinked<T, RBTreeNodeLinked<T, AugmentPolicy>, AugmentPolicy>;
                        RBTreeColor color = BLACK;
                        using BinarySearchTreeNodeLinked<T, RBTreeNodeLinked<T, AugmentPolicy>, AugmentPolicy>::BinarySearchTreeNodeLinked;
                    };
                    // 重载swap_node，以确保在使用tree_swap时，节点的颜色也被正确交换。
                    // 这对于删除操作中的拓扑交换至关重要。
                    template <typename T, typename AugmentPolicy>
                    void swap_node(RBTreeNodeLinked<T, AugmentPolicy> &x, RBTreeNodeLinked<T, AugmentPolicy> &y)
                    {
                        using node_base = RBTreeNodeLinked<T, AugmentPolicy>::node_base;
                        swap_node(static_cast<node_base &>(x), static_cast<node_base &>(y));
                        std::swap(x.color, y.color);
                    }
                    // 红黑树的主类。
                    template <typename T, typename KeyT = T, typename Compare = std::less<KeyT>, typename KeyOfValue = IdentityKeyOfValue<T>, typename AugmentPolicy = DefaultNodeAugmentation>
                    struct RBTreeLinked : BinarySearchTreeLinked<T, KeyT, Compare, KeyOfValue, AugmentPolicy, RBTreeNodeLinked<T, AugmentPolicy>>
                    {
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
                        bool checker_unique() const override
                        {
                            if (!check_rb())
                                return false;
                            return Base::checker_unique();
                        }
                        bool checker_multi() const override
                        {
                            if (!check_rb())
                                return false;
                            return Base::checker_multi();
                        }
                        explicit RBTreeLinked(const RBTreeLinked &other) = default;

                        RBTreeLinked &operator=(const RBTreeLinked &other) = default;
                        std::unique_ptr<RBTreeLinked> clone() const
                        {
                            return std::unique_ptr<RBTreeLinked>(clone_unsafe());
                        }

                        
                    virtual void swap(RBTreeLinked &other) 
                    {
                        this->Base::swap(other);
                    }
                    void swap(Base&other)override{
                        if(typeid(*this)!=typeid(other))return;
                        this->Base::swap(other);
                    }

                    protected:
                        // 辅助函数，将空指针(NIL节点)视为黑色，这是红黑树算法的基础。
                        static bool is_red(Node *p) { return p ? (p->color == RED) : false; }
                        static bool is_black(Node *p) { return p ? (p->color == BLACK) : true; }
                        static Node *set_red(Node *p) { return (p && (p->color = RED)), p; }
                        static Node *set_black(Node *p) { return (p && (p->color = BLACK)), p; }
                        // 插入操作后的再平衡函数。
                        void insert_rebalance(Node *node)
                        {
                            // 算法开始，新插入的节点总是红色（根除外，最后处理）。
                            set_red(node);
                            Node *parent = node->parent;
                            Node *grand = parent->parent;
                            // 循环条件：只要存在“红-红”冲突 (parent节点是红色)，就需要调整。
                            // 由于根的parent节点(header)是黑色的，此循环对根节点不会执行。
                            while (is_red(parent)) // root and header (root's parent) and null are always black
                            {
                                // node,parent,grand are within tree, and grand is black
                                bool is_right_n = this->is_right(node);
                                bool is_right_p = this->is_right(parent);
                                Node *psibling = grand->children[!is_right_p]; // psibling=parent's sibling
                                // Case 1: parent's sibling是红色。
                                if (is_red(psibling))
                                {
                                    // 重新着色。将parent和psibling染黑，将祖先染红。
                                    set_black(parent);
                                    set_black(psibling);
                                    // 如果祖先是根，它必须保持黑色，循环结束。
                                    if (grand == this->root())
                                        break;
                                    set_red(grand);
                                    // 问题被“推”到了祖先节点，继续向上检查。
                                    node = grand;
                                    parent = node->parent;
                                    grand = parent->parent;
                                }
                                else // Case 2: parent's sibling是黑色。
                                {
                                    bool to_right;

                                    // Subcase 2.1: “三角”情况 (e.g., LR or RL)。
                                    if (is_right_n != is_right_p)
                                    {
                                        // 对parent节点进行一次旋转，将其转换为“直线”情况。
                                        this->rotate(parent, to_right = !is_right_n);
                                        // 交换node和parent指针，因为它们的角色互换了。
                                        std::swap(node, parent);
                                    }
                                    // Subcase 2.2: “直线”情况 (e.g., LL or RR)。
                                    set_black(parent);
                                    set_red(grand);
                                    this->rotate(grand, !is_right_p);
                                    // 调整后，不会再有“红-红”冲突，可以退出循环。
                                    break;
                                }
                            }
                            // 最终强制确保根节点是黑色，满足红黑树属性2。
                            set_black(this->root());
                            return;
                        }
                        void insert_hook(Node *p) override { return insert_rebalance(p); }

                        // 删除操作后的再平衡函数。
                        // 它的核心任务是修复因删除黑色节点而导致的“各路径黑高必须相等”被破坏的问题。
                        // 这个函数通过一系列旋转和重新着色，为黑高不足的路径增加一个黑色节点，或将问题上移。
                        // parent:  当前存在黑高不一致问题的子树的根节点。
                        // at_right: 黑高不足的路径位于`parent`的右子树中吗？
                        void erase_rebalance(Node *parent, bool at_right)
                        {
                            // `node`是黑高不足路径的起始节点。算法通过追踪`node`来标记问题所在的位置。
                            Node *node = parent->children[at_right];
                            // 如果`node`是红色，情况很简单。
                            // 我们只需将`node`染黑，就能完全弥补因删除而失去的那个黑色节点，黑高恢复。
                            if (is_red(node))
                            {
                                set_black(node);
                                return;
                            }
                            bool to_right;
                            // 循环直到`node`不再是黑高问题的标记（即问题被解决或推到根部）。
                            // 当`node`为红色或成为根时，循环终止。
                            while (node != this->root())
                            {
                                Node *sibling = parent->children[!at_right];
                                // Case 1: sibling节点是红色。
                                if (is_red(sibling))
                                {
                                    // 目标：将情况转化为sibling节点是黑色的后续情况。
                                    // 策略：对`parent`进行旋转，并交换`parent`和`sibling`的颜色。
                                    // 这一步之后，`node`仍然存在黑高不足的问题，但它面对的新sibling节点一定是黑色的。
                                    this->rotate(parent, to_right = at_right);
                                    set_black(sibling);
                                    set_red(parent);
                                    // 更新sibling节点
                                    sibling = parent->children[!at_right]; // not null
                                    // at_right=ar_right
                                }
                                // Case 2: sibling是黑色，且其两个孩子都是黑色。
                                if (is_black(sibling->left()) && is_black(sibling->right()))
                                {
                                    // 目标：将黑高问题向上推给`parent`。
                                    // 策略：将`sibling`染红。此时，以`parent`为根的整个子树中，所有路径的黑高都一致了，
                                    // 但相对于整棵树，这个子树的黑高比其他子树少1。
                                    // 因此，我们将`parent`视为新的问题节点，继续循环。
                                    set_red(sibling);
                                    // 如果parent是红色，可以直接把parent染黑，结束循环
                                    if (is_red(parent))
                                    {
                                        set_black(parent);
                                        break;
                                    }
                                    // 否则继续循环
                                    node = parent;
                                    parent = node->parent;
                                    at_right = this->is_right(node);
                                }
                                else // Case 3 & 4: sibling是黑色，且至少有一个红色孩子。
                                {
                                    // 目标：通过旋转，从sibling的子树“借调”一个黑色节点到`node`所在的路径，彻底解决问题。

                                    // Case 3: “三角”情况（sibling的离node近侧孩子是红色）。
                                    if (is_black(sibling->children[!at_right]))
                                    {
                                        // 通过一次旋转和着色，将其转化为Case 4的“直线”情况。
                                        this->rotate(sibling, to_right = !at_right);
                                        set_red(sibling);
                                        sibling = sibling->parent;
                                        set_black(sibling);
                                    }
                                    // Case 4: “直线”情况（sibling的离node远侧孩子是红色）。
                                    // 对`parent`进行最终旋转，并精心设置颜色，使得黑高恢复平衡。
                                    sibling->color = parent->color;
                                    this->rotate(parent, to_right = at_right);
                                    set_black(sibling->left());
                                    set_black(sibling->right());
                                    break;
                                }
                            }
                            //确保根节点是黑色
                            set_black(this->root());
                            return;
                        }
                        // 删除操作的钩子函数。
                        void erase_hook(Node *p) override
                        {
                            // `p`是在拓扑结构上被移除的节点。
                            // 只有当`p`是黑色时，才会导致其所有祖先节点的路径黑高减1，从而破坏红黑树。
                            set_black(this->root());
                            if (is_black(p))
                            {
                                // 基类的`tree_remove`在调用此钩子前，用一个技巧在`p`的指针中保存了上下文：
                                // `p`的一个子指针会指向它自己，另一个子指针会指向它的替代者`c`。
                                // `bool at_right = (p->right() == p);` 能准确判断出被删除的黑色节点`p`是在其父节点的左侧还是右侧。
                                bool at_right = (p->right() == p);
                                Node *c = at_right ? p->left() : p->right();
                                if (c != this->root())
                                {
                                    erase_rebalance(p->parent, at_right);
                                }
                            }
                        }
                        //需要set_black,因为哨兵节点需要被当成黑色的nil
                        Node *createNodeInternal(const T &v) override
                        {
                            return set_black(Base::createNodeInternal(v));
                        }
                        //性质检查，用于debug
                        bool check_rb() const
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
                        RBTreeLinked *clone_unsafe() const override
                        {
                            return new RBTreeLinked(*this);
                        }
                    };
                }
            }
        }
    }
}