#version 400


in vec3 aPosition; //local space
in vec3 aNormal; //local space

uniform mat4 uMVPMatrix;
// uniform mat4 uMVMatrix;
uniform mat3 uNMatrix;

out vec3 normal_eye;

void main() 
{
    // vec3 e = normalize( vec3( uMVMatrix * vec4( aPosition, 1.0 ) ) );
    // vec3 n = normalize( uNMatrix * aNormal );

    // vec3 r = reflect( e, n );
    // float m = 2.0 * sqrt( pow( r.x, 2.0 ) + pow( r.y, 2.0 ) + pow( r.z + 1.0, 2.0 ) );
    // vN = r.xy / m + 0.5;

    // gl_Position = uMVPMatrix * vec4( aPosition, 1.0 );
    normal_eye = normalize(uNMatrix * aNormal);
	gl_Position = uMVPMatrix * vec4(aPosition, 1.0);
}


/*
in vec3 aPosition; //local space
in vec3 aNormal; //local space

uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat3 uNMatrix;

out vec3 e;
out vec3 n;

void main() 
{
    e = normalize( vec3( uMVMatrix * vec4( aPosition, 1.0 ) ) );
    n = normalize( uNMatrix * aNormal );
    gl_Position = uMVPMatrix * vec4( aPosition, 1. );
}*/