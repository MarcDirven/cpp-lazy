#include <benchmark/benchmark.h>
#include <utility>
#include <sstream>

#include <Lz.hpp>


constexpr static size_t SizePolicy = 32;

static void Enumerate(benchmark::State& state) {
    std::array<int, SizePolicy> arr{};

    for (auto _ : state) {
        auto enumeration = lz::enumerate(arr);

        for (const std::pair<int, int> pair : enumeration) {
            benchmark::DoNotOptimize(pair);
        }
    }
}

static void Filter(benchmark::State& state) {
    std::array<int, SizePolicy> arr{};

    for (auto _ : state) {
        auto filter = lz::filter(arr, [](const int i) { return i != 0; });

        for (const int filtered : filter) {
            benchmark::DoNotOptimize(filtered);
        }
    }
}

static void Map(benchmark::State& state) {
    std::array<int, SizePolicy> arr{};

    for (auto _ : state) {
        auto map = lz::map(arr, [](const int i) { return i == 0 ? 10 : 5; });

        for (const int mapped : map) {
            benchmark::DoNotOptimize(mapped);
        }
    }
}

static void Range(benchmark::State& state) {
    for (auto _ : state) {
        auto range = lz::range(SizePolicy);

        for (const int i : range) {
            benchmark::DoNotOptimize(i);
        }
    }
}

static void StringSplitter(benchmark::State& state) {
    std::string toSplit = "hello hello hello hello hello he";

    for (auto _ : state) {
        auto splitter = lz::split(toSplit, " ");

#ifdef CXX_LT_17
        // making non const causes: benchmark/benchmark.h:322:48: internal compiler error: in assign_temp,
        // at function.c:977
        using StringType = const std::string&;
#else
        using StringType = const std::string_view;
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

        for (const int taken : takeWhile) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

static void Take(benchmark::State& state) {
    std::array<int, SizePolicy> array = lz::range(static_cast<int>(SizePolicy)).toArray<SizePolicy>();

    for (auto _ : state) {
        auto taken = lz::take(array, 32);

        for (const int i : taken) {
            benchmark::DoNotOptimize(i);
        }
    }
}

static void Slice(benchmark::State& state) {
    std::array<int, SizePolicy> array = lz::range(static_cast<int>(SizePolicy)).toArray<SizePolicy>();

    for (auto _ : state) {
        auto sliced = lz::slice(array, 0, 32);

        for (const int i : sliced) {
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

        for (const auto tuple : zipper) {
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

        for (const auto tuple : zipper) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

static void Zip2(benchmark::State& state) {
    std::array<int, SizePolicy> arrayA{};
    std::array<int, SizePolicy> arrayB{};

    for (auto _ : state) {
        auto zipper = lz::zip(arrayA, arrayB);

        for (const auto tuple : zipper) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

static void ExceptSorted(benchmark::State& state) {
    std::array<int, SizePolicy> largeArr = lz::range(static_cast<int>(SizePolicy)).toArray<SizePolicy>();
    std::array<int, SizePolicy / 2> toLargeExcept =
        lz::range(static_cast<int>(SizePolicy) / 2).toArray<SizePolicy / 2>();

    for (auto _ : state) {
        auto ex = lz::except(largeArr, toLargeExcept);

        for (const auto excepted : ex) {
            benchmark::DoNotOptimize(excepted);
        }
    }
}

static void ExceptUnsorted(benchmark::State& state) {
    std::array<int, SizePolicy> largeArr = lz::range(static_cast<int>(SizePolicy)).toArray<SizePolicy>();
    std::array<int, SizePolicy / 2> toLargeExcept = {3, 2, 5, 2, 5, 6, 1, 3, 2, 5, 6, 3, 2, 1, 5, 6};

    for (auto _ : state) {
        auto ex = lz::except(largeArr, toLargeExcept);

        for (const auto excepted : ex) {
            benchmark::DoNotOptimize(excepted);
        }
    }
}

static void Repeat(benchmark::State& state) {
    for (auto _ : state) {
        auto repeater = lz::repeat(0, SizePolicy);

        for (const int r : repeater) {
            benchmark::DoNotOptimize(r);
        }
    }
}

static void TakeEvery(benchmark::State& state) {
    std::array<int, SizePolicy> array{};
    constexpr size_t offset = 2;

    for (auto _ : state) {
        auto takeEvery = lz::takeevery(array, offset);

        for (const int taken : takeEvery) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

static void Random(benchmark::State& state) {
    for (auto _ : state) {
        auto random = lz::random(0, 32, SizePolicy);
        for (const int i : random) {
            benchmark::DoNotOptimize(i);
        }
    }
}

static void DropWhile(benchmark::State& state) {
    int cnt = 0;
    std::array<int, SizePolicy> array = lz::generate([&cnt]() {
        if (cnt++ == SizePolicy / 2) {
            return *lz::random(2, 1024, 1).begin();
        }
        return 1;
    }, SizePolicy).toArray<SizePolicy>();


    for (auto _ : state) {
        auto random = lz::dropwhile(array, [](const int i) {
            return i == 1;
        });

        for (const int i : random) {
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

        for (const int i : chooser) {
            benchmark::DoNotOptimize(i);
        }
    }
}

static void Generate(benchmark::State& state) {
    size_t cnt = 0;
    for (auto _ : state) {
        auto generator = lz::generate([&cnt]() { return cnt++; }, SizePolicy);

        for (const auto i : generator) {
            benchmark::DoNotOptimize(i);
        }
    }
}

static void Concatenate(benchmark::State& state) {
    std::array<int, SizePolicy> arr = {3, 2, 5, 6, 3, 5, 7, 3, 6, 4, 3, 7, 5, 34, 3, 5, 6, 7, 45, 2, 4, 4, 2, 34, 56, 4, 3, 6, 1, 6, 4, 3};

    for (auto _ : state) {
        auto unique = lz::unique(arr);

        for (const char c : unique) {
            benchmark::DoNotOptimize(c);
        }
    }
}

static void UniquePreSorted(benchmark::State& state) {
    std::array<int, SizePolicy> arr = lz::range<int>(SizePolicy).toArray<SizePolicy>();

    for (auto _ : state) {
        auto unique = lz::unique(arr);

        for (const char c : unique) {
            benchmark::DoNotOptimize(c);
        }
    }
}

static void UniqueUnsorted(benchmark::State& state) {
    std::array<int, SizePolicy> arr = lz::random<int>(0, 4096, SizePolicy).toArray<SizePolicy>();

    for (auto _ : state) {
        auto unique = lz::unique(arr);

        for (const char c : unique) {
            benchmark::DoNotOptimize(c);
        }
    }
}

BENCHMARK(Choose);
BENCHMARK(Concatenate);
BENCHMARK(DropWhile);
BENCHMARK(Enumerate);
BENCHMARK(ExceptSorted);
BENCHMARK(ExceptUnsorted);
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
BENCHMARK(UniquePreSorted);
BENCHMARK(UniqueUnsorted);
BENCHMARK(Zip4);
BENCHMARK(Zip3);
BENCHMARK(Zip2);

BENCHMARK_MAIN();
