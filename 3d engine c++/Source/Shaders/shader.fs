#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoord;
//flat in float faceType;
flat in float faceLight;

uniform sampler2D texture0;

void main()
{
    // Set the base color to the texture color
    vec4 baseColor = vec4(1.0, 1.0, 1.0, 1.0);
    //vec4 baseColor = texture(texture0, TexCoord);

    // Determine face orientation based on FragPos
    vec3 orientationColor = vec3(1.0); // Full brightness by default

    // Assuming the cube is axis-aligned:
    //float faceType=0.0f;
    
    /*
    if (faceType == 0.0) // top face
        orientationColor = vec3(1.0, 1.0, 1.0);
    else if (faceType == 1.0) // front and back
        orientationColor = vec3(0.8, 0.8, 0.8); // Less bright color for the top face
    else if (faceType == 2.0) // left-right
        orientationColor = vec3(0.6, 0.6, 0.6);
    else if (faceType == 3.0) // bottom
        orientationColor = vec3(0.2, 0.2, 0.2);
    */
    orientationColor = vec3(faceLight, faceLight, faceLight);

    if (FragPos.x >0){
        orientationColor.yz = vec2(0,0);
    }
    else if (FragPos.y >127){
        orientationColor.xz = vec2(0,0);
    }
    else if (FragPos.z >0){
        orientationColor.xy = vec2(0,0);
    }

    // Mix the base color with the orientation color
    vec4 finalColor = baseColor * vec4(orientationColor, 1.0);

    FragColor = finalColor;

}