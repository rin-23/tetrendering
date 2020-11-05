#version 400

in vec4 vColor;
out vec4 fFragColor;
in float vClip;

void main()
{
    if (vClip < 0.0)
        discard;
        
    fFragColor = vColor;
}

