# version 440 core

layout(location = 1) uniform int dim;    // the number of voxels in the x, y, z dimensions
layout(location = 2) uniform vec3 boundsMin;   // the extent of the bounding volume
layout(location = 3) uniform vec3 boundsMax;
layout(binding = 1, r8ui)       uniform uimage3D voxelTexture;
layout(binding = 0, offset = 0) uniform atomic_uint arrayCounter;
in vec4 fragPos;

void main() {
    ivec3 coord = ivec3(((fragPos.xyz - boundsMin) * dim) / (boundsMax - boundsMin));
    uvec4 data = uvec4(1, 1, 1, 1);
    imageStore(voxelTexture, coord, data);
    atomicCounterIncrement(arrayCounter);
}
