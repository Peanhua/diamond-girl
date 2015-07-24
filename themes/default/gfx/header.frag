#version 110

uniform sampler2D tex;
uniform gl_MaterialParameters gl_FrontMaterial;
varying vec4 gl_Color;

varying vec4 ld_diffuse, ld_ambient;
varying vec4 lp_diffuse, lp_ambient;
varying vec4 ambientGlobal;
varying vec4 ecPos;
varying vec3 normal, halfVector;


void main()
{
  vec4 colour = gl_Color;

