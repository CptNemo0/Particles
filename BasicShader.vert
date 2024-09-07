#version 430 core
layout (location = 0) in vec3 iv_position;

//uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

layout(std430, binding = 1) buffer MyBuffer 
{
    vec2 data[];  // SSBO array
};

mat4 ScaleMatrix(vec3 scale) 
{
    return mat4
    (
        scale.x, 0.0,     0.0,     0.0,
        0.0,     scale.y, 0.0,     0.0,
        0.0,     0.0,     scale.z, 0.0,
        0.0,     0.0,     0.0,     1.0
    );
}

mat4 TranslationMatrix(vec3 translation) 
{
    return mat4
    (
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        translation.x, translation.y, translation.z, 1.0
    );
}

void main()
{
    vec2 pos = data[gl_InstanceID];
    mat4 t = TranslationMatrix(vec3(pos.x, pos.y, 1.0));
    mat4 s = ScaleMatrix(vec3(2.0, 2.0, 2.0));
    mat4 model_matrix = t * s;
     
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(iv_position, 1.0);
}