#version 460

#define PI 3.14159265

in vec4 Position;
in vec3 Normal;
in vec2 TexCoord;
in vec4 ShadowCoord;

//uniform variables
//lights
uniform struct Light{
    vec4 Position;   //Light Position In Eye Co-ords
    vec3 La;         //Ambient Light Intensity
    vec3 L;          //Diffuse and Specular Light Intensity
} lights[1];

//materials
uniform struct Material{
    vec3 Ka;         //Ambient Reflectivity
    vec3 Kd;         //Diffuse Reflectivity
    vec3 Ks;         //Specular Reflectivity
    float Shininess; //Specular Shininess Factor
    int texID;
} material;

//Cell Shading Constraints
const int levels = 3;
const float scaleFactor = 1.0/levels;

//fog shading info
uniform struct FogInfo{
    float MaxDist;  //distance at which things disappear
    float MinDist;  //distance at which things start to disappear
    vec3 Color;     //Colour of the Fog
}fogInfo;

layout (binding = 0) uniform sampler2DShadow ShadowMap;
uniform vec4 Color;

//create noise
uniform int HasSnow;
uniform int IsCloud;
uniform vec3 SnowColor = vec3(1.0, 1.0, 1.0);
uniform vec4 SkyColor = vec4(0.6f, 0.8f, 1.0f, 1.0f);
uniform vec4 CloudColor = vec4(1.0);

layout (location = 0) out vec4 FragColor;

layout (binding = 3) uniform sampler2D Tex1;
layout (binding = 2) uniform sampler2D NoiseTex;


vec3 blinnPhongModel(int light, vec4 position, vec3 normal){

    //calculate colours
    vec3 texColor = texture(Tex1, TexCoord).rgb;                //get the colour of any given point of a texture

    //calculate ambient here to access each light LA value use this: "lights[light].La"
    vec3 ambient = texColor * lights[light].La;

    //calculate the diffuse here
    vec4 pos = position;                                        //transform vertex position (mesh size) from model coords to view coords
    vec3 s = normalize(vec3(lights[light].Position - pos));     //calculate the direction from which the light strikes the mesh (already in view coords)
    vec3 n = normal;                                            //transform normal (direction of a face) from model coords to view coords
    float sDotN = max(dot(s,n), 0.0);                           //calculate dot product for s (light projection direction) and n (direction of a face)
    vec3 diffuse = texColor * sDotN;                            //calculate light reflections/intensity bouncing off the material using the diffuse lighting calculation
    //vec3 diffuse = texColor * floor(sDotN * levels) * scaleFactor;

    //calculate specular here
    vec3 specular = vec3(0.0);

    if( sDotN > 0.0){
        vec3 v = normalize(-position.xyz);

        //--phong model--
        //vec3 r = reflect(-s, n);
        //specular = material.Ks * pow(max(dot(r,v),0.0), material.Shininess);

        //--blinn-phong model--
        //(more efficient with nearly identical results)
        vec3 h = normalize(v + s);
        specular = material.Ks * pow(max(dot(h,n),0.0), material.Shininess);
    }

    //add together and return
    return ambient + lights[light].L * (diffuse + specular);
}

vec3 ambientCalc(int light) {
    
    //calculate colours
    vec3 texColor = texture(Tex1, TexCoord).rgb; //get the colour of any given point of a texture

    //calculate ambient here to access each light LA value use this: "lights[light].La"
    vec3 ambient = texColor * lights[light].La;

    return ambient;
}

vec3 BlinnPhongModelDiffAndSpec(int light, vec4 position, vec3 normal){
    //calculate colours
    vec3 texColor = texture(Tex1, TexCoord).rgb; //get the colour of any given point of a texture


    //calculate the diffuse here
    vec4 pos = position;                                        //transform vertex position (mesh size) from model coords to view coords
    vec3 s = normalize(vec3(lights[light].Position - pos));     //calculate the direction from which the light strikes the mesh (already in view coords)
    vec3 n = normal;                                            //transform normal (direction of a face) from model coords to view coords
    float sDotN = max(dot(s,n), 0.0);                           //calculate dot product for s (light projection direction) and n (direction of a face)
    vec3 diffuse = texColor * sDotN;                            //calculate light reflections/intensity bouncing off the material using the diffuse lighting calculation
    //vec3 diffuse = texColor * floor(sDotN * levels) * scaleFactor;


    //calculate specular here
    vec3 specular = vec3(0.0);

    if( sDotN > 0.0){
        vec3 v = normalize(-position.xyz);

        //--phong model--
        //vec3 r = reflect(-s, n);
        //specular = material.Ks * pow(max(dot(r,v),0.0), material.Shininess);

        //--blinn-phong model--
        //(more efficient with nearly identical results)
        vec3 h = normalize(v + s);
        specular = material.Ks * pow(max(dot(h,n),0.0), material.Shininess);
    }

    return lights[light].L * (diffuse + specular);
}


subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

subroutine(RenderPassType) void shadeWithShadow() {    
    //for fog
    float dist = abs(Position.z); //distance calculation
    float fogFactor = (fogInfo.MaxDist - dist) / (fogInfo.MaxDist - fogInfo.MinDist); //fog calculation following the formula to do so
    fogFactor = clamp(fogFactor, 0.0, 1.0); //clamps the values

    //evaluate the lighting equation for each light (and account for fog)
    vec3 ambient; //create ambient variable
    vec3 diffAndSpec = vec3(0.0);
    for(int i = 0; i < 1; i++){
        ambient = ambientCalc(i);
        diffAndSpec += BlinnPhongModelDiffAndSpec(i, Position, normalize(Normal));
    }

    //create noise for snow
    vec4 noise = texture(NoiseTex, TexCoord);

    vec3 newColor = material.Kd;

    if(HasSnow == 1){
        newColor = material.Kd;
        if(noise.a > 0.55) newColor = SnowColor;
        //if(noise.a > 0.6) newColor = CloudColor;
    }

    //create cloud colours
    noise = texture(NoiseTex, TexCoord);
    float t = (cos(noise.a * PI) +1.0) /2.0;
    vec4 skyMixColor = mix(SkyColor, CloudColor, t);

    //project shadow map
    float shadow = 1.0;
    if(ShadowCoord.z >= 0) {
        shadow = textureProj(ShadowMap, ShadowCoord);
    }

    //If fragment is in the shadow use ambient only
    vec3 mixedColor = mix(fogInfo.Color, diffAndSpec * shadow + newColor + ambient, fogFactor); //Mix the colours with the fog colours
    FragColor = vec4(mixedColor, 1.0);

    //gamma correct
    FragColor = pow(FragColor, vec4(1.0/2.2));

    //if is a cloud
    if(IsCloud == 1){
        //mixedColor = mix(fogInfo.Color, skyMixColor.rgb, fogFactor);
        //FragColor = vec4(mixedColor, 1.0);
        FragColor = vec4(skyMixColor.rgb, 1.0);
    }
}

subroutine (RenderPassType) void recordDepth(){
    //nothing, depth written automatically
}



void main() {

//    //transform normal from model coords to view coords
//    vec3 n = Normal;

//    //transform vertex pos from model coords to view coords
//    vec4 pos = Position;

//    //evaluate lighting equation for each light:
//    FragColor = vec4(0.0);
//    for(int i = 0; i < 3; i++){
//        FragColor += vec4(blinnPhongModel(i, pos, normalize(Normal)),1);
//    }

    RenderPass();
}