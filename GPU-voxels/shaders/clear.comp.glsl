# version 430 core

layout(local_size_x = 4, local_size_y = 4, local_size_z = 4) in;
layout(r8ui, binding = 0) uniform uimage3D voxelTexture;

void main() {
    imageStore(voxelTexture, ivec3(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, gl_GlobalInvocationID.z), uvec4(0, 0, 0, 0));
}
