#include "sandbox/world_bootstrap.h"

#include <algorithm>
#include <array>
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
inline constexpr int   WATER_SURFACE_Y = 47;

int floorDivToChunk(int p_world_coordinate) {
    int result = p_world_coordinate / CHUNK_SIZE;
    if (p_world_coordinate < 0 && p_world_coordinate % CHUNK_SIZE != 0) {
        --result;
    }
    return result;
}

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

void generateProceduralColumn(VoxelEngine& p_engine, glm::ivec2 p_chunk_column_pos, std::vector<glm::ivec3>& p_created_chunks) {
    std::array<int, CHUNK_SIZE * CHUNK_SIZE> terrain_heights;
    int max_world_y = -1;

    for (int vx = 0; vx < CHUNK_SIZE; ++vx) {
        for (int vz = 0; vz < CHUNK_SIZE; ++vz) {
            const int world_x = p_chunk_column_pos.x * CHUNK_SIZE + vx;
            const int world_z = p_chunk_column_pos.y * CHUNK_SIZE + vz;
            const int height = static_cast<int>(getTerrainHeight(world_x, world_z));
            terrain_heights[static_cast<size_t>(vx * CHUNK_SIZE + vz)] = height;
            max_world_y = std::max(max_world_y, height <= 50 ? std::max(height, WATER_SURFACE_Y) : height);
        }
    }

    if (max_world_y < 0) return;

    const int max_chunk_y = floorDivToChunk(max_world_y);

    for (int chunk_y = 0; chunk_y <= max_chunk_y; ++chunk_y) {
        const glm::ivec3 chunk_pos = {p_chunk_column_pos.x, chunk_y, p_chunk_column_pos.y};
        const int chunk_min_world_y = chunk_y * CHUNK_SIZE;
        bool created_chunk = false;

        for (int vx = 0; vx < CHUNK_SIZE; ++vx) {
            for (int vz = 0; vz < CHUNK_SIZE; ++vz) {
                const int height = terrain_heights[static_cast<size_t>(vx * CHUNK_SIZE + vz)];
                const int voxel_max_world_y = std::min(
                    chunk_min_world_y + CHUNK_SIZE - 1,
                    height <= 50 ? std::max(height, WATER_SURFACE_Y) : height
                );
                if (voxel_max_world_y < chunk_min_world_y) continue;

                for (int world_y = chunk_min_world_y; world_y <= voxel_max_world_y; ++world_y) {
                    const uint16_t voxel_id = getVoxelIdAtWorldPosition(world_y, height);
                    if (voxel_id == 0) continue;

                    if (!created_chunk) {
                        p_engine.createChunk(chunk_pos);
                        p_created_chunks.push_back(chunk_pos);
                        created_chunk = true;
                    }

                    p_engine.setVoxel(chunk_pos, {vx, world_y - chunk_min_world_y, vz}, voxel_id);
                }
            }
        }
    }
}

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
