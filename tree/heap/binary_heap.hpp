#pragma once
#include <vector>
#include <functional>
#include <algorithm>

namespace DSA
{
    namespace Tree
    {
        namespace Heap
        {
            namespace BinaryHeap
            {

                template <typename T, typename Compare = std::less<T>>
                struct BinaryHeap
                {
                    using value_type = T;
                    size_t size() const { return size_r; }
                    bool empty() const { return !size_r; }
                    const T &top() const { return data[0]; }
                    void clear()
                    {
                        size_r = 0;
                        data.clear();
                    }
                    void push(const T &d)
                    {
                        if (size_r == data.size())
                            data.push_back(d);
                        else
                            data[size_r] = d;
                        adjustUp(size_r++);
                        // equivalent to std::push_heap
                    }
                    void pop()
                    {
                        std::swap(data[0], data[--size_r]);
                        adjustDown(0);
                    }
                    void shrink_to_fit()
                    {
                        data.resize(size_r);
                        data.shrink_to_fit();
                    }
                    explicit BinaryHeap(const Compare &cmp = Compare{}) : comp{cmp} {}

                private:
                    std::vector<T> data;
                    size_t size_r = 0;
                    Compare comp;
                    void adjustUp(int pos)
                    {
                        int parent = (pos - 1) / 2;
                        while (pos > 0 && comp(data[parent], data[pos]))
                        {
                            std::swap(data[parent], data[pos]);
                            pos = parent;
                            parent = (pos - 1) / 2;
                        }
                    }
                    void adjustDown(int pos)
                    {
                        int child = pos * 2 + 2;
                        while (child < size_r)
                        {
                            if (comp(data[child], data[child - 1]))
                                --child;
                            if (comp(data[child], data[pos]))
                                return;
                            std::swap(data[child], data[pos]);
                            pos = child;
                            child = pos * 2 + 2;
                        }
                        --child;
                        if (child < size_r && comp(data[pos], data[child]))
                            std::swap(data[child], data[pos]);
                        return;
                    }
                };
                template <typename T>
                using BinaryHeapMax = BinaryHeap<T>;
                template <typename T>
                using BinaryHeapMin = BinaryHeap<T, std::greater<T>>;
            }
        }
    }
}