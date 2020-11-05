#version 400
in vec3 aPosition; //local space
in vec3 aNormal; //local space

uniform mat4 uMVPMatrix;

out vec3 vNormal; //local space

void main()
{   
    vNormal = aNormal; //local space
    gl_Position = uMVPMatrix * vec4(aPosition,1.0);
}
