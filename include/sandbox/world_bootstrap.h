#ifndef SANDBOX_WORLD_BOOTSTRAP_H
#define SANDBOX_WORLD_BOOTSTRAP_H

#include <glm/glm.hpp>

class VoxelEngine;

namespace SandboxWorldBootstrap {

bool generateProceduralChunk(VoxelEngine& p_engine, glm::ivec3 p_chunk_pos);
void fillTestChunk(VoxelEngine& p_engine, glm::ivec3 p_chunk_pos);

} // namespace SandboxWorldBootstrap

#endif
