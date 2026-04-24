#include "sandbox/world_streamer.h"

#include <cmath>
#include <vector>

#include "core/config.h"
#include "core/camera.h"
#include "sandbox/world_bootstrap.h"
#include "engine/voxel_engine.h"

namespace {

int floorDivToChunk(float p_world_coordinate) {
    return static_cast<int>(std::floor(p_world_coordinate / static_cast<float>(CHUNK_SIZE)));
} 

} // namespace

SandboxWorldStreamer::SandboxWorldStreamer(SandboxConfig p_config)
: _config(p_config) {}

void SandboxWorldStreamer::syncInitial(VoxelEngine& p_engine, const Camera& p_camera) {
    _active_chunks.clear();
    _has_center_chunk = false;

    const glm::ivec3 center_chunk = worldToChunk(p_camera.getPosition());
    if (_config.generationMode == SandboxGenerationMode::Static) {
        syncStaticWorld(p_engine, center_chunk);
        return;
    }

    syncToCenterChunk(p_engine, center_chunk);
}

void SandboxWorldStreamer::update(VoxelEngine& p_engine, const Camera& p_camera) {
    if (_config.generationMode == SandboxGenerationMode::Static) {
        return;
    }

    const glm::ivec3 center_chunk = worldToChunk(p_camera.getPosition());
    if (_has_center_chunk && center_chunk == _current_center_chunk) return;

    syncToCenterChunk(p_engine, center_chunk);
}

glm::ivec3 SandboxWorldStreamer::worldToChunk(glm::vec3 p_world_pos) const {
    return {
        floorDivToChunk(p_world_pos.x),
        floorDivToChunk(p_world_pos.y),
        floorDivToChunk(p_world_pos.z)
    };
}

bool SandboxWorldStreamer::shouldKeepChunk(glm::ivec3 p_chunk_pos, glm::ivec3 p_center_chunk) const {
    return std::abs(p_chunk_pos.x - p_center_chunk.x) <= _config.dynamicUnloadDistanceHorizontal &&
           std::abs(p_chunk_pos.y - p_center_chunk.y) <= _config.dynamicUnloadDistanceVertical &&
           std::abs(p_chunk_pos.z - p_center_chunk.z) <= _config.dynamicUnloadDistanceHorizontal;
}

void SandboxWorldStreamer::syncStaticWorld(VoxelEngine& p_engine, glm::ivec3 p_center_chunk) {
    const int static_size = _config.staticSize;
    _active_chunks.reserve(
        static_cast<size_t>((static_size * 2 + 1) *
                            (static_size * 2 + 1) *
                            (static_size * 2 + 1))
    );

    for (int y = p_center_chunk.y - static_size; y <= p_center_chunk.y + static_size; ++y) {
        for (int x = p_center_chunk.x - static_size; x <= p_center_chunk.x + static_size; ++x) {
            for (int z = p_center_chunk.z - static_size; z <= p_center_chunk.z + static_size; ++z) {
                const glm::ivec3 chunk_pos = {x, y, z};
                ActiveChunkState state;
                state.createdInEngine = SandboxWorldBootstrap::generateProceduralChunk(p_engine, chunk_pos);
                _active_chunks.emplace(makeChunkKey(chunk_pos), state);
            }
        }
    }

    _current_center_chunk = p_center_chunk;
    _has_center_chunk = true;
}

void SandboxWorldStreamer::syncToCenterChunk(VoxelEngine& p_engine, glm::ivec3 p_center_chunk) {
    std::vector<glm::ivec3> chunks_to_add;
    chunks_to_add.reserve(
        static_cast<size_t>((_config.dynamicSizeHorizontal * 2 + 1) *
                            (_config.dynamicSizeVertical * 2 + 1) *
                            (_config.dynamicSizeHorizontal * 2 + 1))
    );

    for (int y = p_center_chunk.y - _config.dynamicSizeVertical; y <= p_center_chunk.y + _config.dynamicSizeVertical; ++y) {
        for (int x = p_center_chunk.x - _config.dynamicSizeHorizontal; x <= p_center_chunk.x + _config.dynamicSizeHorizontal; ++x) {
            for (int z = p_center_chunk.z - _config.dynamicSizeHorizontal; z <= p_center_chunk.z + _config.dynamicSizeHorizontal; ++z) {
                const glm::ivec3 chunk_pos = {x, y, z};
                const ChunkKey key = makeChunkKey(chunk_pos);
                if (_active_chunks.contains(key)) continue;
                chunks_to_add.push_back(chunk_pos);
            }
        }
    }

    for (const glm::ivec3& chunk_pos : chunks_to_add) {
        ActiveChunkState state;
        state.createdInEngine = SandboxWorldBootstrap::generateProceduralChunk(p_engine, chunk_pos);
        _active_chunks.emplace(makeChunkKey(chunk_pos), state);
    }

    std::vector<glm::ivec3> chunks_to_remove;
    chunks_to_remove.reserve(_active_chunks.size());

    for (const auto& [key, state] : _active_chunks) {
        const glm::ivec3 chunk_pos = {key.x, key.y, key.z};
        if (!shouldKeepChunk(chunk_pos, p_center_chunk)) {
            chunks_to_remove.push_back(chunk_pos);
        }
    }

    for (const glm::ivec3& chunk_pos : chunks_to_remove) {
        const auto it = _active_chunks.find(makeChunkKey(chunk_pos));
        if (it != _active_chunks.end() && it->second.createdInEngine) {
            p_engine.removeChunk(chunk_pos);
        }
        _active_chunks.erase(makeChunkKey(chunk_pos));
    }

    _current_center_chunk = p_center_chunk;
    _has_center_chunk = true;
}
