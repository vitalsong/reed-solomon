#include <benchmark/benchmark.h>
#include <reed-solomon.h>

void BM_RSEncoder(benchmark::State& state)
{
    auto rs = ReedSolomon(239, 16);
    std::vector<uint8_t> message(rs.msg_len());
    for (auto _ : state) {
        auto enc = rs.encode(message);
    }
}

void BM_RSDecoder(benchmark::State& state)
{
    auto rs = ReedSolomon(239, 16);
    std::vector<uint8_t> message(rs.msg_len());
    auto encoded = rs.encode(message);
    for (auto _ : state) {
        auto dec = rs.decode(encoded);
    }
}

BENCHMARK(BM_RSEncoder)->MinTime(5)->Unit(benchmark::TimeUnit::kMicrosecond);
BENCHMARK(BM_RSDecoder)->MinTime(5)->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_MAIN();