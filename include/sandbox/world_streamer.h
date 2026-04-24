#ifndef SANDBOX_WORLD_STREAMER_H
#define SANDBOX_WORLD_STREAMER_H

#include <unordered_map>

#include <glm/glm.hpp>

#include "core/chunk_key.h"
#include "sandbox/sandbox_config.h"

class Camera;
class VoxelEngine;

class SandboxWorldStreamer {
public:
    explicit SandboxWorldStreamer(SandboxConfig p_config = {});

    void syncInitial(VoxelEngine& p_engine, const Camera& p_camera);
    void update(VoxelEngine& p_engine, const Camera& p_camera);

    const SandboxConfig& getConfig() const { return _config; }

private:
    struct ActiveChunkState {
        bool createdInEngine = false;
    };

    SandboxConfig _config;
    std::unordered_map<ChunkKey, ActiveChunkState, ChunkKeyHash> _active_chunks;
    glm::ivec3 _current_center_chunk = {0, 0, 0};
    bool _has_center_chunk = false;

    glm::ivec3 worldToChunk(glm::vec3 p_world_pos) const;
    void syncStaticWorld(VoxelEngine& p_engine, glm::ivec3 p_center_chunk);
    void syncToCenterChunk(VoxelEngine& p_engine, glm::ivec3 p_center_chunk);
    bool shouldKeepChunk(glm::ivec3 p_chunk_pos, glm::ivec3 p_center_chunk) const;
};

#endif
