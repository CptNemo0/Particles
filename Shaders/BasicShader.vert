#version 430 core

layout (location = 0) in vec3 iv_position;

uniform mat4 view_matrix;
uniform mat4 projection_matrix;

layout(std430, binding = 1) buffer position_buffer 
{
    float data[];
};

out float id;

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
                  1.0,           0.0,           0.0, 0.0,
                  0.0,           1.0,           0.0, 0.0,
                  0.0,           0.0,           1.0, 0.0,
        translation.x, translation.y, translation.z, 1.0
    );
}

void main()
{
    mat4 translation = TranslationMatrix(vec3(data[gl_InstanceID * 3], data[gl_InstanceID * 3 + 1], data[gl_InstanceID * 3 + 2]));
    mat4 scale = ScaleMatrix(vec3(1.0));
    mat4 model_matrix = translation * scale;

    vec4 world_position = model_matrix * vec4(iv_position, 1.0);
    
	gl_Position = projection_matrix * view_matrix * model_matrix * world_position;

    vec3 a = vec3(0.0, 0.0, 0.0) - world_position.xyz;
    float l = length(a);

    id = float(gl_InstanceID);
}