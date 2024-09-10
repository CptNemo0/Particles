#include "Mover.h"
#include <immintrin.h>
#include <omp.h>
#include "pch.h"
#include <iostream>

NormalMover::NormalMover()
{
    
}

void NormalMover::Init(const SOARepository& repository)
{

}

void NormalMover::UpdateVelocities(SOARepository& repository)
{
    int n = static_cast<int>(repository.size_);
    float delta_time = FIXED_DT;
#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        repository.speedx_[i] += FIXED_DT * repository.forcex_[i];
        repository.speedy_[i] += FIXED_DT * repository.forcey_[i];
        repository.speedz_[i] += FIXED_DT * repository.forcez_[i];
    }

}

void NormalMover::PredictPositions(SOARepository& repository)
{
    int n = static_cast<int>(repository.size_);
#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        repository.nx_[i] = repository.x_[i] + repository.speedx_[i] * FIXED_DT;
        repository.ny_[i] = repository.y_[i] + repository.speedy_[i] * FIXED_DT;
        repository.nz_[i] = repository.z_[i] + repository.speedz_[i] * FIXED_DT;
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
        repository.speedz_[i] = (repository.nz_[i] - repository.z_[i]) / FIXED_DT;

        repository.px_[i] = repository.x_[i];
        repository.py_[i] = repository.y_[i];
        repository.pz_[i] = repository.z_[i];

        repository.x_[i] = repository.nx_[i];
        repository.y_[i] = repository.ny_[i];
        repository.z_[i] = repository.nz_[i];

        repository.output_position_[i * 3 + 0] = repository.x_[i];
        repository.output_position_[i * 3 + 1] = repository.y_[i];
        repository.output_position_[i * 3 + 2] = repository.z_[i];
    }
}


SIMDMover::SIMDMover()
{
    
}

void SIMDMover::Init(const SOARepository& repository)
{
    
}

void SIMDMover::UpdateVelocities(SOARepository& repository)
{
    int n = static_cast<int>(repository.size_);
    float delta_time = FIXED_DT;
#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        const __m256 dt{ FIXED_DT, FIXED_DT, FIXED_DT, FIXED_DT, FIXED_DT, FIXED_DT, FIXED_DT, FIXED_DT };

        __m256 speedx = _mm256_load_ps(&repository.speedx_[i]);
        __m256 speedy = _mm256_load_ps(&repository.speedy_[i]);
        __m256 speedz = _mm256_load_ps(&repository.speedz_[i]);

        __m256 forcex = _mm256_load_ps(&repository.forcex_[i]);
        __m256 forcey = _mm256_load_ps(&repository.forcey_[i]);
        __m256 forcez = _mm256_load_ps(&repository.forcez_[i]);

        forcex = _mm256_mul_ps(forcex, dt);
        forcey = _mm256_mul_ps(forcey, dt);
        forcez = _mm256_mul_ps(forcez, dt);

        speedx = _mm256_add_ps(speedx, forcex);
        speedy = _mm256_add_ps(speedy, forcey);
        speedz = _mm256_add_ps(speedy, forcez);

        _mm256_storeu_ps(&repository.speedx_[i], speedx);
        _mm256_storeu_ps(&repository.speedy_[i], speedy);
        _mm256_storeu_ps(&repository.speedz_[i], speedz);
    }
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
        __m256 z = _mm256_load_ps(&repository.z_[i]);

        __m256 speedx = _mm256_load_ps(&repository.speedx_[i]);
        __m256 speedy = _mm256_load_ps(&repository.speedy_[i]);
        __m256 speedz = _mm256_load_ps(&repository.speedz_[i]);

        speedx = _mm256_mul_ps(speedx, dt);
        speedy = _mm256_mul_ps(speedy, dt);
        speedz = _mm256_mul_ps(speedz, dt);
                
        x = _mm256_add_ps(speedx, x);
        y = _mm256_add_ps(speedy, y);
        z = _mm256_add_ps(speedy, z);

        _mm256_storeu_ps(&repository.nx_[i], x);
        _mm256_storeu_ps(&repository.ny_[i], y);
        _mm256_storeu_ps(&repository.nz_[i], z);
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

        repository.output_position_[i * 3 + 0] = repository.x_[i];
        repository.output_position_[i * 3 + 1] = repository.y_[i];
        repository.output_position_[i * 3 + 2] = repository.z_[i];
    }
}