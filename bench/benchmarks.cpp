#include <benchmark/benchmark.h>
#include <Lz.hpp>
#include <utility>


static void BM_Enumerate(benchmark::State& state) {
    std::array<int, 32> arr{};

    for (auto _ : state) {
        auto enumeration = lz::enumerate(arr);
        benchmark::DoNotOptimize(enumeration);

        for (std::pair<int, int> pair : enumeration) {
            benchmark::DoNotOptimize(pair);
        }
    }
}

static void BM_Filter(benchmark::State& state) {
    std::array<int, 32> arr{};

    for (auto _ : state) {
        auto filter = lz::filter(arr, [](const int i) { return i != 0; });
        benchmark::DoNotOptimize(filter);

        for (int filtered : filter) {
            benchmark::DoNotOptimize(filtered);
        }
    }
}

static void BM_Map(benchmark::State& state) {
    std::array<int, 32> arr{};

    for (auto _ : state) {
        auto map = lz::map(arr, [](const int i) { return i == 0 ? 10 : 5; });
        benchmark::DoNotOptimize(map);

        for (int mapped : map) {
            benchmark::DoNotOptimize(mapped);
        }
    }
}

BENCHMARK(BM_Enumerate);
BENCHMARK(BM_Filter);

BENCHMARK_MAIN();