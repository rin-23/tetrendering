#version 400

#define FLT_MAX 3.402823466e+30 // changed +38 to 30 to avoid overflow?
#define RUN_MAIN 1
#define RUN_BARY_UNIT_TEST 0

uniform mat4 uMVPMatrix;
// uniform mat4 uPMatrix;
uniform mat4 uMVMatrix;
uniform mat3 uNMatrix;
// uniform vec4 uViewportCustom;
 
uniform samplerBuffer uVerSampler;
uniform samplerBuffer uNormSampler;
uniform samplerBuffer uDiffusedValuesSampler;
uniform samplerBuffer uMuscleSampler;
uniform samplerBuffer uMuscleColorSampler;

// in vec4 vColor;
// in float vDiffusion;
in vec3 vPosition;
// in vec3 vNormal;
flat in int vTetId;

out vec4 fFragColor;

const float c_zero = 0.0;
const float c_one = 1.0;
const vec3 cLightDir = vec3(c_zero, c_zero, c_one);

// layout(pixel_center_integer) in vec4 gl_FragCoord;

// forward declarations
vec4 barycoord(in vec3 p3, in mat4 V);
int get_max_idx(in vec3 v);
int get_max_idx(in vec4 d);
float volume(in mat4 V);

float doublearea(in mat4 V) 
{
    vec3 AB = (V[1] - V[0]).xyz;
    vec3 AC = (V[2] - V[0]).xyz;
    return length(vec3(AB.y*AC.z-AB.z*AC.y,AB.z*AC.x-AB.x*AC.z,AB.x*AC.y-AB.y*AC.x));
}

vec3 gradient(in mat4 D,
              in mat4 V,
              in mat4 N,
              in int maxidx) 
{

    float Vol = volume(V);

    mat4 V1,V2,V3,V4;
    

    V1[0] = V[1];
    V1[1] = V[2];
    V1[2] = V[3];
    float a1 = 0.5*doublearea(V1);

    V2[0] = V[3];
    V2[1] = V[2];
    V2[2] = V[0];
    float a2 = 0.5*doublearea(V2);

    V3[0] = V[0];
    V3[1] = V[1];
    V3[2] = V[3];
    float a3 = 0.5*doublearea(V3);

    V4[0] = V[0];
    V4[1] = V[2];
    V4[2] = V[1];
    float a4 = 0.5*doublearea(V4);
    
    float Vby3 = 1.0/(Vol*3.0);
    vec3 g0 = -1*a1*Vby3*normalize(N[0].xyz); 
    vec3 g1 = -1*a2*Vby3*normalize(N[1].xyz);
    vec3 g2 = -1*a3*Vby3*normalize(N[2].xyz);
    vec3 g3 = -1*a4*Vby3*normalize(N[3].xyz);

    vec3 g = D[maxidx][0]*g0 + D[maxidx][1]*g1 + D[maxidx][2]*g2 + D[maxidx][3]*g3;
    return g;
}

// project 3d point in object space to screen space with z-depth
vec3 glslproject(in vec3  obj, in mat4 mvp, in vec4  viewport)
{
    vec4 tmp = vec4(obj,1.0);
    tmp = mvp * tmp;
    // tmp = proj * tmp;

    tmp = tmp / tmp[3];
    tmp = 0.5 * tmp  + vec4(0.5);
    tmp[0] = tmp[0] * viewport[2] + viewport[0];
    tmp[1] = tmp[1] * viewport[3] + viewport[1];

    return vec3(tmp);
}

// linearly interpolate diffusion values for point p
vec4 diffuse(in mat4 D, // 4 diffusion values at 4 corners
             in mat4 V, // 4 corners of the tet
             in vec3 p)
{
    vec4 b = barycoord(p, V); 
    //inteporalte diffusion values via bary coordinates, each column are diffusion values for the muscles 
    return b*D; 
}

// vec4 func(in mat4 D, // diffusion values at 4 corners
//           in mat4 V, // 4 corners of the tet
//           in vec3 p)
// {
//     return diffuse(D, V, p);
// }

// vec3 calcNormal(in mat4 D, // diffusion values at 4 corners
//                 in mat4 V, // 4 corners of the tet
//                 in vec3 p,
//                 in int maxidx) // for function f(p)
// {
//     const float eps = 0.000001; // TODO: THIS SHOULD BE VOLUME SIZE DEPENDENT
//     const vec2 h = vec2(eps,0);
//     return normalize(-1* vec3(func(D,V,p+h.xyy)[maxidx] - func(D,V, p-h.xyy)[maxidx],
//                               func(D,V,p+h.yxy)[maxidx] - func(D,V, p-h.yxy)[maxidx],
//                               func(D,V,p+h.yyx)[maxidx] - func(D,V, p-h.yyx)[maxidx]));
// }

// vec3 calcNormal2( in vec3 v0, in vec3 p, in float maxlength ) // for function f(p)
// {
//     const float eps = 0.0001; // or some other value
//     const vec2 h = vec2(eps,0);
//     return normalize( vec3(func2(v0,p+h.xyy,maxlength) - func2(v0, p-h.xyy,maxlength),
//                            func2(v0,p+h.yxy,maxlength) - func2(v0, p-h.yxy,maxlength),
//                            func2(v0,p+h.yyx,maxlength) - func2(v0, p-h.yyx,maxlength)));
// }

float volume(in mat4 V)
{
    // VOLUME Compute volumes of the tet defined over vertices V
    vec3 a = V[0].xyz;
    vec3 b = V[1].xyz;
    vec3 c = V[2].xyz;
    vec3 d = V[3].xyz;
    
    float v = abs(dot((a-d),cross(b-d,c-d))/6);
    return v;
}

vec4 barycoord(in vec3 p3, in mat4 V) 
{
    vec4 a = V[0];
    vec4 b = V[1];
    vec4 c = V[2];
    vec4 d = V[3];
    vec4 p = vec4(p3,0.0); 

    mat4 T = mat4(p,b,c,d);
    float A1 = volume(T);

    T = mat4(p,c,a,d);
    float A2 = volume(T);
    
    T = mat4(p,b,d,a);
    float A3 = volume(T);

    T = mat4(p,b,c,a);
    float A4 = volume(T);
        
    float A = volume(V);

    vec4 B = vec4(A1/A, A2/A, A3/A, A4/A);

    // if (abs(1.0 - dot(B,vec4(1.0))) < 0.01) 
    // {      
    //     discard;
    // }

    return B;
}

int get_max_idx(in vec3 v) 
{
    return v.y > v.x ? ( v.z > v.y ? 2 : 1 ) : ( v.z > v.x ? 2 : 0 );
}

int get_max_idx(in vec4 d) 
{
    int maxidx = get_max_idx(vec3(d));
    return d[maxidx] > d[3] ? maxidx : 3;    
}

// returns 1 if we hit an isovalue, in which case isopoint will contatin the coordinate
// return zero otherwise
int raymarch(in  vec3  entrypoint, // point where ray enters the tet
             in  vec3  exitpoint,  // point where ray exits the tet
             in  mat4  D,          // diffusion values for all 4 corners of the tet
             in  mat4  V,          // vertex coordinates of the tet
             in  ivec4  M,          // vertex coordinates of the tet
             in  float isovalue,   // desired isovalue 

             out vec3  isopoint,
             out int maxidx)   // if isovalue was hit then ispoint will the contain that point
{
    vec3 r = normalize(exitpoint - entrypoint);
        
    //check if we are going throug portion of the tet that has no isosurface intersection
    // {
    //     vec4 d1 = diffuse(D, V, entrypoint);
    //     vec4 d2 = diffuse(D, V, exitpoint);
    //     int maxidx1 = get_max_idx(d1);
    //     int maxidx2 = get_max_idx(d2);
    //     if (M[maxidx1] == 0 && M[maxidx2] == 0)
    //         return 0;
    // }
    
    const int numSteps = 20;
    float stepSize = length(entrypoint - exitpoint)/numSteps;    
    for (int i = 0; i < numSteps; i++) 
    {
        isopoint = entrypoint + stepSize*i*r;
        vec4 d = diffuse(D, V, isopoint);
        maxidx = get_max_idx(d);
        // maxidx = func(D,V,isopoint);
        if (M[maxidx] != 0)
            return 1;
    }   

    return 0;
}

// #if 0
// float func2(in vec3 v0, in vec3 p, in float maxlength) 
// {
//     return length(v0 - p)/maxlength;    
// }

// // returns 1 if we hit an isovalue, in which case isopoint will contatin the coordinate
// // return zero otherwise
// int raymarch2(in  vec3  entrypoint, 
//             in  vec3  exitpoint, 
//             in  vec3  v0, 
//             in float maxlength,
//             in float isovalue,
//             out vec3  isopoint) 
// {
//     vec3 r = normalize(exitpoint - entrypoint);
    
//     const int numSteps = 200;
//     float stepSize = distance(exitpoint,entrypoint)/numSteps;
    
//     for (int i = 0; i < numSteps; i++) 
//     {
//         isopoint = entrypoint + stepSize*i*r;
//         if (func2(v0, isopoint, maxlength) <= isovalue) 
//             return 1;
//     }
//     return 0;
// }
// #endif

void 
fetchTetData(in int vTetId, out mat4 VO, out mat4 VC, out mat4 N, out mat4 D, out ivec4 M) 
{
    // fetch 4 verticies in object space for the current tet
    VO[0] = texelFetch(uVerSampler, 4*vTetId+0);
    VO[1] = texelFetch(uVerSampler, 4*vTetId+1);
    VO[2] = texelFetch(uVerSampler, 4*vTetId+2);
    VO[3] = texelFetch(uVerSampler, 4*vTetId+3);
    
    // convert 4 verticies in object space to camera space
    VC[0] = uMVMatrix * vec4(VO[0].xyz,1.0);
    VC[1] = uMVMatrix * vec4(VO[1].xyz,1.0);
    VC[2] = uMVMatrix * vec4(VO[2].xyz,1.0);
    VC[3] = uMVMatrix * vec4(VO[3].xyz,1.0);

    // fetch 4 normals for the current tet
    N[0] = texelFetch(uNormSampler, 4*vTetId+0);
    N[1] = texelFetch(uNormSampler, 4*vTetId+1);
    N[2] = texelFetch(uNormSampler, 4*vTetId+2);
    N[3] = texelFetch(uNormSampler, 4*vTetId+3);

    // fetch 4 diffusion values for the current tet
    D[0] = texelFetch(uDiffusedValuesSampler, 4*vTetId+0);
    D[1] = texelFetch(uDiffusedValuesSampler, 4*vTetId+1);
    D[2] = texelFetch(uDiffusedValuesSampler, 4*vTetId+2);
    D[3] = texelFetch(uDiffusedValuesSampler, 4*vTetId+3);
    // D = vec4(0.4, 0.6, 0.8, 0.9);

    // fetch which muscles IDs are you using for diffusion
    vec4 Mtemp = texelFetch(uMuscleSampler, vTetId);
    // Mtemp = Mtemp+0.5;
    M = ivec4(round(Mtemp[0]), round(Mtemp[1]), round(Mtemp[2]), round(Mtemp[3]));
}

void
findExitPoint(in mat4 VO, in mat4 N, in vec3 entrypoint, out vec3 exitpoint) 
{   
    vec3 e = vec3(0.); // eye origin in eye space
    // vec3 entrypoint = vec3(uMVMatrix * vec4(vPosition,1.0)); // entry point into tet in eye space   
    vec3 r = normalize(entrypoint - e); // ray  vector from the camera to  the entry point
    vec4 lambdavec = vec4(FLT_MAX);     // the interstion of the ray with the face i is e+lambda[i]*r 

    for (int i=0; i < 4; i++) 
    {   
        vec3 v = vec3(uMVMatrix*vec4(VO[3-i].xyz,1.0));
        vec3 n = normalize(uNMatrix*N[i].xyz);

        float num = dot(v - e, n);
        float denom = dot(r, n);    
        
        if (denom > 0.01) // face is not visible to a potential exit point
            lambdavec[i] = num/denom;
    }

    // find closest exit point
    float lambda = min(min(min(lambdavec[0], lambdavec[1]), lambdavec[2]), lambdavec[3]);
    exitpoint = e + lambda*r;
}

#if RUN_MAIN

bool unit_test()
{   
    // vec4 MM = texelFetch(uMuscleSampler, 0);
    // vec4 Mtemp = MM;
    // ivec4 M = ivec4(round(Mtemp[0]), round(Mtemp[1]), round(Mtemp[2]), round(Mtemp[3]));
    int tetid = 56986-1;
    mat4 VO, VC, N, D;
    ivec4 M; 
    fetchTetData(tetid, VO, VC, N, D, M);
    vec3 G = gradient(D, VO, N, 0); 

    vec3 GG = vec3(-0.72832, -0.2819, -0.33206);
    // vec3 GG = vec3(0.84144, 0.81077 , 0.83597);
    if (abs(G[0] - GG[0])>0.01)
        return false;
    
    if (abs(G[1] - GG[1])>0.01)
        return false;
    
    if (abs(G[2] - GG[2])>0.01)
        return false;
    
    // if (abs(D[0][1] - GG[1])>0.01)
    //     return false;
    
    // if (abs(D[0][2] - GG[2])>0.01)
    //     return false;

    // vec3 g = D[0][maxidx]*g0 + D[1][maxidx]*g1 + D[2][maxidx]*g2 + D[3][maxidx]*g3;

    // Mtemp = Mtemp+0.5;
    // ivec4 MM = ivec4(Mtemp);
    
    // if (int(MM[0] +0.5) != 2)
    //     return false; 
    // if ( int(MM[1] +0.5) != 0)
    //     return false; 
    // if (int(MM[2] -0.5) != -1)
    //     return false; 
    // if (int(MM[3] -0.5) != -1)
    //     return false; 

    // if (M[0] != 0)
    //     return false; 
    // if (M[1] != -1)
    //     return false; 
    // if (M[2] != -1)
    //     return false; 
    // if (M[3] != -1)
    //     return false; 
    
    return true;
}


void main()
{   
    // if (!unit_test()) 
    // {
    //     discard;
    //     return;
    // }

    mat4 VO, VC, N, D;
    ivec4 M; 
    fetchTetData(vTetId, VO, VC, N, D, M);

    vec3 entrypoint = vec3(uMVMatrix * vec4(vPosition,1.0)); // entry point into tet in eye space
    vec3 exitpoint;
    findExitPoint(VO, N, entrypoint, exitpoint);

    vec3 u = exitpoint-entrypoint;  

    // compute max depth of the tet
    // vec3 center = vec3(0.25*(VC[0] + VC[1] + VC[2] + VC[3]));
    // center = vec3(uMVMatrix  * vec4(center, 1));
    // vec3 v0 = VC[0].xyz;
    // float tl = 2*length(center - v0);

    vec3 isopoint;
    int maxidx;
    int rm = raymarch(entrypoint, exitpoint, D, VC, M, 0.1,  isopoint, maxidx);
    // int rm = raymarch2(entrypoint, exitpoint, v0, tl, 0.5, isopoint);

    if (rm == 0) 
    {
        discard;  // we didnt hit isovalue so discard the  point
    }
    else 
    {   

        // we hit the isovalue so compute lighting for the point
        // vec3 g = vec3(0.0);//vec3(texelFetch(uGradSampler, vTetId));
        vec3 eyeNormal;
        // if (length(isopoint-entrypoint) < 0.001) 
        //     eyeNormal = normalize(uNMatrix * normalize(vNormal));
        // else 
        // eyeNormal = vec3(normalize(uNMatrix * normalize(-1*g))); //calcNormal(D,VC,isopoint);
        // eyeNormal = calcNormal(D,VC,isopoint,maxidx);
        vec3 gvec = gradient(D, VO, N, maxidx);
        eyeNormal = -1*normalize(uNMatrix*gvec);
        
        float intensity = max(c_zero, dot(eyeNormal, cLightDir));
        vec4 uFragClr =  texelFetch(uMuscleColorSampler, M[maxidx]%20);
        uFragClr = vec4(161/255.0,44/255.0,44/255.0,1);
        
          // overwrite depth
        // vec3 isopointscreen = glslproject(isopoint, uPMatrix, uViewportCustom);
        // gl_FragDepth = isopointscreen.z;
        
        fFragColor.rgb = max(0.3 * uFragClr.rgb, intensity * uFragClr.rgb);

        bool show_wires = false;
        if (show_wires) 
        {
            vec4 bb=barycoord(isopoint, VC); //TODO: we are calculating this twice
            int dd=0;
            for (int jj=0;jj<4;++jj)  
                if (bb[jj] < 0.01) 
                    dd++; 
            
            if (dd == 1)
                fFragColor.rgb = vec3(0); 
        }
        // if (bb[1] < 0.001)
        //     fFragColor.rgb = vec3(0);
        // if (bb[2] < 0.001)
        //     fFragColor.rgb = vec3(0);
        // if (bb[3] < 0.001)
        //     fFragColor.rgb = vec3(0);

        // if (length(isopoint-entrypoint) < 0.001)
        //     fFragColor.rgb = vec3(0,1,0);
        // if (length(isopoint-exitpoint) < 0.001)
            // fFragColor.rgb = vec3(0,0,1);
        // fFragColor.rgb = (length(u)/tl) * uColor.rgb;
        // fFragColor.rgb = uColor.rgb;
        // fFragColor.a = uColor.a;    
        // if (M[maxidx] == -1) 
        // {
        //     // fFragColor = vec4(0); //uColor+uViewportCustom;
        //     // discard;
        //     // fFragColor.x = fFragColor.y + vDiffusion + vNormal.y;
        // }
        // else  
        // {  
        //     // fFragColor =  texelFetch(uMuscleColorSampler, M[maxidx]);
        // }

      
    }

    // vec4 DD = vec4(0.4, 0.6, 0.8, 0.9);
    // // for (int i = 0; i < 4; ++i)
    // int i = 2;
    // if (abs(DD[i] - D[i]) > 0.1)
    //     discard;
    
    // vec4(vPosition,1.0)
    // vec3 entrypointscreen = glslproject(vPosition, uMVPMatrix, uViewportCustom);
    
    // if (abs(gl_FragCoord.x - entrypointscreen.x) > 0.01)
    //     discard;
    
    // if (abs(gl_FragCoord.y - entrypointscreen.y) > 0.01)
    //     discard;
    
    // if (abs(gl_FragDepth - entrypointscreen.z) > 0.02)
    //     discard;

        // fFragColor = vec4(0.0, 1.0, 0.0, 1.0);
}

#elif RUN_BARY_UNIT_TEST
// void main()
// {   
//     mat4 VO, VC, N; 
//     vec4 D;
//     fetchTetData(vTetId, VO, VC, N, D);

//     vec3 entrypoint = vec3(uMVMatrix * vec4(vPosition,1.0)); // entry point into tet in eye space
//     vec3 exitpoint;
//     findExitPoint(VO, N, entrypoint, exitpoint);
    
//     vec3 u = exitpoint-entrypoint;

//     vec3 isopoint;
//     int rm = raymarch(entrypoint, exitpoint, D, VC, 50.0, isopoint);

//     if (rm == 0) 
//     {
//         discard;  // we didnt hit isovalue so discard the  point
//     }
//     else 
//     {   
//         // we hit the isovalue so compute lighting for the point

//         vec3 eyeNormal;
//         // if (length(isopoint-entrypoint) < 0.01) 
//         // {
//             eyeNormal = normalize(uNMatrix * normalize(vNormal));
//         // }
//         // else 
//         // {
//         //     eyeNormal =  calcNormal(v0, isopoint, tl );
//         // }
        
//         float intensity = max(c_zero, dot(eyeNormal, cLightDir));
//         fFragColor.rgb = max(0.3 * uColor.rgb, intensity * uColor.rgb);
//         fFragColor.a = uColor.a;

//         // overwrite depth
//         vec3 isopointscreen = glslproject(isopoint, uPMatrix, uViewportCustom);
//         // gl_FragDepth = isopointscreen.z;
//     }

//     vec4 b1 = vec4(0.25);
//     vec3 center = vec3(b1[0]*VC[0] + b1[1]*VC[1] + b1[2]*VC[2]+ b1[3]*VC[3]);
//     vec4 b2 = barycoord(center, VC);
//     for (int i = 0; i < 4; ++i)
//         if (b2[i] < 0 || abs(b2[i] - b1[i]) > 0.01)
//             discard;
    
//     b1 = vec4(1., 0., 0., 0.);
//     center = vec3(b1[0]*VC[0] + b1[1]*VC[1] + b1[2]*VC[2]+ b1[3]*VC[3]);
//     b2 = barycoord(center, VC);
//     for (int i = 0; i < 4; ++i)
//         if (b2[i] < 0 || abs(b2[i] - b1[i]) > 0.01)
//             discard;

//     b1 = vec4(0., 0., 1.0, 0.);
//     center = vec3(b1[0]*VC[0] + b1[1]*VC[1] + b1[2]*VC[2]+ b1[3]*VC[3]);
//     b2 = barycoord(center, VC);
//     for (int i = 0; i < 4; ++i)
//         if (b2[i] < 0 || abs(b2[i] - b1[i]) > 0.01)
//             discard;
    
//     fFragColor = vec4(1.0, 0.0, 0.0, 0.0);
// }
#endif

