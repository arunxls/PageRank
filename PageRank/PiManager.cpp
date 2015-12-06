#include "stdafx.h"
#include "PiManager.h"
#include <iostream>
#include <algorithm>

PiManager::PiManager()
{
}

PiManager::PiManager(uint32 ways, uint32 size)
{
    this->ways = ways;
    this->way = new float*[ways];
    this->state = 0;
    this->size = size;

    for (uint32 i = 0; i < ways; ++i) 
    {
        *(this->way + i) = new float[size];
    }
}

PiManager::~PiManager()
{
    for (int i = 0; i < this->ways; ++i)
    {
        delete[] *(this->way + i);
    }

    delete[] this->way;
}

void PiManager::reset()
{
    memset(this->way[this->current()], 0.0, sizeof(float) * this->size);
}

void PiManager::print()
{
    double X = 0.0;
    for (int i = 0; i < this->size; ++i)
    {
        X += this->way[this->current()][i];
    }

    printf("Current pi sum = %.3f\n", X);
}

void PiManager::normalize()
{
    double X = 0.0;
    double Y = 1.0;
    for (int i = 0; i < TOTAL_NODES_WITH_OUT; ++i)
    {
        X += this->way[this->prev()][i];
    }

    Y = 1.0 - X;

    X *= (1.0 - ALPHA) /(float)TOTAL_NODES;
    Y *= 1.0 /(float) TOTAL_NODES;

    for (int i = 0; i < TOTAL_NODES_WITH_OUT; ++i)
    {
        this->way[this->current()][i] = (ALPHA * this->way[this->current()][i]) + X + Y;
    }
}

void PiManager::getTopN()
{
    typedef std::pair<float, uint32_t> hash_pair_t;

    const uint8_t rank_vec_sz = TOPN + 1;
    hash_pair_t rank_vec[rank_vec_sz];
    memset(rank_vec, 0, sizeof(rank_vec));

    for (int i = 0; i < this->size; ++i) 
    {
        if (this->way[this->current()][i] > 0) 
        {
            if (this->way[this->current()][i] > rank_vec[0].first || this->way[this->current()][i] > rank_vec[rank_vec_sz -2].first)
            {
                rank_vec[10] = { this->way[this->current()][i], i };
                std::push_heap(rank_vec, rank_vec + rank_vec_sz, [](hash_pair_t& a, hash_pair_t& b)
                {
                    return a.first > b.first;
                });
                std::pop_heap(rank_vec, rank_vec + rank_vec_sz, [](hash_pair_t& a, hash_pair_t& b)
                {
                    return a.first > b.first;
                });
            }
        }
    }

    std::sort(rank_vec, rank_vec + rank_vec_sz, [](hash_pair_t& a, hash_pair_t& b) 
    {
        return a.first > b.first;
    });

    for (int i = 0; i < rank_vec_sz - 1; ++i) {
        std::cout << rank_vec[i].first << " -> " << rank_vec[i].second << std::endl;
    }
}

void PiManager::resizeCurrent()
{
    delete[] * (this->way + (this->current()));
    *(this->way + (this->current())) = new float[TOTAL_NODES];
    this->size = TOTAL_NODES;
}
