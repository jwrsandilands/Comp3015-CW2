#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec4 Position;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

void main()
{
    //pass out normal (aka n (direction of a face) from model coords to view coords)
    Normal = normalize(NormalMatrix * VertexNormal);

    //pass out vertex position from model coords to view coords
    Position = (ModelViewMatrix * vec4(VertexPosition, 1.0));

    //pass Vertex Texture Co-ordinates to out variable TexCoord
    TexCoord = VertexTexCoord;

    //set gl_position
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}
