#version 400

in vec3 aPosition; //local space
in vec3 aNormal; //local space
in vec3 aColor;

uniform mat4 uMVPMatrix;

out vec3 vColor;
out vec3 vNormal; //local space

void main()
{   
    vColor = aColor;
    vNormal = aNormal; //local space
    gl_Position = uMVPMatrix * vec4(aPosition,1.0);
}
