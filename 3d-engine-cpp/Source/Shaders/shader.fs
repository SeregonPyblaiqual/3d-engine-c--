#version 330 core
out vec4 FragColor;


in vec3 FragPos;
in vec2 TexCoord;
flat in float FaceLight;

uniform sampler2D texture0;

void main()
{
    // Set the base color to the texture color
    vec4 baseColor = texture(texture0, TexCoord);// vec4(1.0, 1.0, 1.0, 1.0);// texture(texture0, TexCoord);//
    //float faceLight=1.0f;
    /*
    if (gNormal==vec3(1.0,0.0,0.0)){
        faceLight = 0.8f;
    }
    if (gNormal==vec3(-1.0,0.0,0.0)){
        faceLight = 0.8f;
    }
    if (gNormal==vec3(0.0,1.0,0.0)){
        faceLight = 1.0f;
    }
    if (gNormal==vec3(0.0,-1.0,0.0)){
        faceLight = 0.2f;
    }
    if (gNormal==vec3(0.0,0.0,1.0)){
        faceLight = 0.6f;
    }
    if (gNormal==vec3(0.0,0.0,-1.0)){
        faceLight = 0.6f;
    }*/

    vec3 orientationColor = vec3(FaceLight, FaceLight, FaceLight);//vec3(1.0,1.0,1.0);//vec3(faceLight, faceLight, faceLight);
    
    if (FragPos.x >0){
        orientationColor.yz = vec2(0,0);
    }
    else if (FragPos.y >127){
        orientationColor.xz = vec2(0,0);
    }
    else if (FragPos.z >0){
        orientationColor.xy = vec2(0,0);
    }
    
    FragColor = baseColor * vec4(orientationColor, 1.0);;

}