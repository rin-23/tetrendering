#version 400

in vec3 aPosition; //local space
in vec3 aNormal; //local space
in vec3 aColor;
uniform vec3 planepoint;

uniform mat4 uMVPMatrix;

out vec3 vColor;
out vec3 vNormal; //local space
out float vClip;

void main()
{   
    vColor = aColor;
    vNormal = aNormal; //local space
    gl_Position = uMVPMatrix * vec4(aPosition,1.0);
    
    if (aPosition[0]>planepoint[0])
        vClip = -100;
    else if (aPosition[1]>planepoint[1])
        vClip = -100;
    else if (aPosition[2]>planepoint[2])
        vClip = -100;
    else
        vClip = 1;
}
