#pragma once
#include <algorithm>
#include <exception>
#include <sstream>
#include "graph_basic.hpp"
#include "weighted_graph.hpp"
namespace DSA
{
    namespace Graph
    {
        namespace TopologicalSorting
        {
            using GraphBasic::Graph;
            using GraphBasic::Edge;
            
            using GraphWeighted::WGraph;
            using GraphWeighted::WeightedEdge;
            using GraphWeighted::WeightedAdjoint;
            namespace TopoSort
            {
                bool Kahn(const Graph &g, std::vector<int> &order)
                {
                    order.clear();
                    bool not_DAG = false;
                    if (!g.is_directed)
                        return not_DAG = true;
                    int n = g.number_of_node;
                    std::vector<int> ind(n + 1);
                    for (auto e : g.E)
                        ++ind[e.v];
                    std::queue<int> q;
                    for (int i = 1; i <= n; i++)
                        if (!ind[i])
                            q.push(i);
                    while (!q.empty())
                    {
                        int u = q.front();
                        q.pop();
                        order.push_back(u);
                        for (auto v : g.adj[u])
                        {
                            --ind[v];
                            if (!ind[v])
                                q.push(v);
                        }
                    }
                    not_DAG = (order.size() != n);
                    return not_DAG;
                }
                bool TopoValidate(const Graph &g, const std::vector<int> &order)
                {
                    if (!g.is_directed)
                        return false;
                    std::vector<int> idx(g.number_of_node + 1);
                    int n = 0;
                    for (auto u : order)
                    {
                        if (u > g.number_of_node || u < 1)
                            return false;
                        idx[u] = ++n;
                    }
                    if (n != g.number_of_node)
                        return false;
                    for (auto e : g.E)
                        if (idx[e.u] > idx[e.v])
                            return false;
                    return true;
                }
            }
            namespace AOE
            {
                template<typename T=int>
                bool validateAOE(const WGraph<T> &g, std::vector<int> &topo_order)
                {
                    int n = g.number_of_node, begin_v = 0, end_v = 0;
                    std::vector<int> ind(n + 1), outd(n + 1);
                    for (auto e : g.E)
                    {
                        ++ind[e.v], ++outd[e.u];
                        if (e.w < 0)
                            return false;
                    }

                    for (int i = 1; i <= n; i++)
                    {
                        if (!ind[i])
                        {
                            if (!begin_v)
                                begin_v = i;
                            else
                                return false;
                        }
                        if (!outd[i])
                        {
                            if (!end_v)
                                end_v = i;
                            else
                                return false;
                        }
                    }
                    if (!begin_v || !end_v)
                        return false;
                    return !TopoSort::Kahn(g.ignoreWeight(), topo_order);
                }
                template<typename T=int>
                bool resolve(const WGraph<T> &g, int &critical_path, std::vector<int> &critical_event, std::vector<int> &earliest, std::vector<int> &latest)
                {
                    std::vector<int> order;
                    int n = g.number_of_node;
                    if (!validateAOE(g, order))
                        return false;
                    earliest = std::vector<int>(n + 1);
                    for (auto u : order)
                    {
                        for (auto e : g.adj[u])
                        {
                            int v = e.adjvex;
                            earliest[v] = std::max(earliest[v], earliest[u] + e.weight);
                        }
                    }
                    critical_path = earliest[order[n - 1]];
                    latest = std::vector<int>(n + 1, critical_path);
                    for (int i = n; i >= 1; i--)
                    {
                        int u = order[i - 1];
                        for (auto e : g.adj[u])
                        {
                            int v = e.adjvex;
                            latest[u] = std::min(latest[u], latest[v] - e.weight);
                        }
                    }
                    critical_event.clear();
                    for (int i = 1; i <= n; i++)
                        if (earliest[i] == latest[i])
                            critical_event.push_back(i);
                    return true;
                }
            }
        }
    }
}