#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in float aFaceLighting;

out vec2 TexCoord;
out vec3 FragPos;
flat out float faceLight;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform samplerBuffer positions;

mat4 translation(int pos){
    mat4 translated = mat4(1.0);
    translated[3][0] = texelFetch(positions, pos).x;
    translated[3][1] = texelFetch(positions, pos).y;
    translated[3][2] = texelFetch(positions, pos).z;
    return translated;
}

void main()
{
    mat4 model = translation(gl_InstanceID);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    FragPos = vec3(model * vec4(aPos, 1.0));
    faceLight = aFaceLighting;
}