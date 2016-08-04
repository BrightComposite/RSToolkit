/**
 *	!vertex: p3
 *	!instance: t
 */
#version 330 core

in vec3 position;
in vec4 transform;

void main(void)
{
    gl_Position = vec4(position * transform.w + transform.xyz, 1.0);
}
