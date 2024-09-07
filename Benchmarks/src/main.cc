#include <benchmark/benchmark.h>
#include <iostream>
#include <Logic/BallRepository.h>
#include <Logic/Mover.h>
#include <Logic/IDManager.h>

namespace
{
    const unsigned int size = 100000;
    const unsigned int block_size = 4;
    const unsigned int iterations = 1000000;
}

class Benchmark : public benchmark::Fixture 
{
public:
    Benchmark()
    {
        
        Iterations(iterations);
        
    }

protected:    

    //SOARepository repository;
};


BENCHMARK_F(Benchmark, TestSimd)(benchmark::State& state) 
{
    IDManager::Initialize();
    SIMDMover mover = SIMDMover(nullptr);
    auto repository = SOARepository(BALL_NUMBER);
    for (auto _ : state) 
    {
        mover.PredictPositions(repository);
        mover.UpdatePositions(repository);
    }
    IDManager::Destroy();
}

BENCHMARK_F(Benchmark, TestNormal)(benchmark::State& state)
{
    IDManager::Initialize();
    NormalMover mover = NormalMover(nullptr);
    auto repository = SOARepository(BALL_NUMBER);
    for (auto _ : state)
    {
        mover.PredictPositions(repository);
        mover.UpdatePositions(repository);
    }
    IDManager::Destroy();
}

// Run the benchmark
BENCHMARK_MAIN();