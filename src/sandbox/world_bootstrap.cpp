#include "sandbox/world_bootstrap.h"

#include <cmath>

#include "core/config.h"
#include "engine/voxel_engine.h"
#include "world/perlin_noise.h"

namespace {

inline constexpr int   PROCEDURAL_OCTAVES = 2;
inline constexpr float PROCEDURAL_FREQUENCY = 0.0054f;
inline constexpr float PROCEDURAL_AMPLITUDE = 128.f;
inline constexpr float PROCEDURAL_PERSISTENCE = 0.1f;
inline constexpr float PROCEDURAL_MULT_FREQUENCY = 12.f;

float getTerrainHeight(int p_world_x, int p_world_z) {
    static PerlinNoise2D perlin_noise;

    float total = 0.0f;
    for (int i = 0; i < PROCEDURAL_OCTAVES; i++) {
        const float frequency = PROCEDURAL_FREQUENCY * static_cast<float>(std::pow(PROCEDURAL_MULT_FREQUENCY, i));
        const float amplitude = PROCEDURAL_AMPLITUDE * static_cast<float>(std::pow(PROCEDURAL_PERSISTENCE, i));
        total += perlin_noise.noise_2d(p_world_x * frequency, p_world_z * frequency) * amplitude;
    }

    return total;
}

uint16_t getVoxelIdAtWorldPosition(int p_world_y, int p_height) {
    if (p_height <= 50) {
        if (p_world_y < p_height - 5) return 3;
        if (p_world_y < p_height + 1) return 9;
        if (p_world_y < 48) return 8;
        return 0;
    }

    if (p_world_y < p_height - 2) return 3;
    if (p_world_y < p_height) return 2;
    if (p_world_y == p_height) return 1;
    return 0;
}

} // namespace

namespace SandboxWorldBootstrap {

bool generateProceduralChunk(VoxelEngine& p_engine, glm::ivec3 p_chunk_pos) {
    bool created_chunk = false;

    for (int vx = 0; vx < CHUNK_SIZE; ++vx) {
        for (int vz = 0; vz < CHUNK_SIZE; ++vz) {
            const int world_x = p_chunk_pos.x * CHUNK_SIZE + vx;
            const int world_z = p_chunk_pos.z * CHUNK_SIZE + vz;
            const int height = static_cast<int>(getTerrainHeight(world_x, world_z));

            for (int vy = 0; vy < CHUNK_SIZE; ++vy) {
                const int world_y = p_chunk_pos.y * CHUNK_SIZE + vy;
                const uint16_t voxel_id = getVoxelIdAtWorldPosition(world_y, height);
                if (voxel_id == 0) continue;

                if (!created_chunk) {
                    p_engine.createChunk(p_chunk_pos);
                    created_chunk = true;
                }

                p_engine.setVoxel(p_chunk_pos, {vx, vy, vz}, voxel_id);
            }
        }
    }

    return created_chunk;
}

void fillTestChunk(VoxelEngine& p_engine, glm::ivec3 p_chunk_pos) {
    p_engine.createChunk(p_chunk_pos);

    for (int x = 1; x < CHUNK_SIZE - 1; x++) {
        for (int y = 1; y < CHUNK_SIZE - 1; y++) {
            for (int z = 1; z < CHUNK_SIZE - 1; z++) {
                p_engine.setVoxel(p_chunk_pos, {x, y, z}, 6);
            }
        }
    }
}

} // namespace SandboxWorldBootstra
