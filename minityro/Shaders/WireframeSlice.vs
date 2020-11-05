#version 400

in vec3 aPosition;
in vec3 aColor;

uniform mat4 uMVPMatrix;
out vec4 vColor;

uniform vec3 planepoint;
out float vClip;

void main()
{
    gl_Position = uMVPMatrix * vec4(aPosition,1.0);
    vColor.rgb = aColor;
    vColor.a = 1.0;

    if (aPosition[0]>planepoint[0])
        vClip = -100;
    else if (aPosition[1]>planepoint[1])
        vClip = -100;
    else if (aPosition[2]>planepoint[2])
        vClip = -100;
    else
        vClip = 1;

}


