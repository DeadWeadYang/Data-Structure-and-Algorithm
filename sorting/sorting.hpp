#pragma once
#include <algorithm>
#include <random>
#include <type_traits> // for std::decay
#include <utility>     // for std::declval
#include <vector>
#include <exception>
#include <iostream>
#include <sstream>
namespace DSA
{
    namespace Sorting
    {
        // always use [first,last)

        template <typename It>
        using iterator_value_type_t = typename std::iterator_traits<It>::value_type;
        template <typename RandIt>
        void SelectionSort(RandIt first, RandIt last)
        {
            int n = std::distance(first, last);
            if (n <= 1)
                return;
            auto arr = first;
            for (int i = 0; i < n - 1; i++)
            {
                int ith = i;
                for (int j = i + 1; j < n; j++)
                {
                    if (arr[j] < arr[ith])
                        ith = j;
                }
                std::swap(arr[ith], arr[i]);
            }
        }

        template <typename RandIt>
        void BobbleSort(RandIt first, RandIt last)
        {
            int n = std::distance(first, last);
            if (n <= 1)
                return;
            auto arr = first;
            bool flag = true;
            while (flag)
            {
                flag = false;
                for (int i = 0; i < n - 1; i++)
                {
                    if (arr[i + 1] < arr[i])
                    {
                        flag = true;
                        std::swap(arr[i], arr[i + 1]);
                    }
                }
            }
        }

        template <typename RandIt>
        void InsertionSort(RandIt first, RandIt last)
        {
            int n = std::distance(first, last);
            if (n <= 1)
                return;
            auto arr = first;
            for (int i = 1, j; i < n; i++)
            {
                auto tmp = arr[i];
                for (j = i - 1; j >= 0 && tmp < arr[j]; j--)
                    arr[j + 1] = arr[j];
                arr[j + 1] = tmp;
            }
        }
        struct AlwaysFirstSampler
        {
            template <typename RandIt>
            RandIt operator()(RandIt first, RandIt last)
            {
                return first;
            }
        };
        struct RandomSampler
        {
            std::mt19937 rng{std::random_device{}()};
            template <typename RandIt>
            RandIt operator()(RandIt first, RandIt last)
            {
                auto n = std::distance(first, last);
                std::uniform_int_distribution<decltype(n)> dist(0, n - 1);
                return (first + dist(rng));
            }
        };
        struct MedianOfTreeSampler
        {
            template <typename RandIt>
            RandIt operator()(RandIt first, RandIt last)
            {
                auto n = std::distance(first, last);
                auto mid = n / 2;
                auto x = (first), y = (first + mid), z = (last - 1);
                if (*y < *x)
                    std::swap(x, y);
                if (*z < *x)
                    std::swap(x, z);
                if (*z < *y)
                    std::swap(y, z);
                return y;
            }
        };
        template <typename RandIt, typename Sampler>
        RandIt Partition(RandIt first, RandIt last, Sampler &sampler)
        {
            int low = 0, high = std::distance(first, last) - 1;
            if (high <= 0)
                return first;
            auto arr = first;
            auto pivot = *sampler(first, last);
            while (true)
            {
                while (arr[low] < pivot)
                    ++low;
                while (pivot < arr[high])
                    --high;
                if (low < high)
                    std::swap(arr[low++], arr[high--]);
                else
                    break;
            }
            return arr + std::max(1, low);
        }
        template <typename RandIt, typename Sampler>
        void QuickSort(RandIt first, RandIt last, Sampler &sampler)
        {
            // std::sort(first,last);
            int n = std::distance(first, last);
            if (n <= 1)
                return;
            auto p = Partition<RandIt, Sampler>(first, last, sampler);
            auto pos = p - first;
            QuickSort(first, p, sampler);
            QuickSort(p, last, sampler);
        }
        template <typename RandIt>
        void QuickSort(RandIt first, RandIt last)
        {
            MedianOfTreeSampler sampler{};
            return QuickSort<RandIt, MedianOfTreeSampler>(first, last, sampler);
        }
        template <typename RandIt>
        void HeapSort(RandIt first, RandIt last)
        {
            // std::make_heap(first,last); std::sort_heap(first,last);
            // std::make_heap(first,last); while(first!=last) std::pop_heap(first,last--);
            int n = std::distance(first, last);
            if (n <= 1)
                return;
            auto arr = first;
            auto shift_down = [&](int root_pos, const int end_pos)
            {
                int child_pos = root_pos * 2 + 2;
                while (child_pos < end_pos)
                {
                    if ((arr[child_pos] < arr[child_pos - 1]))
                        --child_pos;
                    if ((arr[child_pos] < arr[root_pos]))
                        return;
                    std::swap(arr[child_pos], arr[root_pos]);
                    root_pos = child_pos;
                    child_pos = root_pos * 2 + 2;
                }
                --child_pos;
                if (child_pos < end_pos && (arr[root_pos] < arr[child_pos]))
                    std::swap(arr[child_pos], arr[root_pos]);
                return;
            }; //[root_pos,end_pos)
            // making heap
            for (int i = n / 2 - 1; i >= 0; i--)
                shift_down(i, n);
            // popping to sort
            for (int i = n - 1; i > 0; i--)
            {
                std::swap(arr[0], arr[i]);
                shift_down(0, i);
            }
        }
        template <typename RandIt>
        void MergeSort(RandIt first, RandIt last)
        {
            int n = std::distance(first, last);
            if (n <= 1)
                return;
            int mid = n / 2;
            MergeSort(first, first + mid);
            MergeSort(first + mid, last);
            auto arr = first;
            // merging
            {
                std::vector<iterator_value_type_t<RandIt>> buf;
                buf.reserve(n);
                int l = 0, r = mid;
                while (l < mid && r < n)
                {
                    if (arr[r] < arr[l])
                    {
                        buf.emplace_back(arr[r++]);
                    }
                    else
                    {
                        buf.emplace_back(arr[l++]);
                    }
                }
                std::copy(arr + l, arr + mid, std::back_inserter(buf));
                std::copy(arr + r, arr + n, std::back_inserter(buf));
                std::copy(buf.begin(), buf.end(), arr);
            }
        }
        template <typename T>
        struct SignedIntRadixAdaper
        {
            static_assert(std::is_signed_v<T> && std::is_integral_v<T>,
                          "SignedIntRadixAdaper is designed for signed integer");
            static constexpr size_t max_key_size = 256;
            static constexpr std::make_unsigned_t<T> mask = 0xff;
            template <typename RandIt>
            static size_t end_key_index(RandIt first, RandIt last)
            {
                using TT = iterator_value_type_t<RandIt>;
                static_assert(std::is_signed_v<TT> && std::is_integral_v<TT>,
                              "SignedIntRadixAdaper is designed for signed integer");
                return sizeof(TT);
            }
            static std::make_unsigned_t<T> convert(T value)
            {
                return value ^ (std::numeric_limits<T>::min());
            }
            size_t operator()(T value, size_t index)
            {
                return (convert(value) >> (8 * index)) & mask;
            }
        };
        template <typename T>
        struct UnsignedIntRadixAdaper
        {
            static_assert(std::is_unsigned_v<T> && std::is_integral_v<T>,
                          "UnignedIntRadixAdaper is designed for unsigned integer");
            static constexpr size_t max_key_size = 256;
            static constexpr T mask = 0xff;
            template <typename RandIt>
            static size_t end_key_index(RandIt first, RandIt last)
            {
                using TT = iterator_value_type_t<RandIt>;
                {
                    static_assert(std::is_unsigned_v<T> && std::is_integral_v<T>,
                                  "UnignedIntRadixAdaper is designed for unsigned integer");
                    return sizeof(TT);
                }
            }
            size_t operator()(T value, size_t index)
            {
                return ((value) >> (8 * index)) & mask;
            }
        };
        template <typename RandIt, typename Adapter>
        void RadixSortLSD(RandIt first, RandIt last, Adapter &adapter)
        {
            int n = std::distance(first, last);
            if (n <= 1)
                return;
            using value_type = iterator_value_type_t<RandIt>;
            auto arr = first;
            std::vector<value_type> buf(n);
            constexpr int max_key = Adapter::max_key_size;
            std::array<size_t, max_key> counts{};
            int key_n = Adapter::end_key_index(first, last);
            for (int key_id = 0; key_id < key_n; key_id++)
            {
                std::fill(counts.begin(), counts.end(), 0);
                for (int i = 0; i < n; i++)
                {
                    counts[adapter(arr[i], key_id)]++;
                }

                for (int i = 1; i < max_key; ++i)
                {
                    counts[i] += counts[i - 1];
                }

                for (int i = n - 1; i >= 0; i--)
                {
                    buf[--counts[adapter(arr[i], key_id)]] = arr[i];
                }

                std::copy(buf.begin(), buf.end(), first);
            }
        }
        template <typename RandIt>
        void IntRadixSort(RandIt first, RandIt last)
        {
            using value_type = iterator_value_type_t<RandIt>;
            static_assert(std::is_integral_v<value_type>, "IntRadixSort is designed for integer");
            if constexpr (std::is_signed_v<value_type>)
            {
                SignedIntRadixAdaper<value_type> adapter{};
                RadixSortLSD(first, last, adapter);
            }
            else
            {
                UnsignedIntRadixAdaper<value_type> adapter{};
                RadixSortLSD(first, last, adapter);
            }
        }

        struct KnuthGapGenerator
        {
            explicit KnuthGapGenerator(size_t n)
            {
                for (cur_h = 1; cur_h < n; cur_h = cur_h * 3 + 1)
                    ;
                cur_h /= 3;
            }
            size_t operator()()
            {
                size_t gap = cur_h;
                cur_h /= 3;
                return gap;
            }

        private:
            size_t cur_h;
        };

        /**
         *  O(N log^2 N) worse case
         */
        struct PrattGapGenerator
        {

            explicit PrattGapGenerator(size_t n)
            {
                if (n < 2)
                    return;
                gaps.push_back(1);
                size_t p2_idx = 0;
                size_t p3_idx = 0;
                while (true)
                {
                    size_t next_val = std::min(gaps[p2_idx] * 2, gaps[p3_idx] * 3);
                    if (next_val >= n)
                        break;
                    if (next_val > gaps.back())
                        gaps.push_back(next_val);
                    if (next_val == gaps[p2_idx] * 2)
                        p2_idx++;
                    if (next_val == gaps[p3_idx] * 3)
                        p3_idx++;
                }
                cur_idx = gaps.size();
            }

            size_t operator()()
            {
                return (cur_idx > 0) ? gaps[--cur_idx] : 0;
            }

        private:
            std::vector<size_t> gaps;
            size_t cur_idx;
        };
        template < typename GapGen,typename RandIt>
        void ShellSort(RandIt first, RandIt last)
        {
            int n = std::distance(first, last);
            if (n <= 1)
                return;
            auto arr = first;
            GapGen gap_generator(n);
            for (int gap = gap_generator(); gap > 0; gap = gap_generator())
            {

                for (int i = gap; i < n; ++i)
                {

                    auto tmp = arr[i];
                    int j = i;
                    for (; j >= gap && tmp < arr[j - gap]; j -= gap)
                        arr[j] = arr[j - gap];
                    arr[j] = tmp;
                }
            }
        }
        template<typename RandIt>
        void ShellSort(RandIt first, RandIt last){
            ShellSort<KnuthGapGenerator>(first,last);
        }
    }
}