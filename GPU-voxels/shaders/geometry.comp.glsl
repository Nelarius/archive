#version 440 core

/*
 The purpose of this shader is to build up the voxel geometry
 into a big buffer, which can then be rendered.
*/

layout(local_size_x = 4, local_size_y = 4, local_size_z = 4) in;

layout(binding = 0, offset = 0) uniform atomic_uint arrayCounter;
layout(r8ui,    binding = 1)    uniform uimage3D voxelTexture;
layout(rgba32f, binding = 2)    uniform imageBuffer geometryTexture;

layout(location = 3) uniform vec3 boundsMin;
layout(location = 4) uniform vec3 boundsMax;
layout(location = 5) uniform int dim;

void main() {
    uint voxel = imageLoad(voxelTexture, ivec3(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, gl_GlobalInvocationID.z)).x;
    float pixelVal = clamp(float(voxel), 0.0, 1.0);
    if (voxel == 1u) {
        uint index = atomicCounterIncrement(arrayCounter);
        vec4 pos = vec4(boundsMin + (boundsMax - boundsMin) * vec3(gl_GlobalInvocationID) / float(dim), 1.0);
        imageStore(geometryTexture, int(index), pos);
    }
    // uint index = atomicCounterIncrement(arrayCounter);
    // vec4 pos = vec4(boundsMin + (boundsMax - boundsMin) * vec3(gl_GlobalInvocationID) / float(dim), 1.0);
    // imageStore(geometryTexture, int(index), pos);
}
