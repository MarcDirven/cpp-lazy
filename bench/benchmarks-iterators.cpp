#include <benchmark/benchmark.h>
#include <utility>
#include <sstream>

#include <Lz.hpp>


constexpr static size_t SizePolicy = 32;

static void Enumerate(benchmark::State& state) {
    std::array<int, SizePolicy> arr{};

    for (auto _ : state) {
        auto enumeration = lz::enumerate(arr);

        for (std::pair<int, int> pair : enumeration) {
            benchmark::DoNotOptimize(pair);
        }
    }
}

static void Filter(benchmark::State& state) {
    std::array<int, SizePolicy> arr{};

    for (auto _ : state) {
        auto filter = lz::filter(arr, [](const int i) { return i != 0; });

        for (int filtered : filter) {
            benchmark::DoNotOptimize(filtered);
        }
    }
}

static void Map(benchmark::State& state) {
    std::array<int, SizePolicy> arr{};

    for (auto _ : state) {
        auto map = lz::map(arr, [](const int i) { return i == 0 ? 10 : 5; });

        for (int mapped : map) {
            benchmark::DoNotOptimize(mapped);
        }
    }
}

static void Range(benchmark::State& state) {
    for (auto _ : state) {
        auto range = lz::range(SizePolicy);

        for (int i : range) {
            benchmark::DoNotOptimize(i);
        }
    }
}

static void StringSplitter(benchmark::State& state) {
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

static void TakeWhile(benchmark::State& state) {
    std::array<int, SizePolicy> array = lz::range(static_cast<int>(SizePolicy)).toArray<SizePolicy>();

    for (auto _ : state) {
        auto takeWhile = lz::takewhile(array, [](const int i) { return i != SizePolicy - 1; });

        for (int taken : takeWhile) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

static void Take(benchmark::State& state) {
    std::array<int, SizePolicy> array = lz::range(static_cast<int>(SizePolicy)).toArray<SizePolicy>();

    for (auto _ : state) {
        auto taken = lz::take(array, 32);

        for (int i : taken) {
            benchmark::DoNotOptimize(i);
        }
    }
}

static void Slice(benchmark::State& state) {
    std::array<int, SizePolicy> array = lz::range(static_cast<int>(SizePolicy)).toArray<SizePolicy>();

    for (auto _ : state) {
        auto sliced = lz::slice(array, 0, 32);

        for (int i : sliced) {
            benchmark::DoNotOptimize(i);
        }
    }
}

static void Zip4(benchmark::State& state) {
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

static void Zip3(benchmark::State& state) {
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

static void Zip2(benchmark::State& state) {
    std::array<int, SizePolicy> arrayA{};
    std::array<int, SizePolicy> arrayB{};

    for (auto _ : state) {
        auto zipper = lz::zip(arrayA, arrayB);

        for (auto tuple : zipper) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

static void Except(benchmark::State& state) {
    std::array<int, SizePolicy> largeArr = lz::range(static_cast<int>(SizePolicy)).toArray<SizePolicy>();
    std::array<int, SizePolicy / 2> toLargeExcept =
        lz::range(static_cast<int>(SizePolicy) / 2).toArray<SizePolicy / 2>();

    for (auto _ : state) {
        auto ex = lz::except(largeArr, toLargeExcept);

        for (auto excepted : ex) {
            benchmark::DoNotOptimize(excepted);
        }
    }
}

static void Repeat(benchmark::State& state) {
    for (auto _ : state) {
        auto repeater = lz::repeat(0, SizePolicy);

        for (int r : repeater) {
            benchmark::DoNotOptimize(r);
        }
    }
}

static void TakeEvery(benchmark::State& state) {
    std::array<int, SizePolicy> array{};
    constexpr size_t offset = 2;

    for (auto _ : state) {
        auto takeEvery = lz::takeevery(array, offset);

        for (int taken : takeEvery) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

static void Random(benchmark::State& state) {
    for (auto _ : state) {
        auto random = lz::random(0, 32, SizePolicy);
        for (int i : random) {
            benchmark::DoNotOptimize(i);
        }
    }
}

static void Choose(benchmark::State& state) {
    std::string s = "123df574d587f85432df52f4ssf5d222";
    for (auto _ : state) {
        auto chooser = lz::choose(s, [](const char c) {
            return std::make_pair(static_cast<bool>(std::isdigit(c)), static_cast<int>(c - '0'));
        });

        for (int i : chooser) {
            benchmark::DoNotOptimize(i);
        }
    }
}

static void Generate(benchmark::State& state) {
    size_t cnt = 0;
    for (auto _ : state) {
        auto generator = lz::generate([&cnt]() { return cnt++; }, SizePolicy);

        for (auto i : generator) {
            benchmark::DoNotOptimize(i);
        }
    }
}

BENCHMARK(Choose);
BENCHMARK(Enumerate);
BENCHMARK(Except);
BENCHMARK(Filter);
BENCHMARK(Generate);
BENCHMARK(Map);
BENCHMARK(Range);
BENCHMARK(Random);
BENCHMARK(Repeat);
BENCHMARK(StringSplitter);
BENCHMARK(Take);
BENCHMARK(TakeWhile);
BENCHMARK(TakeEvery);
BENCHMARK(Slice);
BENCHMARK(Zip4);
BENCHMARK(Zip3);
BENCHMARK(Zip2);

BENCHMARK_MAIN();
