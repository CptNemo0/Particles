#include "Mover.h"
#include <immintrin.h>
#include <omp.h>
#include "pch.h"
#include <iostream>

NormalMover::NormalMover()
{
    
}

void NormalMover::Step(SOARepository& repository)
{
    PredictPositions(repository);
    UpdatePositions(repository);
}

void NormalMover::Init(const SOARepository& repository)
{

}

void NormalMover::PredictPositions(SOARepository& repository)
{
    int n = static_cast<int>(repository.size_);
#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        repository.nx_[i] = repository.x_[i] + repository.speedx_[i] * FIXED_DT;
        repository.ny_[i] = repository.y_[i] + repository.speedy_[i] * FIXED_DT;
    }
    
}

void NormalMover::UpdatePositions(SOARepository& repository)
{
    int n = static_cast<int>(repository.size_);
#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        repository.speedx_[i] = (repository.nx_[i] - repository.x_[i]) / FIXED_DT;
        repository.speedy_[i] = (repository.ny_[i] - repository.y_[i]) / FIXED_DT;

        repository.px_[i] = repository.x_[i];
        repository.py_[i] = repository.y_[i];

        repository.x_[i] = repository.nx_[i];
        repository.y_[i] = repository.ny_[i];

        repository.output_position_[i * 2] = repository.x_[i];
        repository.output_position_[i * 2 + 1] = repository.y_[i];
    }
}

void NormalMover::UpdateVelocities(SOARepository& repository)
{
    int n = static_cast<int>(repository.size_);
    float delta_time = FIXED_DT;  

    for (int i = 0; i < n; i++)
    {
        //repository.forcex_[i] = std::min(2.0f, repository.forcex_[i]);
        //repository.forcey_[i] = std::min(2.0f, repository.forcey_[i]);
        repository.speedx_[i] += FIXED_DT * repository.forcex_[i];// / repository.density_[i];
        repository.speedy_[i] += FIXED_DT * 100.0f; /// repository.density_[i];
    }
    
}

SIMDMover::SIMDMover()
{
    
}

void SIMDMover::Init(const SOARepository& repository)
{
    
}

void SIMDMover::Step(SOARepository& repository)
{
    PredictPositions(repository);
    UpdatePositions(repository);
}

void SIMDMover::PredictPositions(SOARepository& repository)
{
    int n = static_cast<int>(repository.size_);
    int step = SIMD_BLOCK_SIZE;
#pragma omp parallel for
    for (int i = 0; i < n; i += step)
    {
        const __m256 dt{ FIXED_DT, FIXED_DT, FIXED_DT, FIXED_DT, FIXED_DT, FIXED_DT, FIXED_DT, FIXED_DT };

        __m256 x = _mm256_load_ps(&repository.x_[i]);
        __m256 y = _mm256_load_ps(&repository.y_[i]);

        __m256 speedx = _mm256_load_ps(&repository.speedx_[i]);
        __m256 speedy = _mm256_load_ps(&repository.speedy_[i]);

        speedx = _mm256_mul_ps(speedx, dt);
        speedy = _mm256_mul_ps(speedy, dt);
                
        _mm256_storeu_ps(&repository.nx_[i], _mm256_add_ps(speedx, x));
        _mm256_storeu_ps(&repository.ny_[i], _mm256_add_ps(speedy, y));
    }
}

void SIMDMover::UpdatePositions(SOARepository& repository)
{
    int n = static_cast<int>(repository.size_);
#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        

        repository.px_[i] = repository.x_[i];
        repository.py_[i] = repository.y_[i];

        repository.x_[i] = repository.nx_[i];
        repository.y_[i] = repository.ny_[i];
    }
}
