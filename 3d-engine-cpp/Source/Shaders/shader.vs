#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in uint texCoord_lightIndexes;
// 1 first bits: texCoords.x: 5
// 1 second bits: texCoords.y: 4
// 2 third bits: lightIndexes: 0-3
// 0x...XYLL
//      3210
out vec2 TexCoord;
out vec3 FragPos;
flat out float FaceLight;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));

    uint lightIndex = texCoord_lightIndexes & uint(0x3); // 0b00000011
    if (lightIndex == uint(0x3)) // 0b11
        FaceLight = 0.2f;
    else if (lightIndex == uint(0x2)) // 0b10
        FaceLight = 0.6f;
    else if (lightIndex == uint(0x1)) // 0b01
        FaceLight = 1.0f;
    else
        FaceLight = 0.8f;

    // Extract texture coordinates
    uint piece_of_info = (texCoord_lightIndexes >> 2); // Shift right by 2 bits
    float texX = float((piece_of_info & uint(0x2)) != 0u); // 0b0010
    float texY = float((piece_of_info & uint(0x1)) != 0u); // 0b0001
    
    TexCoord = vec2(texX, texY);
}