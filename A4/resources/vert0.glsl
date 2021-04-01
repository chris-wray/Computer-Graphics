#version 120

uniform mat4 P;
uniform mat4 MV;
uniform mat4 ITMV;

attribute vec4 aPos; // in object space
attribute vec3 aNor; // in object space


varying vec3 normal;
varying vec3 vpos;

void main()
{
	normal = vec3(ITMV * vec4(aNor, 1.0));
	vpos = vec3(MV * aPos);
	gl_Position = P * MV * aPos;

}