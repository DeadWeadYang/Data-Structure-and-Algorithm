#pragma once
#include <algorithm>
#include <string>
#include <exception>
#include <limits>
#include <sstream>
#include "weighted_graph.hpp"

namespace DSA
{
    namespace Graph
    {
        namespace ShortestPath
        {
            using GraphWeighted::WeightedAdjoint;
            using GraphWeighted::WeightedEdge;
            using GraphWeighted::WGraph;
            template <typename T>
            constexpr T Infinity()
            {
                // static_assert to ensure T is an arithmetic type
                static_assert(std::is_arithmetic_v<T>,
                              "Infinity identifier is only for arithmetic types (int, float, etc).");

                if constexpr (std::is_integral_v<T>)
                {
                    // If T is an integer type (int, long, short, etc.)
                    return std::numeric_limits<T>::max();
                }
                else
                { // Assumes floating point, already checked by static_assert
                    // If T is a floating point type (float, double, etc.)
                    return std::numeric_limits<T>::infinity();
                }
            }
            namespace AllPairs
            {
                template <typename T = int>
                bool Floyd(const WGraph<T> &g, std::vector<std::vector<T>> &dis)
                {
                    int n = g.number_of_node;
                    dis = std::vector<std::vector<T>>(n + 1, std::vector<T>(n + 1, Infinity<T>()));
                    for (int i = 1; i <= n; i++)
                        dis[i][i] = 0;
                    for (int i = 1; i <= n; i++)
                        for (auto e : g.adj[i])
                        {
                            int j = e.adjvex;
                            dis[i][j] = std::min(dis[i][j], e.weight);
                        }
                    for (int k = 1; k <= n; k++)
                        for (int i = 1; i <= n; i++)
                            for (int j = 1; j <= n; j++)
                                if (dis[i][k] < Infinity<T>() && dis[k][j] < Infinity<T>())
                                    dis[i][j] = std::min(dis[i][j], dis[i][k] + dis[k][j]);
                    bool reach_negative_ring = false;
                    for (int i = 1; i <= n; i++)
                        reach_negative_ring = reach_negative_ring || dis[i][i] < 0;
                    return reach_negative_ring;
                }
            }
            namespace SingleSource
            {
                template <typename T = int>
                bool BellmanFord(const WGraph<T> &g, int s, std::vector<T> &dis)
                {
                    int n = g.number_of_node;
                    dis = std::vector<T>(n + 1, Infinity<T>());
                    if (s < 1 || s > n)
                        return false;
                    dis[s] = 0;
                    bool relaxed = false;
                    for (int i = 1; i <= n; i++)
                    {
                        relaxed = false;
                        for (auto e : g.E)
                        {
                            if (dis[e.u] < Infinity<T>() && dis[e.u] + e.w < dis[e.v])
                                dis[e.v] = dis[e.u] + e.w, relaxed = true;
                            if (!g.is_directed)
                                if (dis[e.v] < Infinity<T>() && dis[e.v] + e.w < dis[e.u])
                                    dis[e.u] = dis[e.v] + e.w, relaxed = true;
                        }
                        if (!relaxed)
                            break;
                    }
                    bool reach_negative_ring = relaxed;
                    return reach_negative_ring;
                }
                template <typename T = int>
                bool BellmanFord_Queue(const WGraph<T> &g, int s, std::vector<T> &dis)
                {
                    int n = g.number_of_node;
                    dis = std::vector<T>(n + 1, Infinity<T>());
                    if (s < 1 || s > n)
                        return false;
                    dis[s] = 0;
                    std::vector<int> cnt(n + 1);
                    std::queue<int> q;
                    std::vector<int> inq(n + 1); // avoid using std::std::vector<bool> which never store bool
                    q.push(s);
                    inq[s] = true;
                    bool reach_negative_ring = false;
                    while (!q.empty())
                    {
                        int u = q.front();
                        q.pop();
                        inq[u] = false;
                        for (auto e : g.adj[u])
                        {
                            int v = e.adjvex;
                            if (dis[u] + e.weight < dis[v])
                            {
                                dis[v] = dis[u] + e.weight;
                                cnt[v] = cnt[u] + 1;
                                if (!inq[v])
                                {
                                    inq[v] = true;
                                    q.push(v);
                                }
                                if (cnt[v] > n)
                                {
                                    reach_negative_ring = true;
                                    break;
                                }
                            }
                        }
                        if (reach_negative_ring)
                            break;
                    }
                    return reach_negative_ring;
                }
                template <typename T = int>
                bool Dijkstra(const WGraph<T> &g, int s, std::vector<T> &dis)
                {
                    int n = g.number_of_node;
                    dis = std::vector<T>(n + 1, Infinity<T>());
                    if (s < 1 || s > n)
                        return false;
                    dis[s] = 0;
                    bool has_negative_edge = false;
                    for (auto e : g.E)
                        if (e.w < 0)
                            return has_negative_edge = true;
                    struct lazy_node
                    {
                        int node_index;T lazy_dis;
                        bool operator<(const lazy_node &U) const { return lazy_dis > U.lazy_dis; }
                    };
                    std::priority_queue<lazy_node> heap;
                    heap.push({s, dis[s]});
                    while (!heap.empty())
                    {
                        int u = heap.top().node_index;
                        heap.pop();
                        for (auto e : g.adj[u])
                        {
                            int v = e.adjvex;
                            if (dis[u] + e.weight < dis[v])
                            {
                                dis[v] = dis[u] + e.weight;
                                heap.push({v, dis[v]});
                            }
                        }
                        while (!heap.empty() && dis[heap.top().node_index] != heap.top().lazy_dis)
                            heap.pop();
                    }

                    return has_negative_edge;
                }
            }
            
        }
    }
}