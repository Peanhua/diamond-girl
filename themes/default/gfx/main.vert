#version 110
attribute vec4 gl_Color;
varying vec4 gl_FrontColor;

varying vec4 ld_diffuse, ld_ambient, lp_diffuse, lp_ambient, ambientGlobal, ecPos;
varying vec3 normal, halfVector;

void main()
{
  gl_FrontColor = gl_Color;
  gl_TexCoord[0] = /* gl_TextureMatrix[0] * */ gl_MultiTexCoord0;

  /* first transform the normal into eye space and
     normalize the result */
  normal = normalize(gl_NormalMatrix * gl_Normal);

  /* compute the vertex position  in camera space. */
  ecPos = gl_ModelViewMatrix * gl_Vertex;
 
  /* pass the halfVector to the fragment shader */
  halfVector = gl_LightSource[0].halfVector.xyz;
 
  /* Compute the diffuse, ambient and globalAmbient terms */
  ld_diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
  ld_ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
  lp_diffuse = gl_FrontMaterial.diffuse * gl_LightSource[1].diffuse;
  lp_ambient = gl_FrontMaterial.ambient * gl_LightSource[1].ambient;
  ambientGlobal = gl_LightModel.ambient * gl_FrontMaterial.ambient;

  gl_FogFragCoord = abs((gl_ModelViewMatrix * gl_Vertex).z);

  gl_Position = ftransform();
}
