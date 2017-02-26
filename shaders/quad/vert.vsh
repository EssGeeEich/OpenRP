#version 440
layout(location = 0) in vec2 position;
uniform vec2 g_offset;
uniform vec2 g_size;
out vec4 fragColor;

void main()
{
    gl_Position = vec4( -1.0 + (g_offset.x + (position.x * g_size.x)),
                         1.0 - (g_offset.y + (position.y * g_size.y)),
                        0.0,
                        1.0);
    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
