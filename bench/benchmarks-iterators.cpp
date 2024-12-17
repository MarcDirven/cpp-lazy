#include <Lz/Lz.hpp>
#include <benchmark/benchmark.h>
#include <sstream>
#include <utility>

namespace {
constexpr std::size_t size_policy = 32;

void CartesianProduct(benchmark::State& state) {
    std::array<int, size_policy / 8> a{};
    std::array<char, size_policy / 4> b{};

    for (auto _ : state) {
        for (auto&& tup : lz::cartesian_product(a, b)) {
            benchmark::DoNotOptimize(tup);
        }
    }
}

void ChunkIf(benchmark::State& state) {
    std::array<int, size_policy> a = lz::range<int>(size_policy).to<std::array<int, size_policy>>();
    constexpr static auto half = static_cast<int>(size_policy / 2);

    for (auto _ : state) {
        for (auto&& x : lz::chunk_if(a, [](int i) noexcept { return i == half; })) {
            for (int y : x) {
                benchmark::DoNotOptimize(y);
            }
        }
    }
}

void Chunks(benchmark::State& state) {
    std::array<int, size_policy> a = lz::range<int>(size_policy).to<std::array<int, size_policy>>();
    for (auto _ : state) {
        for (auto&& chunk : lz::chunks(a, 8)) {
            for (int x : chunk) {
                benchmark::DoNotOptimize(x);
            }
        }
    }
}

void Concatenate(benchmark::State& state) {
    std::string a(size_policy / 2, '0');
    std::string b(size_policy / 2, '1');

    for (auto _ : state) {
        for (char c : lz::concat(a, b)) {
            benchmark::DoNotOptimize(c);
        }
    }
}

void CString(benchmark::State& state) {
    constexpr const char* str = "this is a 32 char long stringggg";
    if (std::strlen(str) != size_policy) {
        throw std::runtime_error(fmt::format("String is not {} characters long", size_policy));
    }

    for (auto _ : state) {
        for (char c : lz::c_string(str)) {
            benchmark::DoNotOptimize(c);
        }
    }
}

void Enumerate(benchmark::State& state) {
    std::array<int, size_policy> arr{};

    for (auto _ : state) {
        for (auto pair : lz::enumerate(arr)) {
            benchmark::DoNotOptimize(pair);
        }
    }
}

void Except(benchmark::State& state) {
    std::array<int, size_policy> large_arr = lz::range(static_cast<int>(size_policy)).to<std::array<int, size_policy>>();
    std::array<int, size_policy / 2> large_arr_except =
        lz::range(static_cast<int>(size_policy) / 2).to<std::array<int, size_policy / 2>>();

    for (auto _ : state) {
        for (auto excepted : lz::except(large_arr, large_arr_except)) {
            benchmark::DoNotOptimize(excepted);
        }
    }
}

void Exclude(benchmark::State& state) {
    std::array<int, size_policy> a = lz::range<int>(size_policy).to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int i : lz::exclude(a, 5, 10)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void ExclusiveScan(benchmark::State& state) {
    auto array = lz::range(size_policy).to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (std::size_t i : lz::exclusive_scan(array, static_cast<std::size_t>(0))) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void Filter(benchmark::State& state) {
    std::array<int, size_policy> arr{};

    for (auto _ : state) {
        for (int filtered : lz::filter(arr, [](const int i) noexcept { return i == 0; })) {
            benchmark::DoNotOptimize(filtered);
        }
    }
}

void Flatten(benchmark::State& state) {
    std::array<std::array<int, size_policy / 4>, size_policy / 8> arr{};
    for (auto _ : state) {
        for (auto&& val : lz::flatten(arr)) {
            benchmark::DoNotOptimize(val);
        }
    }
}

void Generate(benchmark::State& state) {
    size_t cnt = 0;

    for (auto _ : state) {
        for (auto i : lz::generate([&cnt]() noexcept { return cnt++; }, size_policy)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void GenerateWhile(benchmark::State& state) {
    for (auto _ : state) {
        for (auto i : lz::generate_while(
                 [](std::size_t& cnt) -> std::pair<bool, std::size_t> {
                     const auto old_value = cnt++;
                     return { old_value < size_policy, cnt };
                 },
                 static_cast<std::size_t>(0))) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void GroupBy(benchmark::State& state) {
    std::array<int, size_policy> arr = lz::range<int>(size_policy).to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (auto&& group : lz::group_by(arr, [](int a, int b) noexcept { return a == b; })) {
            benchmark::DoNotOptimize(group.first);
            for (auto&& pair : group.second) {
                benchmark::DoNotOptimize(pair);
            }
        }
    }
}

void InclusiveScan(benchmark::State& state) {
    auto array = lz::range(size_policy).to<std::array<int, size_policy>>();
    auto t = lz::exclusive_scan(array);
    auto d = std::distance(t.begin(), t.end());
    (void)d;

    for (auto _ : state) {
        for (auto i : lz::exclusive_scan(array)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void JoinInt(benchmark::State& state) {
    std::array<int, size_policy> arr = lz::range<int>(size_policy).to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (std::string s : lz::join(arr, ",")) {
            benchmark::DoNotOptimize(s);
        }
    }
}

void JoinString(benchmark::State& state) {
    std::array<std::string, size_policy> arr = lz::repeat(std::string("hello"), size_policy).to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (std::string& s : lz::join(arr, ",")) {
            benchmark::DoNotOptimize(s);
        }
    }
}

void JoinWhere(benchmark::State& state) {
    std::vector<int> arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    std::vector<int> to_join = { 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };

    auto random_index = lz::random<std::size_t>(0, to_join.size() - 1);
    to_join[random_index.next_random()] = arr[random_index.next_random()]; // Create a value where both values are equal
    std::sort(to_join.begin(), to_join.end());

    for (auto _ : state) {
        for (std::tuple<int, int> val : lz::join_where(
                 arr, to_join, [](int i) noexcept { return i; }, [](int i) noexcept { return i; },
                 [](int a, int b) noexcept { return std::make_tuple(a, b); })) {
            benchmark::DoNotOptimize(val);
        }
    }
}

void Map(benchmark::State& state) {
    std::array<int, size_policy> arr{};

    for (auto _ : state) {
        for (int mapped : lz::map(arr, [](const int i) noexcept { return i == 0 ? 10 : 5; })) {
            benchmark::DoNotOptimize(mapped);
        }
    }
}

void Random(benchmark::State& state) {
    for (auto _ : state) {
        for (int i : lz::random(0, 32, size_policy)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void Range(benchmark::State& state) {
    for (auto _ : state) {
        for (int i : lz::range<int>(size_policy)) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void RegexSplit(benchmark::State& state) {
    std::string to_split = "hello hello hello hello hello he";
    std::regex r(" ");

    for (auto _ : state) {
        for (lz::string_view substring : lz::regex_split(to_split, r)) {
            benchmark::DoNotOptimize(substring);
        }
    }
}

void Repeat(benchmark::State& state) {
    for (auto _ : state) {
        for (int r : lz::repeat(0, size_policy)) {
            benchmark::DoNotOptimize(r);
        }
    }
}

void Rotate(benchmark::State& state) {
    std::array<int, size_policy> arr = lz::range<int>(size_policy).to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int i : lz::rotate(arr.begin() + 5, arr.begin(), arr.end())) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void StringSplitter(benchmark::State& state) {
    std::string to_split = "hello hello hello hello hello he";

    for (auto _ : state) {
        for (lz::StringView substring : lz::split(to_split, ' ')) {
            benchmark::DoNotOptimize(substring);
        }
    }
}

void TakeWhile(benchmark::State& state) {
    std::array<int, size_policy> array = lz::range(static_cast<int>(size_policy)).to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int taken : lz::take_while(array, [](const int i) noexcept { return i != size_policy - 1; })) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

void TakeEvery(benchmark::State& state) {
    constexpr size_t offset = 2;
    std::array<int, size_policy * offset> array{};

    for (auto _ : state) {
        for (int taken : lz::take_every(array, offset)) {
            benchmark::DoNotOptimize(taken);
        }
    }
}

void DropWhile(benchmark::State& state) {
    std::array<int, size_policy> array = lz::generate(
                                            [](int& cnt) {
                                                if (cnt++ == size_policy / 2) {
                                                    return *lz::random(2, 1024, 1).begin();
                                                }
                                                return 1;
                                            },
                                            size_policy, 0)
                                            .to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (int i : lz::drop_while(array, [](const int i) noexcept { return i == 1; })) {
            benchmark::DoNotOptimize(i);
        }
    }
}

void Unique(benchmark::State& state) {
    std::array<int, size_policy> arr = lz::range<int>(size_policy).to<std::array<int, size_policy>>();

    for (auto _ : state) {
        for (char c : lz::unique(arr)) {
            benchmark::DoNotOptimize(c);
        }
    }
}

void Zip4(benchmark::State& state) {
    std::array<int, size_policy> arr_a{};
    std::array<int, size_policy> arr_b{};
    std::array<int, size_policy> arr_c{};
    std::array<int, size_policy> arr_d{};

    for (auto _ : state) {
        for (auto tuple : lz::zip(arr_a, arr_b, arr_c, arr_d)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void Zip3(benchmark::State& state) {
    std::array<int, size_policy> arr_a{};
    std::array<int, size_policy> arr_b{};
    std::array<int, size_policy> arr_c{};

    for (auto _ : state) {
        for (auto tuple : lz::zip(arr_a, arr_b, arr_c)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void Zip2(benchmark::State& state) {
    std::array<int, size_policy> arr_a{};
    std::array<int, size_policy> arr_b{};

    for (auto _ : state) {
        for (auto tuple : lz::zip(arr_a, arr_b)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void ZipLongest4(benchmark::State& state) {
    std::array<int, size_policy> arr_a{};
    std::array<int, size_policy - 1> arr_b{};
    std::array<int, size_policy - 2> arr_c{};
    std::array<int, size_policy - 3> arr_d{};

    for (auto _ : state) {
        for (auto tuple : lz::zip_longest(arr_a, arr_b, arr_c, arr_d)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void ZipLongest3(benchmark::State& state) {
    std::array<int, size_policy> arr_a{};
    std::array<int, size_policy - 1> arr_b{};
    std::array<int, size_policy - 2> arr_c{};

    for (auto _ : state) {
        for (auto tuple : lz::zip_longest(arr_a, arr_b, arr_c)) {
            benchmark::DoNotOptimize(tuple);
        }
    }
}

void ZipLongest2(benchmark::State& state) {
    std::array<int, size_policy> arr_a{};
    std::array<int, size_policy - 1> arr_b{};

    for (auto _ : state) {
        for (auto tuple : lz::zip_longest(arr_a, arr_b)) {
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
