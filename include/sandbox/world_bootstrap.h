#ifndef SANDBOX_WORLD_BOOTSTRAP_H
#define SANDBOX_WORLD_BOOTSTRAP_H

#include <vector>

#include <glm/glm.hpp>

class VoxelEngine;

namespace SandboxWorldBootstrap {

bool generateProceduralChunk(VoxelEngine& p_engine, glm::ivec3 p_chunk_pos);
void generateProceduralColumn(VoxelEngine& p_engine, glm::ivec2 p_chunk_column_pos, std::vector<glm::ivec3>& p_created_chunks);
void fillTestChunk(VoxelEngine& p_engine, glm::ivec3 p_chunk_pos);

} // namespace SandboxWorldBootstrap

#endif
