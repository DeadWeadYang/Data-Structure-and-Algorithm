#pragma once
#include <algorithm>
#include <vector>
#include <queue>
#include <functional>
#include <map>
#include <tuple>
#include <string>
#include <sstream>
#include <iostream>
#include "binary_tree_basic.hpp"

namespace DSA
{
    namespace Tree
    {
        namespace BinaryTree
        {
            namespace HuffmanTree
            {
                template <typename T>
                struct HuffmanTreeNodeValue
                {
                    static_assert(std::is_arithmetic<T>::value, "HuffmanTree: the type of weight should be arithmetic");
                    int label;
                    T weight;
                    HuffmanTreeNodeValue() = default;
                    HuffmanTreeNodeValue(const std::pair<int, T> &lwp) : label(lwp.first), weight(lwp.second) {};
                    bool operator<(const HuffmanTreeNodeValue &U) const { return weight < U.weight; }
                    bool operator>(const HuffmanTreeNodeValue &U) const { return weight > U.weight; }
                };
                template <typename T>
                struct HuffmanTreeNode : BinaryTreeNodeLinked<HuffmanTreeNodeValue<T>, HuffmanTreeNode<T>>
                {
                    using BinaryTreeNodeLinked<HuffmanTreeNodeValue<T>, HuffmanTreeNode<T>>::BinaryTreeNodeLinked;
                };

                template <typename T>
                struct HuffmanTree : BinaryTreeLinked<HuffmanTreeNodeValue<T>, HuffmanTreeNode<T>>
                {
                    using value_type = HuffmanTreeNodeValue<T>;
                    using Node = HuffmanTreeNode<T>;
                    using BinaryTreeLinked<value_type ,Node>::BinaryTreeLinked;
                    struct NodeCompare
                    {
                        bool operator()(Node *pleft, Node *pright) { return pleft->value > pright->value; }
                    };
                    T getWeightedPathLength() const { return WPL; }
                    const std::map<int, std::string> &getHuffmanCode() const { return HuffmanCode; }
                    void build(std::map<int, T> weights)
                    {
                        if (!this->empty())
                            this->clear();
                        std::priority_queue<Node *, std::vector<Node *>, NodeCompare> heap;
                        for (auto const &kv : weights)
                            heap.push(this->createNodeInternal(kv.first, kv.second));
                        while (heap.size() > 1)
                        {
                            Node *pleft = heap.top();
                            heap.pop();
                            Node *pright = heap.top();
                            heap.pop();
                            Node *pparent = this->createNodeInternal(0, pleft->value.weight + pright->value.weight);
                            pparent->left() = pleft;
                            pparent->right() = pright;
                            pleft->parent = pright->parent = pparent;
                            heap.push(pparent);
                        }
                        if (!heap.empty())
                        {
                            this->release(heap.top());
                            heap.pop();
                        }
                        WPL = this->empty() ? 0 : calculateWPL(this->root(), 0);
                        HuffmanCode.clear();
                        if (!this->empty())
                            calculateHuffmanCode(this->root(), "");
                    }

                protected:
                    Node *createNodeInternal(int label, T weight) { return new Node(value_type({label, weight})); }

                private:
                    // friend struct BinaryTreeLinked<value_type, Node>;s
                    T WPL;
                    std::map<int, std::string> HuffmanCode;
                    T calculateWPL(Node *p, int pathLength)
                    {
                        if (!p)
                            return 0;
                        if (!p->left() && !p->right())
                            return p->value.weight * pathLength;
                        return calculateWPL(p->left(), pathLength + 1) + calculateWPL(p->right(), pathLength + 1);
                    }
                    void calculateHuffmanCode(Node *p, std::string hcode)
                    {
                        if (!p)
                            return;
                        if (!p->left() && !p->right())
                        {
                            HuffmanCode[p->value.label] = hcode.empty() ? "0" : hcode;
                            return;
                        }
                        calculateHuffmanCode(p->left(), hcode + "0");
                        calculateHuffmanCode(p->right(), hcode + "1");
                    }
                };


            }
        }
    }
}