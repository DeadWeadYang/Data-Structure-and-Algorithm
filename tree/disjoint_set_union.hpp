#pragma once
#include <vector>

namespace DSA
{
    namespace Tree
    {
        namespace DisjointSetUnion
        {
            struct DSU
            {
                int number_of_node;
                std::vector<int> parent, size, rnk;
                DSU(int n = 0)
                {
                    n = std::max(n, 0);
                    number_of_node = n;
                    parent = std::vector<int>(n + 1);
                    size = std::vector<int>(n + 1, 1);
                    rnk = std::vector<int>(n + 1);
                    for (int i = 1; i <= n; i++)
                        parent[i] = i;
                }
                int FindRecursive(int x)
                {
                    return parent[x] == x ? x : (parent[x] = FindRecursive(parent[x]));
                }
                int FindNonrecursive(int x)
                {
                    int t, p = parent[x];
                    while (parent[p] != p)
                        p = parent[p];
                    while (x != p)
                    {
                        t = parent[x];
                        parent[x] = p;
                        x = t;
                    }
                    return p;
                }
                int Find(int x) { return FindRecursive(x); }
                void UnionRandomly(int x, int y)
                {
                    x = Find(x), y = Find(y);
                    parent[x] = y;
                    size[y] += size[x];
                    rnk[y] = std::max(rnk[y], rnk[x] + 1);
                }
                void UnionByRank(int x, int y)
                {
                    x = Find(x), y = Find(y);
                    if (rnk[x] > rnk[y])
                        std::swap(x, y);
                    parent[x] = y;
                    size[y] += size[x];
                    if (rnk[x] == rnk[y])
                        ++rnk[y];
                }
                void UnionBySize(int x, int y)
                {
                    x = Find(x), y = Find(y);
                    if (size[x] > size[y])
                        std::swap(x, y);
                    parent[x] = y;
                    size[y] += size[x];
                    rnk[y] = std::max(rnk[y], rnk[x] + 1);
                }
                void Union(int x, int y)
                {
                    UnionByRank(x, y);
                }
            };
        }
    }
}