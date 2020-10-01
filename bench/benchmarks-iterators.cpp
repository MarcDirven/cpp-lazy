#include <benchmark/benchmark.h>
#include <utility>
#include <sstream>

#include <Lz/Lz.hpp>


constexpr static size_t SizePolicy = 32;

static void Enumerate(benchmark::State& state) {
    std::array<int, SizePolicy> arr{};
    auto enumeration = lz::enumerate(arr);

    for (auto _ : state) {
        for (const std::pair<int, int> pair : enumeration) {
            benchmark::DoNotOptimize(pair);
        }
    }
}

static void Filter(benchmark::State& state) {
    std::array<int, SizePolicy> arr{};
    auto filter = lz::filter(arr, [](const int i) { return i == 0; });

    for (auto _ : state) {
        for (const int filtered : filter) {
            benchmark::DoNotOptimize(filtered);
        }
    }
}

static void Map(benchmark::State& state) {
    std::array<int, SizePolicy> arr{};
    auto map = lz::map(arr, [](const int i) { return i == 0 ? 10 : 5; });

    for (auto _ : state) {
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
    auto splitter = lz::split(toSplit, " ");

    for (auto _ : state) {
#ifndef LZ_HAS_CXX17
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
    auto takeWhile = lz::takeWhile(array, [](const int i) { return i != SizePolicy - 1; });

    for (auto _ : state) {
        for (const int taken : takeWhile) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

static void Take(benchmark::State& state) {
    std::array<int, SizePolicy> array = lz::range(static_cast<int>(SizePolicy)).toArray<SizePolicy>();
    auto taken = lz::take(array, 32);

    for (auto _ : state) {
        for (const int i : taken) {
            benchmark::DoNotOptimize(i);
        }
    }
}

static void Slice(benchmark::State& state) {
    std::array<int, SizePolicy> array = lz::range(static_cast<int>(SizePolicy)).toArray<SizePolicy>();
    auto sliced = lz::slice(array, 0, 32);

    for (auto _ : state) {
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
    auto zipper = lz::zip(arrayA, arrayB, arrayC, arrayD);

    for (auto _ : state) {
        for (const auto tuple : zipper) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

static void Zip3(benchmark::State& state) {
    std::array<int, SizePolicy> arrayA{};
    std::array<int, SizePolicy> arrayB{};
    std::array<int, SizePolicy> arrayC{};
    auto zipper = lz::zip(arrayA, arrayB, arrayC);

    for (auto _ : state) {
        for (const auto tuple : zipper) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

static void Zip2(benchmark::State& state) {
    std::array<int, SizePolicy> arrayA{};
    std::array<int, SizePolicy> arrayB{};
    auto zipper = lz::zip(arrayA, arrayB);

    for (auto _ : state) {
        for (const auto tuple : zipper) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

static void Except(benchmark::State& state) {
    std::array<int, SizePolicy> largeArr = lz::range(static_cast<int>(SizePolicy)).toArray<SizePolicy>();
    std::array<int, SizePolicy / 2> toLargeExcept =
        lz::range(static_cast<int>(SizePolicy) / 2).toArray<SizePolicy / 2>();
    auto ex = lz::except(largeArr, toLargeExcept);

    for (auto _ : state) {
        for (const auto excepted : ex) {
            benchmark::DoNotOptimize(excepted);
        }
    }
}

static void Repeat(benchmark::State& state) {
    auto repeater = lz::repeat(0, SizePolicy);

    for (auto _ : state) {
        for (const int r : repeater) {
            benchmark::DoNotOptimize(r);
        }
    }
}

static void TakeEvery(benchmark::State& state) {
    constexpr size_t offset = 2;
    std::array<int, SizePolicy * offset> array{};
    auto takeEvery = lz::takeEvery(array, offset);

    for (auto _ : state) {
        for (const int taken : takeEvery) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

static void Random(benchmark::State& state) {
    auto random = lz::random(0, 32, SizePolicy);

    for (auto _ : state) {
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

    auto drop = lz::dropWhile(array, [](const int i) {
        return i == 1;
    });

    for (auto _ : state) {
        for (const int i : drop) {
            benchmark::DoNotOptimize(i);
        }
    }
}

static void Generate(benchmark::State& state) {
    size_t cnt = 0;
    auto generator = lz::generate([&cnt]() { return cnt++; }, SizePolicy);

    for (auto _ : state) {
        for (const auto i : generator) {
            benchmark::DoNotOptimize(i);
        }
    }
}

static void Concatenate(benchmark::State& state) {
    std::string a(SizePolicy / 2, '0');
    std::string b(SizePolicy / 2, '1');
    auto concatenate = lz::concat(a, b);

    for (auto _ : state) {
        for (const char c : concatenate) {
            benchmark::DoNotOptimize(c);
        }
    }
}

static void Unique(benchmark::State& state) {
    std::array<int, SizePolicy> arr = lz::range<int>(SizePolicy).toArray<SizePolicy>();
    auto unique = lz::unique(arr);

    for (auto _ : state) {
        for (const char c : unique) {
            benchmark::DoNotOptimize(c);
        }
    }
}

static void JoinInt(benchmark::State& state) {
    std::array<int, SizePolicy> arr = lz::range<int>(SizePolicy).toArray<SizePolicy>();
    auto join = lz::join(arr, ",");

    for (auto _ : state) {
        for (const std::string s : join) {
            benchmark::DoNotOptimize(s);
        }
    }
}

static void JoinString(benchmark::State& state) {
    std::array<std::string, SizePolicy> arr = lz::repeat(std::string("hello"), SizePolicy).toArray<SizePolicy>();
    auto join = lz::join(arr, ",");

    for (auto _ : state) {
        for (const std::string& s : join) {
            benchmark::DoNotOptimize(s);
        }
    }
}

BENCHMARK(Concatenate);
BENCHMARK(DropWhile);
BENCHMARK(Enumerate);
BENCHMARK(Except);
BENCHMARK(Filter);
BENCHMARK(Generate);
BENCHMARK(JoinInt);
BENCHMARK(JoinString);
BENCHMARK(Map);
BENCHMARK(Range);
BENCHMARK(Random);
BENCHMARK(Repeat);
BENCHMARK(StringSplitter);
BENCHMARK(Take);
BENCHMARK(TakeWhile);
BENCHMARK(TakeEvery);
BENCHMARK(Slice);
BENCHMARK(Unique);
BENCHMARK(Zip4);
BENCHMARK(Zip3);
BENCHMARK(Zip2);

BENCHMARK_MAIN();
