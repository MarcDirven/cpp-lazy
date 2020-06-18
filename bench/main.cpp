#include <benchmark/benchmark.h>
#include <Lz.hpp>


static void BM_EnumerateCreation(benchmark::State& state) {
    for (auto _ : state) {
        std::string empty_string;
    }
}

BENCHMARK(BM_EnumerateCreation);

BENCHMARK_MAIN();