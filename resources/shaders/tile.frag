#version 300 es
precision mediump float;
in vec3 frag_colour;
out vec4 out_v4_colour;
void main()
{
    out_v4_colour = vec4(frag_colour, 1.0);
}
