/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#ifndef QUARTZ_SHADERS_XOROSHIRO64S_H
#define QUARTZ_SHADERS_XOROSHIRO64S_H

struct RNG
{
    uvec2 s;
};

uint rotl(uint x, uint k)
{
    return (x << k) | (x >> (32 - k));
}

// Xoroshiro64* RNG
uint rngNext(inout RNG rng)
{
    uint result = rng.s.x * 0x9e3779bb;

    rng.s.y ^= rng.s.x;
    rng.s.x = rotl(rng.s.x, 26) ^ rng.s.y ^ (rng.s.y << 9);
    rng.s.y = rotl(rng.s.y, 13);

    return result;
}

// Thomas Wang 32-bit hash.
uint rngHash(uint seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

RNG rngInit(uvec2 id, uint frameIndex)
{
    uint s0 = (id.x << 16) | id.y;
    uint s1 = frameIndex;

    RNG rng;
    rng.s.x = rngHash(s0);
    rng.s.y = rngHash(s1);
    rngNext(rng);
    return rng;
}

#endif // QUARTZ_SHADERS_XOROSHIRO64S_H