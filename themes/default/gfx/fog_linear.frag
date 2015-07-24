  {
    float fog;

    fog = (gl_Fog.end - gl_FogFragCoord) * gl_Fog.scale;
    fog = clamp(fog, 0.0, 1.0);
    colour = vec4(mix(vec3(gl_Fog.color), vec3(colour), fog), colour.a);
  }
