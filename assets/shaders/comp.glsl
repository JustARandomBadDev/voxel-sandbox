#version 450

layout(push_constant) uniform PushConstants {
    uint updateVoxelsSize;
} pc;

struct Vertex {
    vec3 pos;
    vec3 color;
    vec2 texCoord;
};

struct BlockUpdate {
    ivec4 chunkPos;
    uvec4 blockPos;
    uvec4 blockID;
};

layout(std430, binding = 0) buffer BlockBuffer {
    uint voxelData[];
};

layout(std430, binding = 1) readonly buffer UpdateBuffer {
    BlockUpdate updates[];
};

layout(std430, binding = 2) buffer VertexBuffer {
    Vertex vertexData[];
};

layout(std430, binding = 3) buffer IndexBuffer {
    uint indexData[];
};

void main() {
    uint index = gl_GlobalInvocationID.x;

    uvec3 pos = updates[index].blockPos.xyz;

    vertexData[index*4+0].pos = vec3(pos.x, pos.y+1.0f, pos.z);
    vertexData[index*4+0].color = vec3(0.9f, 0.9f, 0.9f);
    vertexData[index*4+0].texCoord = vec2(0.0f, 0.0f);

    vertexData[index*4+1].pos = vec3(pos.x+1.0f, pos.y+1.0f, pos.z);
    vertexData[index*4+1].color = vec3(0.9f, 0.9f, 0.9f);
    vertexData[index*4+1].texCoord = vec2(1.0f/16, 0.0f);

    vertexData[index*4+2].pos = vec3(pos.x+1.0f, pos.y+1.0f, pos.z+1.0f);
    vertexData[index*4+2].color = vec3(0.9f, 0.9f, 0.9f);
    vertexData[index*4+2].texCoord = vec2(1.0f/16, 1.0f/16);

    vertexData[index*4+3].pos = vec3(pos.x, pos.y+1.0f, pos.z+1.0f);
    vertexData[index*4+3].color = vec3(0.9f, 0.9f, 0.9f);
    vertexData[index*4+3].texCoord = vec2(0.0f, 1.0f/16);

    indexData[index*6+0] = index*4+0;
    indexData[index*6+1] = index*4+1;
    indexData[index*6+2] = index*4+2;
    indexData[index*6+3] = index*4+2;
    indexData[index*6+4] = index*4+3;
    indexData[index*6+5] = index*4+0;
}
