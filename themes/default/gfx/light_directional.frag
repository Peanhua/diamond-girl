  {
    vec3 lightDir = vec3(gl_LightSource[0].position);
    vec4 light_colour = ld_ambient + ambientGlobal;
    vec3 n = normalize(normal);
    float NdotL = max(dot(n, lightDir), 0.0);
    if(NdotL > 0.0)
      {
        light_colour += ld_diffuse * NdotL;
        vec3 halfV = normalize(halfVector);
        float NdotHV = max(dot(n, halfV), 0.0);
        light_colour +=
          gl_FrontMaterial.specular *
          gl_LightSource[0].specular *
          pow(NdotHV, gl_FrontMaterial.shininess);
      }
  
    colour *= light_colour;
  }
