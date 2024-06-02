vert = [[
#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 cd;

layout(location=1) uniform mat4 m_matrix;
layout(location=2) uniform mat4 vrp_matrix;

out vec2 coord;
out flat int depth;

void main(){
    vec3 p = pos;
    p += 3 * vec3(int(gl_InstanceID / 16),gl_InstanceID % 16,0);
    gl_Position = vrp_matrix * m_matrix * vec4(p,1.0);
    coord = cd;
    depth = gl_InstanceID;
}
]]

frag = [[
#version 430
#extension GL_EXT_gpu_shader4 : enable


layout(binding = 0) uniform sampler2DArray tex;

out vec4 color;
in vec2 coord;
in flat int depth;

void main(){
    vec4 pick = texture2DArray(tex,vec3(coord,depth));
    color = pick;
}
]]