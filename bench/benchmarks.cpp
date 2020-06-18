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

static void BM_Range(benchmark::State& state) {
    for (auto _ : state) {
        auto range = lz::range(32);
        benchmark::DoNotOptimize(range);

        for (int i : range) {
            benchmark::DoNotOptimize(i);
        }
    }
}

static void BM_StringSplitter(benchmark::State& state) {
    std::string toSplit = "Benchmark Benchmark Benchmark Benchmark Benchmark "
                          "Benchmark Benchmark Benchmark Benchmark Benchmark "
                          "Benchmark Benchmark Benchmark Benchmark Benchmark "
                          "Benchmark Benchmark Benchmark Benchmark Benchmark "
                          "Benchmark Benchmark Benchmark Benchmark Benchmark "
                          "Benchmark Benchmark Benchmark Benchmark Benchmark "
                          "Benchmark Benchmark Benchmark Benchmark Benchmark "
                          "Benchmark Benchmark Benchmark Benchmark Benchmark ";

    for (auto _ : state) {
        auto splitter = lz::split(toSplit, " ");
        benchmark::DoNotOptimize(splitter);

//#if __cplusplus < 201703L || (defined(_MSVC_LANG) && _MSVC_LANG < 201703L)
//        using StringType = std::string&;
//#else
//        using StringType = std::string_view;
//#endif

        for (std::string& substring : splitter) {
            benchmark::DoNotOptimize(substring);
        }
    }
}

static void BM_TakeWhile(benchmark::State& state) {
    constexpr size_t size = 32;
    std::array<int, size> array = lz::range(static_cast<int>(size)).toArray<size>();

    for (auto _ : state) {
        auto takeWhile = lz::takewhile(array, [](const int i) { return i != size - 1; });
        benchmark::DoNotOptimize(takeWhile);

        for (int taken : takeWhile) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

static void BM_Take(benchmark::State& state) {
    constexpr size_t size = 32;
    std::array<int, size> array = lz::range(static_cast<int>(size)).toArray<size>();

    for (auto _ : state) {
        auto taken = lz::take(array, 32);
        benchmark::DoNotOptimize(taken);

        for (int i : taken) {
            benchmark::DoNotOptimize(i);
        }
    }
}

static void BM_Slice(benchmark::State& state) {
    constexpr size_t size = 32;
    std::array<int, size> array = lz::range(static_cast<int>(size)).toArray<size>();

    for (auto _ : state) {
        auto sliced = lz::slice(array, 0, 32);
        benchmark::DoNotOptimize(sliced);

        for (int i : sliced) {
            benchmark::DoNotOptimize(i);
        }
    }
}

static void BM_Zip4(benchmark::State& state) {
    std::array<int, 32> arrayA{};
    std::array<int, 32> arrayB{};
    std::array<int, 32> arrayC{};
    std::array<int, 32> arrayD{};

    for (auto _ : state) {
        auto zipper = lz::zip(arrayA, arrayB, arrayC, arrayD);
        benchmark::DoNotOptimize(zipper);

        for (auto tuple : zipper) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

static void BM_Zip3(benchmark::State& state) {
    std::array<int, 32> arrayA{};
    std::array<int, 32> arrayB{};
    std::array<int, 32> arrayC{};

    for (auto _ : state) {
        auto zipper = lz::zip(arrayA, arrayB, arrayC);
        benchmark::DoNotOptimize(zipper);

        for (auto tuple : zipper) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

static void BM_Zip2(benchmark::State& state) {
    std::array<int, 32> arrayA{};
    std::array<int, 32> arrayB{};

    for (auto _ : state) {
        auto zipper = lz::zip(arrayA, arrayB);
        benchmark::DoNotOptimize(zipper);

        for (auto tuple : zipper) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

BENCHMARK(BM_Enumerate);
BENCHMARK(BM_Filter);
BENCHMARK(BM_Map);
BENCHMARK(BM_Range);
BENCHMARK(BM_StringSplitter);
BENCHMARK(BM_TakeWhile);
BENCHMARK(BM_Take);
BENCHMARK(BM_Slice);
BENCHMARK(BM_Zip4);
BENCHMARK(BM_Zip3);
BENCHMARK(BM_Zip2);


BENCHMARK_MAIN();