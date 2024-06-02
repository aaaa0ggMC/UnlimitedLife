#ifndef GPUINFO_H_INCLUDED
#define GPUINFO_H_INCLUDED
#include <string>
#include <vector>

struct GPUInfo {
    std::string vendor;
    std::string renderer;
    std::string version;
    std::string glslVersion;
    int maxTextureSize;
    int maxVertexAttribs;
    float maxAnisotropy;
    int maxViewportDims[2];
    std::vector<std::string> extensions;
};

#endif // GPUINFO_H_INCLUDED
