  {
    vec4 light_color = ambientGlobal;
    
    vec3 n = normalize(normal);
    vec3 lightDir = vec3(gl_LightSource[1].position - ecPos);
    float dist = length(lightDir);
    float NdotL = max(dot(n, normalize(lightDir)), 0.0);
 
    if(NdotL > 0.0)
      {
        float att = 1.0 / (gl_LightSource[1].constantAttenuation +
                           gl_LightSource[1].linearAttenuation * dist +
                           gl_LightSource[1].quadraticAttenuation * dist * dist);
        light_color += att * (lp_diffuse * NdotL + lp_ambient);
     
        vec3 halfV = normalize(halfVector);
        float NdotHV = max(dot(n, halfV), 0.0);
        light_color += att * gl_FrontMaterial.specular * gl_LightSource[1].specular * pow(NdotHV, gl_FrontMaterial.shininess);
      }
