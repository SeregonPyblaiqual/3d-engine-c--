#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 TexCoord[];
in vec3 FragPos[];

out vec2 gTexCoord;
out vec3 gFragPos;
out vec3 gNormal;
out float gFaceLigth;

void main()
{
    // Calculate the two edge vectors for the triangle
    vec3 edge1 = FragPos[1] - FragPos[0];
    vec3 edge2 = FragPos[2] - FragPos[0];

    // Compute the normal using the cross product
    vec3 normal = normalize(cross(edge1, edge2));
    //vec3 normal = vec3(0,1,0);
    ivec3 inormal = ivec3(round(normal[0]), round(normal[1]), round(normal[2])); 
    float faceLight;
    
    
    if (inormal[0]!=0){
        faceLight = 0.8f;
    }
    else if (inormal[1]!=0){
        faceLight = 1.0f;
    }
    else if (inormal[2]!=0){
        faceLight = 0.6f;
    }
    
    
    // Emit the vertices with calculated normals
    for (int i = 0; i < 3; ++i)
    {
        gFragPos = FragPos[i];
        gTexCoord = TexCoord[i];
        gNormal = normal;
        gFaceLigth = faceLight;
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();

}