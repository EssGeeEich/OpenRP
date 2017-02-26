#version 440
in vec2 location;
out vec4 colorOut;

layout (binding = 0) uniform sampler2D diffuse;

void main()
{
    colorOut = texture(diffuse, location);
}
