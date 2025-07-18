#pragma once
#include <algorithm>
#include <string>
#include <sstream>
#include <exception>
#include "weighted_graph.hpp"
#include "../tree/disjoint_set_union.hpp"
namespace DSA
{
    namespace Graph
    {
        namespace SpanningTree
        {

            using GraphWeighted::WGraph;
            using GraphWeighted::WeightedEdge;
            using GraphWeighted::WeightedAdjoint;
            using Tree::DisjointSetUnion::DSU;

            namespace MinimumSpanningTree
            {
                template<typename T=int>
                bool Kruskal(const WGraph<T> &g, int &sum, std::vector<T> &chosen)
                {
                    chosen.clear();
                    DSU dsu(g.number_of_node);
                    std::vector<int> sortedE(g.E.size());
                    for (int i = 0; i < sortedE.size(); i++)
                        sortedE[i] = i;
                    std::sort(sortedE.begin(), sortedE.end(), [&](int x, int y)
                              { return g.E[x].w < g.E[y].w; });
                    sum = 0;
                    for (auto id : sortedE)
                    {
                        auto e = g.E[id];
                        int pu = dsu.Find(e.u), pv = dsu.Find(e.v);
                        if (pu == pv)
                            continue;
                        sum += e.w;
                        dsu.Union(pu, pv);
                        chosen.push_back(id);
                        if (chosen.size() == g.number_of_node - 1)
                            break;
                    }
                    return chosen.size() == g.number_of_node - 1;
                }
                template<typename T=int>
                bool Prim(const WGraph<T> &g, int &sum, std::vector<int> &chosen)
                {
                    chosen.clear();
                    if (std::any_of(g.E.begin(), g.E.end(), [](WeightedEdge<T> e)
                                    { return e.w < 0; }))
                        return false;
                    std::vector<T> dis(g.number_of_node + 1, -1);
                    struct lazy_node
                    {
                        int node_id;T lazy_dis;
                        bool operator<(const lazy_node &U) const { return lazy_dis > U.lazy_dis; }
                    };
                    std::priority_queue<lazy_node> heap;
                    std::vector<int> last_updated_edge(g.number_of_node + 1, -1);
                    dis[1] = 0;
                    heap.push({1, 0});
                    while (!heap.empty())
                    {
                        int u = heap.top().node_id;
                        heap.pop();
                        dis[u] = 0;
                        for (auto e : g.adj[u])
                        {
                            int v = e.adjvex;
                            if (dis[v] == -1 || e.weight < dis[v])
                            {
                                heap.push({v, (dis[v] = e.weight)});
                                last_updated_edge[v] = e.edge_index;
                            }
                        }
                        while (!heap.empty() && dis[heap.top().node_id] != heap.top().lazy_dis)
                            heap.pop();
                    }
                    sum = 0;
                    for (auto c : last_updated_edge)
                        if (c != -1)
                        {
                            chosen.push_back(c);
                            sum += g.E[c].w;
                        }
                    return chosen.size() == g.number_of_node - 1;
                }
                
            }
        }
    }
}