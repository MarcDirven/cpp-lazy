#include <Lz/Lz.hpp>
#include <benchmark/benchmark.h>
#include <sstream>
#include <utility>

namespace {
constexpr std::size_t SizePolicy = 32;

void CartesianProduct(benchmark::State& state) {
    std::array<int, SizePolicy / 8> a{};
    std::array<char, SizePolicy / 4> b{};

    for (auto _ : state) {
        for (auto&& tup : lz::cartesian(a, b)) {
            benchmark::DoNotOptimize(tup);
        }
    }
}

void ChunkIf(benchmark::State& state) {
    std::array<int, SizePolicy> a = lz::range<int>(SizePolicy).to<std::array<int, SizePolicy>>();
    constexpr static auto half = static_cast<int>(SizePolicy / 2);

    for (auto _ : state) {
        for (auto&& x : lz::chunkIf(a, [](int i) noexcept { return i == half; })) {
            for (int y : x) {
                benchmark::DoNotOptimize(y);
            }
        }
    }
}

void Chunks(benchmark::State& state) {
    std::array<int, SizePolicy> a = lz::range<int>(SizePolicy).to<std::array<int, SizePolicy>>();
    for (auto _ : state) {
        for (auto&& chunk : lz::chunks(a, 8)) {
            for (int x : chunk) {
                benchmark::DoNotOptimize(x);
            }
        }
    }
}

void Concatenate(benchmark::State& state) {
    std::string a(SizePolicy / 2, '0');
    std::string b(SizePolicy / 2, '1');

    for (auto _ : state) {
        for (char c : lz::concat(a, b)) {
            benchmark::DoNotOptimize(c);
        }
    }
}

void CString(benchmark::State& state) {
    constexpr const char* str = "this is a 32 char long stringggg";
    if (std::strlen(str) != SizePolicy) {
        throw std::runtime_error(fmt::format("String is not {} characters long", SizePolicy));
    }

    for (auto _ : state) {
        for (char c : lz::c_string(str)) {
            benchmark::DoNotOptimize(c);
        }
    }
}

void Enumerate(benchmark::State& state) {
    std::array<int, SizePolicy> arr{};

    for (auto _ : state) {
        for (auto pair : lz::enumerate(arr)) {
            benchmark::DoNotOptimize(pair);
        }
    }
}

void Except(benchmark::State& state) {
    std::array<int, SizePolicy> largeArr = lz::range(static_cast<int>(SizePolicy)).to<std::array<int, SizePolicy>>();
    std::array<int, SizePolicy / 2> toLargeExcept =
        lz::range(static_cast<int>(SizePolicy) / 2).to<std::array<int, SizePolicy / 2>>();

    for (auto _ : state) {
        for (auto excepted : lz::except(largeArr, toLargeExcept)) {
            benchmark::DoNotOptimize(excepted);
        }
    }
}

void Exclude(benchmark::State& state) {
    std::array<int, SizePolicy> a = lz::range<int>(SizePolicy).to<std::array<int, SizePolicy>>();

    for (auto _ : state) {
        for (int i : lz::exclude(a, 5, 10)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void ExclusiveScan(benchmark::State& state) {
    auto array = lz::range(SizePolicy).to<std::array<int, SizePolicy>>();

    for (auto _ : state) {
        for (std::size_t i : lz::eScan(array, static_cast<std::size_t>(0))) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void Filter(benchmark::State& state) {
    std::array<int, SizePolicy> arr{};

    for (auto _ : state) {
        for (int filtered : lz::filter(arr, [](const int i) noexcept { return i == 0; })) {
            benchmark::DoNotOptimize(filtered);
        }
    }
}

void Flatten(benchmark::State& state) {
    std::array<std::array<int, SizePolicy / 4>, SizePolicy / 8> arr{};
    for (auto _ : state) {
        for (auto&& val : lz::flatten(arr)) {
            benchmark::DoNotOptimize(val);
        }
    }
}

void Generate(benchmark::State& state) {
    size_t cnt = 0;

    for (auto _ : state) {
        for (auto i : lz::generate([&cnt]() noexcept { return cnt++; }, SizePolicy)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void GenerateWhile(benchmark::State& state) {
    for (auto _ : state) {
        for (auto i : lz::generateWhile(
                 [](std::size_t& cnt) -> std::pair<bool, std::size_t> {
                     const auto oldValue = cnt++;
                     return { oldValue < SizePolicy, cnt };
                 },
                 static_cast<std::size_t>(0))) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void GroupBy(benchmark::State& state) {
    std::array<int, SizePolicy> arr = lz::range<int>(SizePolicy).to<std::array<int, SizePolicy>>();

    for (auto _ : state) {
        for (auto &&group : lz::groupBy(arr, [](int a, int b) noexcept { return a == b; })) {
            benchmark::DoNotOptimize(group.first);
            for (auto&& pair : group.second) {
                benchmark::DoNotOptimize(pair);
            }
        }
    }
}

void InclusiveScan(benchmark::State& state) {
    auto array = lz::range(SizePolicy).to<std::array<int, SizePolicy>>();
    auto t = lz::iScan(array);
    auto d = std::distance(t.begin(), t.end());
    (void)d;

    for (auto _ : state) {
        for (auto i : lz::iScan(array)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void JoinInt(benchmark::State& state) {
    std::array<int, SizePolicy> arr = lz::range<int>(SizePolicy).to<std::array<int, SizePolicy>>();

    for (auto _ : state) {
        for (std::string s : lz::join(arr, ",")) {
            benchmark::DoNotOptimize(s);
        }
    }
}

void JoinString(benchmark::State& state) {
    std::array<std::string, SizePolicy> arr = lz::repeat(std::string("hello"), SizePolicy).to<std::array<int, SizePolicy>>();

    for (auto _ : state) {
        for (std::string& s : lz::join(arr, ",")) {
            benchmark::DoNotOptimize(s);
        }
    }
}

void JoinWhere(benchmark::State& state) {
    std::vector<int> arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    std::vector<int> toJoin = { 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };

    auto randomIndex = lz::random<std::size_t>(0, toJoin.size() - 1);
    toJoin[randomIndex.nextRandom()] = arr[randomIndex.nextRandom()]; // Create a value where both values are equal
    std::sort(toJoin.begin(), toJoin.end());

    for (auto _ : state) {
        for (std::tuple<int, int> val : lz::joinWhere(
                 arr, toJoin, [](int i) noexcept { return i; }, [](int i) noexcept { return i; },
                 [](int a, int b) noexcept { return std::make_tuple(a, b); })) {
            benchmark::DoNotOptimize(val);
        }
    }
}

void Map(benchmark::State& state) {
    std::array<int, SizePolicy> arr{};

    for (auto _ : state) {
        for (int mapped : lz::map(arr, [](const int i) noexcept { return i == 0 ? 10 : 5; })) {
            benchmark::DoNotOptimize(mapped);
        }
    }
}

void Random(benchmark::State& state) {
    for (auto _ : state) {
        for (int i : lz::random(0, 32, SizePolicy)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void Range(benchmark::State& state) {
    for (auto _ : state) {
        for (int i : lz::range<int>(SizePolicy)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void RegexSplit(benchmark::State& state) {
    std::string toSplit = "hello hello hello hello hello he";
    std::regex r(" ");

    for (auto _ : state) {
        for (lz::StringView substring : lz::regexSplit(toSplit, r)) {
            benchmark::DoNotOptimize(substring);
        }
    }
}

void Repeat(benchmark::State& state) {
    for (auto _ : state) {
        for (int r : lz::repeat(0, SizePolicy)) {
            benchmark::DoNotOptimize(r);
        }
    }
}

void Rotate(benchmark::State& state) {
    std::array<int, SizePolicy> arr = lz::range<int>(SizePolicy).to<std::array<int, SizePolicy>>();

    for (auto _ : state) {
        for (int i : lz::rotate(arr.begin() + 5, arr.begin(), arr.end())) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void StringSplitter(benchmark::State& state) {
    std::string toSplit = "hello hello hello hello hello he";

    for (auto _ : state) {
        for (lz::StringView substring : lz::split(toSplit, ' ')) {
            benchmark::DoNotOptimize(substring);
        }
    }
}

void TakeWhile(benchmark::State& state) {
    std::array<int, SizePolicy> array = lz::range(static_cast<int>(SizePolicy)).to<std::array<int, SizePolicy>>();

    for (auto _ : state) {
        for (int taken : lz::take_while(array, [](const int i) noexcept { return i != SizePolicy - 1; })) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

void TakeEvery(benchmark::State& state) {
    constexpr size_t offset = 2;
    std::array<int, SizePolicy * offset> array{};

    for (auto _ : state) {
        for (int taken : lz::take_every(array, offset)) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

void DropWhile(benchmark::State& state) {
    std::array<int, SizePolicy> array = lz::generate(
                                            [](int& cnt) {
                                                if (cnt++ == SizePolicy / 2) {
                                                    return *lz::random(2, 1024, 1).begin();
                                                }
                                                return 1;
                                            },
                                            SizePolicy, 0)
                                            .to<std::array<int, SizePolicy>>();

    for (auto _ : state) {
        for (int i : lz::dropWhile(array, [](const int i) noexcept { return i == 1; })) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void Unique(benchmark::State& state) {
    std::array<int, SizePolicy> arr = lz::range<int>(SizePolicy).to<std::array<int, SizePolicy>>();

    for (auto _ : state) {
        for (char c : lz::unique(arr)) {
            benchmark::DoNotOptimize(c);
        }
    }
}

void Zip4(benchmark::State& state) {
    std::array<int, SizePolicy> arrayA{};
    std::array<int, SizePolicy> arrayB{};
    std::array<int, SizePolicy> arrayC{};
    std::array<int, SizePolicy> arrayD{};

    for (auto _ : state) {
        for (auto tuple : lz::zip(arrayA, arrayB, arrayC, arrayD)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void Zip3(benchmark::State& state) {
    std::array<int, SizePolicy> arrayA{};
    std::array<int, SizePolicy> arrayB{};
    std::array<int, SizePolicy> arrayC{};

    for (auto _ : state) {
        for (auto tuple : lz::zip(arrayA, arrayB, arrayC)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void Zip2(benchmark::State& state) {
    std::array<int, SizePolicy> arrayA{};
    std::array<int, SizePolicy> arrayB{};

    for (auto _ : state) {
        for (auto tuple : lz::zip(arrayA, arrayB)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void ZipLongest4(benchmark::State& state) {
    std::array<int, SizePolicy> arrayA{};
    std::array<int, SizePolicy - 1> arrayB{};
    std::array<int, SizePolicy - 2> arrayC{};
    std::array<int, SizePolicy - 3> arrayD{};

    for (auto _ : state) {
        for (auto tuple : lz::zip_longest(arrayA, arrayB, arrayC, arrayD)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void ZipLongest3(benchmark::State& state) {
    std::array<int, SizePolicy> arrayA{};
    std::array<int, SizePolicy - 1> arrayB{};
    std::array<int, SizePolicy - 2> arrayC{};

    for (auto _ : state) {
        for (auto tuple : lz::zip_longest(arrayA, arrayB, arrayC)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void ZipLongest2(benchmark::State& state) {
    std::array<int, SizePolicy> arrayA{};
    std::array<int, SizePolicy - 1> arrayB{};

    for (auto _ : state) {
        for (auto tuple : lz::zip_longest(arrayA, arrayB)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}
} // namespace

BENCHMARK(CartesianProduct);
BENCHMARK(ChunkIf);
BENCHMARK(Chunks);
BENCHMARK(Concatenate);
BENCHMARK(CString);
BENCHMARK(Enumerate);
BENCHMARK(Except);
BENCHMARK(Exclude);
BENCHMARK(ExclusiveScan);
BENCHMARK(Filter);
BENCHMARK(Flatten);
BENCHMARK(DropWhile);
BENCHMARK(Generate);
BENCHMARK(GenerateWhile);
BENCHMARK(GroupBy);
BENCHMARK(InclusiveScan);
BENCHMARK(JoinInt);
BENCHMARK(JoinString);
BENCHMARK(JoinWhere);
BENCHMARK(Map);
BENCHMARK(Range);
BENCHMARK(RegexSplit);
BENCHMARK(Random);
BENCHMARK(Repeat);
BENCHMARK(Rotate);
BENCHMARK(StringSplitter);
BENCHMARK(TakeWhile);
BENCHMARK(TakeEvery);
BENCHMARK(Unique);
BENCHMARK(Zip4);
BENCHMARK(Zip3);
BENCHMARK(Zip2);
BENCHMARK(ZipLongest4);
BENCHMARK(ZipLongest3);
BENCHMARK(ZipLongest2);

BENCHMARK_MAIN();
