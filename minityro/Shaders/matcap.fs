#version 400


uniform sampler2D tMatCap;
in vec3 normal_eye;
out vec4 fFragColor;

void main() 
{   
//     //vec2 d_down = normalize(vec2(0.0, 1.0));
//     //float angle = dot(d_down, vN - vec2(0.5,0.5));
//     //float d = distance(vN, vec2(0.5,0.5))  ;
//     vec2 vN2 = vN;//clamp(vN, vec2(0.1,0.1), vec2(0.9,0.9));
//     vec4 base = texture( tMatCap, vN2 );
    
//     //if ( angle > 0.0 && d > 0.49) 
//    // {
//     //    base = base;// vec3(1.0, 0.0, 0.0);
//    // }

//     fFragColor = base;

    vec2 uv = normalize(normal_eye).xy * 0.5 + 0.5;
	fFragColor = texture(tMatCap, uv);
}


/*
uniform sampler2D tMatCap;
in vec3 e;
in vec3 n;
out vec4 fFragColor;

void main() 
{
    vec3 r = reflect( e, n );
    float m = 2. * sqrt( pow( r.x, 2. ) + pow( r.y, 2. ) + pow( r.z + 1., 2. ) );
    vec2 vN = r.xy / m + .5;
    vec3 base = texture2D( tMatCap, vN ).rgb;
    fFragColor = vec4( base, 1. );
}*/