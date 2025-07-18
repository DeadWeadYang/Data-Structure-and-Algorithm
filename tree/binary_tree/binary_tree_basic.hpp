#pragma once
#include <type_traits>
#include <functional>
#include <queue>

namespace DSA
{
    namespace Tree
    {
        namespace BinaryTree
        {

            /*
                Node info that depend on tree structure should be added in derived NodeType, rather than fit into the value
            */
            template <typename T, typename NodeType>
            struct BinaryTreeNodeLinked
            {
                using value_type = T;
                using Node = NodeType;
                using CNode = const Node;
                T value;
                Node *parent = nullptr;
                Node *children[2] = {nullptr, nullptr};
                Node *&left() { return children[0]; }
                Node *&right() { return children[1]; }
                Node *left() const { return children[0]; }
                Node *right() const { return children[1]; }
                explicit BinaryTreeNodeLinked(const T &v = T{}) : value(v)
                {
                    static_assert(
                        std::is_base_of<BinaryTreeNodeLinked<T, NodeType>, NodeType>::value,
                        "NodeType must inherit from BinaryTreeNodeLinked<T, NodeType>");
                }
                virtual ~BinaryTreeNodeLinked() = default;
            };
            template <typename T, typename NodeType>
            void swap_node(BinaryTreeNodeLinked<T, NodeType> &x, BinaryTreeNodeLinked<T, NodeType> &y)
            {
                std::swap(x.parent, y.parent);
                std::swap(x.children, y.children);
            }
            template <typename NodeType>
            struct is_binary_tree_node_linked
                : std::is_base_of<BinaryTreeNodeLinked<typename NodeType::value_type, NodeType>, NodeType>
            {
            };
            template <typename T, typename NodeType>
            struct is_binary_tree_node_linked_T
                : std::is_base_of<BinaryTreeNodeLinked<T, NodeType>, NodeType>
            {
            };
            template <typename NodeType>
            NodeType *binary_leftmost(NodeType *node)
            {
                static_assert(is_binary_tree_node_linked<NodeType>::value,
                              "binary_leftmost() only accepts NodeType inheriting BinaryTreeNodeLinked");
                while (node && node->left())
                    node = node->left();
                return node;
            }
            template <typename NodeType>
            NodeType *binary_rightmost(NodeType *node)
            {
                static_assert(is_binary_tree_node_linked<NodeType>::value,
                              "binary_rightmost() only accepts NodeType inheriting BinaryTreeNodeLinked");
                while (node && node->right())
                    node = node->right();
                return node;
            }

            namespace detail
            {
                template <typename NodeType>
                NodeType *successor(NodeType *node)
                {
                    static_assert(is_binary_tree_node_linked<NodeType>::value,
                                  "successor() only accepts NodeType inheriting BinaryTreeNodeLinked");
                    // if (node->right())
                    // {
                    //     node = node->right();
                    //     while (node->left())
                    //         node = node->left();
                    //     return node;
                    // }
                    // NodeType *p = node->parent;
                    // // The header is the "super-root", all paths eventually lead to it.
                    // while (node == p->right())
                    // {
                    //     node = p;
                    //     p = p->parent;
                    // }
                    // if (node->right() == p)
                    //     p = node; // for single node case
                    // above is for header->left()==leftmost,header->right()==rightmost(),header->parent==root(),root()->parent==header
                    // below is for header->left()==root(),header->right()==leftmost(),header->parent==rightmost(),root()->parent==header
                    if (node->right())
                    {
                        node = node->right();
                        while (node->left())
                            node = node->left();
                        return node;
                    }
                    NodeType *p = node->parent;
                    // The header is the "super-root", all paths eventually lead to it.
                    while (node != p->left())
                    {
                        node = p;
                        p = p->parent;
                    }

                    // If we are the max element, p will be the header. That is the correct successor (end()).
                    // If node is the header, this will not execute correctly, but ++end() is UB.
                    return p;
                }

                template <typename NodeType>
                NodeType *predecessor(NodeType *node)
                {
                    static_assert(is_binary_tree_node_linked<NodeType>::value,
                                  "predecessor() only accepts NodeType inheriting BinaryTreeNodeLinked");
                    // If 'node' is the header, its predecessor is the maximum element.
                    if (node->left())
                    {
                        node = node->left();
                        while (node->right())
                            node = node->right();
                        return node;
                    }
                    NodeType *p = node->parent;
                    while (node == p->left())
                    {
                        node = p;
                        p = p->parent;
                    }
                    // above is for header->left()==root(),header->right()==leftmost(),header->parent==rightmost(),root()->parent==header
                    // below is for header->left()==leftmost,header->right()==rightmost(),header->parent==root(),root()->parent==header (has bug)
                    //  if (node->parent->parent == node)
                    //  {
                    //      return node->right(); // Header's right() pointer stores the maximum element.
                    //  }
                    //  if (node->left())
                    //  {
                    //      node = node->left();
                    //      while (node->right())
                    //          node = node->right();
                    //      return node;
                    //  }
                    //  NodeType *p = node->parent;
                    //  while (node == p->left())
                    //  {
                    //      node = p;
                    //      p = p->parent;
                    //  }
                    return p;
                }

                template <typename T, typename NodeType>
                struct BinaryTreeLinked;

                template <typename T, typename NodeType>
                class ConstInOrderIterator;

                template <typename T, typename NodeType>
                class InOrderIterator
                {

                public:
                    using value_type = T;
                    using reference = T &;
                    using pointer = T *;
                    using difference_type = std::ptrdiff_t;
                    using iterator_category = std::bidirectional_iterator_tag;

                    InOrderIterator() : node_ptr_(nullptr) {}
                    explicit InOrderIterator(NodeType *p) : node_ptr_(p) {}

                    reference operator*() const { return node_ptr_->value; }
                    pointer operator->() const { return &(node_ptr_->value); }

                    InOrderIterator &operator++()
                    {
                        node_ptr_ = successor<NodeType>(node_ptr_);
                        return *this;
                    }
                    InOrderIterator operator++(int)
                    {
                        auto tmp = *this;
                        ++(*this);
                        return tmp;
                    }
                    InOrderIterator &operator--()
                    {
                        node_ptr_ = predecessor<NodeType>(node_ptr_);
                        return *this;
                    }
                    InOrderIterator operator--(int)
                    {
                        auto tmp = *this;
                        --(*this);
                        return tmp;
                    }

                    bool operator==(const InOrderIterator &other) const { return node_ptr_ == other.node_ptr_; }
                    bool operator!=(const InOrderIterator &other) const { return !(*this == other); }

                    // Allow conversion to const_iterator
                    friend class ConstInOrderIterator<T, NodeType>;
                    // NodeType *get_node() const { return node_ptr_; } // Needed for const_iterator conversion

                protected:
                    NodeType *node_ptr_;

                    friend struct BinaryTreeLinked<T, NodeType>;
                };

                template <typename T, typename NodeType>
                class ConstInOrderIterator
                {
                public:
                    using value_type = T;
                    using reference = const T &;
                    using pointer = const T *;
                    using difference_type = std::ptrdiff_t;
                    using iterator_category = std::bidirectional_iterator_tag;

                    ConstInOrderIterator() : node_ptr_(nullptr) {}
                    explicit ConstInOrderIterator(const NodeType *p) : node_ptr_(p) {}
                    ConstInOrderIterator(const InOrderIterator<T, NodeType> &other) : node_ptr_(other.node_ptr_) {}

                    reference operator*() const { return node_ptr_->value; }
                    pointer operator->() const { return &(node_ptr_->value); }

                    ConstInOrderIterator &operator++()
                    {
                        node_ptr_ = successor<NodeType>(const_cast<NodeType *>(node_ptr_));
                        return *this;
                    }
                    ConstInOrderIterator operator++(int)
                    {
                        auto tmp = *this;
                        ++(*this);
                        return tmp;
                    }
                    ConstInOrderIterator &operator--()
                    {
                        node_ptr_ = predecessor<NodeType>(const_cast<NodeType *>(node_ptr_));
                        return *this;
                    }
                    ConstInOrderIterator operator--(int)
                    {
                        auto tmp = *this;
                        --(*this);
                        return tmp;
                    }

                    bool operator==(const ConstInOrderIterator &other) const { return node_ptr_ == other.node_ptr_; }
                    bool operator!=(const ConstInOrderIterator &other) const { return !(*this == other); }

                protected:
                    const NodeType *node_ptr_;
                    friend struct BinaryTreeLinked<T, NodeType>;
                };

            }

            template <typename T>
            struct BinaryTreeNodeLinkedBasic : BinaryTreeNodeLinked<T, BinaryTreeNodeLinkedBasic<T>>
            {
                using BinaryTreeNodeLinked<T, BinaryTreeNodeLinkedBasic<T>>::BinaryTreeNodeLinked;
                ~BinaryTreeNodeLinkedBasic() = default;
            };

            // template <
            //     typename NodeType = BinaryTreeNodeLinkedBasic,
            //     typename = typename std::enable_if<
            //         std::is_base_of<
            //             BinaryTreeNodeLinked<NodeType>, NodeType
            //         >::value
            //     >::type
            // >
            template <typename T, typename NodeType = BinaryTreeNodeLinkedBasic<T>>
            struct BinaryTreeLinked
            {

                static_assert(
                    is_binary_tree_node_linked_T<T, NodeType>::value,
                    "BinaryTreeLinked: NodeType must inherit from BinaryTreeNodeLinked<T, NodeType>");

                using Node = NodeType;
                using CNode = const Node;
                using value_type = T;
                using size_type = std::size_t;
                using pointer = value_type *;
                using const_pointer = const value_type *;
                using reference = value_type &;
                using const_reference = const value_type &;
                using difference_type = std::ptrdiff_t;

                using iterator = detail::InOrderIterator<T, NodeType>;
                using const_iterator = detail::ConstInOrderIterator<T, NodeType>;

                BinaryTreeLinked()
                {
                    header = createNodeInternal();
                    root() = nullptr;
                    leftmost() = header;
                    rightmost() = header;
                }
                bool empty() const { return root() == nullptr; }
                virtual void clear()
                {
                    destroyRecursive(root());
                    root() = nullptr;
                }
                virtual ~BinaryTreeLinked()
                {
                    clear();
                    destroyNodeInternal(end_ptr());
                }

                void preOrderTraversal(const std::function<void(const T &)> &f) const
                {
                    // if (!empty())
                    preOrderRecursive(root(), f);
                }
                void inOrderTraversal(const std::function<void(const T &)> &f) const
                {
                    // if (!empty())
                    inOrderRecursive(root(), f);
                }
                void postOrderTraversal(const std::function<void(const T &)> &f) const
                {
                    // if (!empty())
                    postOrderRecursive(root(), f);
                }
                void levelOrderTraversal(const std::function<void(const T &)> &f) const
                {
                    if (empty())
                        return;
                    std::queue<CNode *> q;
                    q.push(root());
                    while (!q.empty())
                    {
                        CNode *p = q.front();
                        q.pop();
                        f(p->value);
                        if (p->left())
                            q.push(p->left());
                        if (p->right())
                            q.push(p->right());
                    }
                }

                iterator begin() { return iterator(leftmost()); }
                const_iterator begin() const { return const_iterator(leftmost()); }
                const_iterator cbegin() const { return begin(); }
                iterator end() { return iterator(this->end_ptr()); }
                const_iterator end() const { return const_iterator(this->end_ptr()); }
                const_iterator cend() const { return end(); }

            protected:
                // Derived *self() { return static_cast<Derived *>(this); }
                // const Derived *self() const { return static_cast<const Derived *>(this); }
                Node *header;
                // Node *&root() { return end_ptr()->parent; }
                // Node *root() const { return end_ptr()->parent; }
                // Node *end_ptr() const { return header; }
                // Node *begin_ptr() const { return leftmost(); }
                Node *&root() { return end_ptr()->left(); }
                Node *root() const { return end_ptr()->left(); }
                Node *end_ptr() const { return header; }
                Node *begin_ptr() const { return leftmost(); }

                Node *release(Node *new_root = nullptr)
                {
                    Node *old_root = root();
                    if (!new_root)
                    {
                        root() = nullptr;
                        leftmost() = rightmost() = end_ptr();
                    }
                    else
                    {
                        root() = new_root;
                        root()->parent = end_ptr();
                        leftmost() = binary_leftmost(root());
                        rightmost() = binary_rightmost(root());
                    }
                    return old_root;
                }
                // Node *&leftmost() { return end_ptr()->left(); }
                // Node *&rightmost() { return end_ptr()->right(); }
                // Node *leftmost() const { return end_ptr()->left(); }
                // Node *rightmost() const { return end_ptr()->right(); }
                Node *&leftmost() { return end_ptr()->right(); }
                Node *&rightmost() { return end_ptr()->parent; }
                Node *leftmost() const { return end_ptr()->right(); }
                Node *rightmost() const { return end_ptr()->parent; }
                void destroyRecursive(Node *p)
                {
                    if (p)
                    {
                        destroyRecursive(p->left());
                        destroyRecursive(p->right());
                        destroyNodeInternal(p);
                    }
                }
                void preOrderRecursive(CNode *p, const std::function<void(const T &)> &f) const
                {
                    if (p)
                    {
                        f(p->value);
                        preOrderRecursive(p->left());
                        preOrderRecursive(p->right());
                    }
                }
                void inOrderRecursive(CNode *p, const std::function<void(const T &)> &f) const
                {
                    if (p)
                    {
                        inOrderRecursive(p->left());
                        f(p->value);
                        inOrderRecursive(p->right());
                    }
                }
                void postOrderRecursive(CNode *p, const std::function<void(const T &)> &f) const
                {
                    if (p)
                    {
                        postOrderRecursive(p->left());
                        postOrderRecursive(p->right());
                        f(p->value);
                    }
                }
                virtual void destroyNodeInternal(Node *p)
                {
                    delete p;
                }
                virtual Node *createNodeInternal(const T &v = T{})
                {
                    return new Node{v};
                }

                static void link_to(const bool link_right, Node *node, Node *p)
                {
                    node && (node->parent = p);
                    p->children[link_right] = node;
                }
                void link_root(Node *p)
                {
                    root() = p;
                    p->parent = end_ptr();
                }
                void solo_root(Node *p)
                {
                    link_root(p);
                    leftmost() = rightmost() = p;
                }
                void remove_root()
                {
                    root() = nullptr;
                    leftmost() = rightmost() = end_ptr();
                }
                bool only_root()
                {
                    return leftmost() == root() && rightmost() == root();
                }

                /*
                    to keep iterator consistent, instead of swapping the node value, we swap the tree info
                */
                void tree_swap(Node *x, Node *y)
                {
                    if (x == y)
                        return;
                    bool xright = is_right(x);
                    bool yright = is_right(y);
                    bool link_right;
                    if (x->parent == y || y->parent == x)
                    {
                        if (x->parent == y)
                        {
                            swap_node(*x, *y);
                            y->parent = x;
                            x->children[xright] = y;
                        }
                        else
                        {
                            swap_node(*x, *y);
                            x->parent = y;
                            y->children[yright] = x;
                        }
                    }
                    else
                    {
                        swap_node(*x, *y);
                    }

                    if (x == root() || y == root())
                    {
                        if (x == root())
                        {
                            link_root(y);
                            link_to(link_right = yright, x, x->parent);
                        }
                        else
                        {
                            link_root(x);
                            link_to(link_right = xright, y, y->parent);
                        }
                    }
                    else
                    {
                        link_to(link_right = yright, x, x->parent);
                        link_to(link_right = xright, y, y->parent);
                    }

                    link_to(link_right = false, x->left(), x);
                    link_to(link_right = true, x->right(), x);
                    link_to(link_right = false, y->left(), y);
                    link_to(link_right = true, y->right(), y);

                    if (x == leftmost() || y == leftmost())
                    {
                        if (x == leftmost())
                        {
                            leftmost() = y;
                        }
                        else
                        {
                            leftmost() = x;
                        }
                    }
                    if (x == rightmost() || y == rightmost())
                    {
                        if (x == rightmost())
                        {
                            rightmost() = y;
                        }
                        else
                        {
                            rightmost() = x;
                        }
                    }
                }
                static Node *prev(Node *p)
                {
                    return detail::predecessor<Node>(p);
                }
                static Node *next(Node *p)
                {
                    return detail::successor<Node>(p);
                }
                static bool is_right(Node *p)
                {
                    return p == p->parent->right();
                }
                static Node *getNode(const_iterator iter)
                {
                    return const_cast<Node *>(iter.node_ptr_);
                }
            };

        }
    }
}