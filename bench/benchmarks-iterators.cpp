#include <benchmark/benchmark.h>
#include <utility>
#include <sstream>

#include <Lz/Lz.hpp>


constexpr static size_t SizePolicy = 32;

static void BM_Enumerate(benchmark::State& state) {
    std::array<int, SizePolicy> arr{};

    for (auto _ : state) {
        auto enumeration = lz::enumerate(arr);

        for (std::pair<int, int> pair : enumeration) {
            benchmark::DoNotOptimize(pair);
        }
    }
}

static void BM_Filter(benchmark::State& state) {
    std::array<int, SizePolicy> arr{};

    for (auto _ : state) {
        auto filter = lz::filter(arr, [](const int i) { return i != 0; });

        for (int filtered : filter) {
            benchmark::DoNotOptimize(filtered);
        }
    }
}

static void BM_Map(benchmark::State& state) {
    std::array<int, SizePolicy> arr{};

    for (auto _ : state) {
        auto map = lz::map(arr, [](const int i) { return i == 0 ? 10 : 5; });

        for (int mapped : map) {
            benchmark::DoNotOptimize(mapped);
        }
    }
}

static void BM_Range(benchmark::State& state) {
    for (auto _ : state) {
        auto range = lz::range(SizePolicy);

        for (int i : range) {
            benchmark::DoNotOptimize(i);
        }
    }
}

static void BM_StringSplitter(benchmark::State& state) {
    std::string toSplit = "hello hello hello hello hello he";

    for (auto _ : state) {
        auto splitter = lz::split(toSplit, " ");

#if __cplusplus < 201703L || (defined(_MSVC_LANG) && _MSVC_LANG < 201703L)
        // making non const causes: benchmark/benchmark.h:322:48: internal compiler error: in assign_temp,
        // at function.c:977
        using StringType = const std::string&;
#else
        using StringType = std::string_view;
#endif

        for (StringType substring : splitter) {
            benchmark::DoNotOptimize(substring);
        }
    }
}

static void BM_TakeWhile(benchmark::State& state) {
    std::array<int, SizePolicy> array = lz::range(static_cast<int>(SizePolicy)).toArray<SizePolicy>();

    for (auto _ : state) {
        auto takeWhile = lz::takewhile(array, [](const int i) { return i != SizePolicy - 1; });

        for (int taken : takeWhile) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

static void BM_Take(benchmark::State& state) {
    std::array<int, SizePolicy> array = lz::range(static_cast<int>(SizePolicy)).toArray<SizePolicy>();

    for (auto _ : state) {
        auto taken = lz::take(array, 32);

        for (int i : taken) {
            benchmark::DoNotOptimize(i);
        }
    }
}

static void BM_Slice(benchmark::State& state) {
    std::array<int, SizePolicy> array = lz::range(static_cast<int>(SizePolicy)).toArray<SizePolicy>();

    for (auto _ : state) {
        auto sliced = lz::slice(array, 0, 32);

        for (int i : sliced) {
            benchmark::DoNotOptimize(i);
        }
    }
}

static void BM_Zip4(benchmark::State& state) {
    std::array<int, SizePolicy> arrayA{};
    std::array<int, SizePolicy> arrayB{};
    std::array<int, SizePolicy> arrayC{};
    std::array<int, SizePolicy> arrayD{};

    for (auto _ : state) {
        auto zipper = lz::zip(arrayA, arrayB, arrayC, arrayD);

        for (auto tuple : zipper) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

static void BM_Zip3(benchmark::State& state) {
    std::array<int, SizePolicy> arrayA{};
    std::array<int, SizePolicy> arrayB{};
    std::array<int, SizePolicy> arrayC{};

    for (auto _ : state) {
        auto zipper = lz::zip(arrayA, arrayB, arrayC);

        for (auto tuple : zipper) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

static void BM_Zip2(benchmark::State& state) {
    std::array<int, SizePolicy> arrayA{};
    std::array<int, SizePolicy> arrayB{};

    for (auto _ : state) {
        auto zipper = lz::zip(arrayA, arrayB);

        for (auto tuple : zipper) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

static void BM_Except(benchmark::State& state) {
    constexpr size_t s = SizePolicy;

    std::array<int, s> largeArr = lz::range(static_cast<int>(s)).toArray<s>();
    std::array<int, SizePolicy> toLargeExcept = lz::range(static_cast<int>(SizePolicy)).toArray<SizePolicy>();

    for (auto _ : state) {
        auto ex = lz::except(largeArr, toLargeExcept);

        for (auto excepted : ex) {
            benchmark::DoNotOptimize(excepted);
        }
    }
}


BENCHMARK(BM_Except);
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