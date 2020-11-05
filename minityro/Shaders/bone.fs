#version 400

uniform mat3 uNMatrix;

in vec3 vColor;
in vec3 vNormal; 
out vec4 fFragColor;

const float c_zero = 0.0;
const float c_one = 1.0;
const vec3 cLightDir = vec3(c_zero, c_zero, c_one);

void main()
{   
    vec3 n = normalize(vNormal);
    
    vec3 eyeNormal = normalize(uNMatrix * n);
    float intensity = max(0.2, dot(eyeNormal, cLightDir));
    fFragColor.rgb = max(0.5 * vColor.rgb, intensity * vColor.rgb);
    fFragColor.a = 1.0;
}

