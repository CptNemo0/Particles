#include "Mover.h"
#include <immintrin.h>
#include <omp.h>
#include "pch.h"
#include <iostream>


void NormalMover::Init()
{

}

void NormalMover::UpdateVelocities()
{
    int n = static_cast<int>(repository_->size_);

#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        repository_->speedx_[i] += (1.0f  /240.0f) * repository_->forcex_[i] * repository_->inv_mass[i];
        repository_->speedy_[i] += (1.0f / 240.0f) * (-100.0f) * repository_->inv_mass[i];
        repository_->speedz_[i] += (1.0f / 240.0f) * repository_->forcez_[i] * repository_->inv_mass[i];
    }

}

void NormalMover::PredictPositions()
{
    int n = static_cast<int>(repository_->size_);
#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        repository_->nx_[i] = repository_->x_[i] + repository_->speedx_[i] * FIXED_DT;
        repository_->ny_[i] = repository_->y_[i] + repository_->speedy_[i] * FIXED_DT;
        repository_->nz_[i] = repository_->z_[i] + repository_->speedz_[i] * FIXED_DT;
    }
    
}

void NormalMover::UpdatePositions()
{
    int n = static_cast<int>(repository_->size_);
#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        repository_->speedx_[i] = (repository_->nx_[i] - repository_->x_[i]) * 240.0f;
        repository_->speedy_[i] = (repository_->ny_[i] - repository_->y_[i]) * 240.0f;
        repository_->speedz_[i] = (repository_->nz_[i] - repository_->z_[i]) * 240.0f;

        repository_->px_[i] = repository_->x_[i];
        repository_->py_[i] = repository_->y_[i];
        repository_->pz_[i] = repository_->z_[i];

        repository_->x_[i] = repository_->nx_[i];
        repository_->y_[i] = repository_->ny_[i];
        repository_->z_[i] = repository_->nz_[i];

        repository_->output_position_[i * 3 + 0] = repository_->x_[i];
        repository_->output_position_[i * 3 + 1] = repository_->y_[i];
        repository_->output_position_[i * 3 + 2] = repository_->z_[i];
    }
}


void SIMDMover::Init()
{
    
}

void SIMDMover::UpdateVelocities()
{
    int n = static_cast<int>(repository_->size_);
    float delta_time = FIXED_DT;

#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        __m256 dt     = _mm256_set1_ps(1.0f / 240.0f);

        __m256 speedx = _mm256_load_ps(&(repository_->speedx_[i]));
        __m256 speedy = _mm256_load_ps(&(repository_->speedy_[i]));
        __m256 speedz = _mm256_load_ps(&(repository_->speedz_[i]));

        __m256 forcex = _mm256_load_ps(&(repository_->forcex_[i]));
        __m256 forcey = _mm256_load_ps(&(repository_->forcey_[i]));
        __m256 forcez = _mm256_load_ps(&(repository_->forcez_[i]));

        __m256 inv_w = _mm256_load_ps(&(repository_->inv_mass[i]));

        speedx = _mm256_add_ps(speedx, _mm256_mul_ps(_mm256_mul_ps(forcex, inv_w), dt));
        speedy = _mm256_add_ps(speedy, _mm256_mul_ps(_mm256_mul_ps(forcey, inv_w), dt));
        speedz = _mm256_add_ps(speedz, _mm256_mul_ps(_mm256_mul_ps(forcez, inv_w), dt));

        _mm256_store_ps(&repository_->speedx_[i], speedx);
        _mm256_store_ps(&repository_->speedy_[i], speedy);
        _mm256_store_ps(&repository_->speedz_[i], speedz);
    }
}

void SIMDMover::PredictPositions()
{
    int n = static_cast<int>(repository_->size_);
    int step = SIMD_BLOCK_SIZE;
#pragma omp parallel for
    for (int i = 0; i < n; i += step)
    {
        __m256 dt     = _mm256_set1_ps(1.0f / 240.0f);

        __m256 x      = _mm256_load_ps(&repository_->x_[i]);
        __m256 y      = _mm256_load_ps(&repository_->y_[i]);
        __m256 z      = _mm256_load_ps(&repository_->z_[i]);

        __m256 speedx = _mm256_load_ps(&repository_->speedx_[i]);
        __m256 speedy = _mm256_load_ps(&repository_->speedy_[i]);
        __m256 speedz = _mm256_load_ps(&repository_->speedz_[i]);

        speedx = _mm256_mul_ps(speedx, dt);
        speedy = _mm256_mul_ps(speedy, dt);
        speedz = _mm256_mul_ps(speedz, dt);

        _mm256_store_ps(&repository_->nx_[i], _mm256_add_ps(speedx, x));
        _mm256_store_ps(&repository_->ny_[i], _mm256_add_ps(speedy, y));
        _mm256_store_ps(&repository_->nz_[i], _mm256_add_ps(speedz, z));
    }
}

void SIMDMover::UpdatePositions()
{
    int n = static_cast<int>(repository_->size_);
#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        repository_->speedx_[i] = (repository_->nx_[i] - repository_->x_[i]) * 240.0f;
        repository_->speedy_[i] = (repository_->ny_[i] - repository_->y_[i]) * 240.0f;
        repository_->speedz_[i] = (repository_->nz_[i] - repository_->z_[i]) * 240.0f;

        repository_->px_[i] = repository_->x_[i];
        repository_->py_[i] = repository_->y_[i];
        repository_->pz_[i] = repository_->z_[i];

        repository_->x_[i] = repository_->nx_[i];
        repository_->y_[i] = repository_->ny_[i];
        repository_->z_[i] = repository_->nz_[i];

        repository_->output_position_[i * 3 + 0] = repository_->x_[i];
        repository_->output_position_[i * 3 + 1] = repository_->y_[i];
        repository_->output_position_[i * 3 + 2] = repository_->z_[i];
    }
}