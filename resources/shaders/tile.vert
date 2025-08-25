#version 300 es
precision mediump float;
layout(location = 0) in vec2 in_v2_pos;
layout(location = 1) in vec3 in_v3_colour;
uniform mat4 u_m4_transform;
out vec3 frag_colour;
void main()
{
    gl_Position = u_m4_transform * vec4(in_v2_pos, 0.0, 1.0);
    frag_colour = in_v3_colour;
}

