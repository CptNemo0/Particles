#version 430 core

layout (location = 0) in vec3 iv_position;
layout (location = 0) in vec3 iv_normal;

uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform float size = 0.5;

layout(std430, binding = 1) buffer position_buffer 
{
    float data[];
};

out float id;
out vec3 world_pos;
out vec3 view_pos;
out vec3 normal;

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
    mat4 scale = ScaleMatrix(vec3(size));
    mat4 model_matrix = translation * scale;

    vec4 world_position = model_matrix * vec4(iv_position, 1.0);
    vec4 view_position = view_matrix * world_position;
	gl_Position = projection_matrix *  view_position;

    vec3 a = vec3(0.0, 0.0, 0.0) - world_position.xyz;
    float l = length(a);

    id = float(gl_InstanceID);
    normal = mat3(transpose(inverse(model_matrix))) * iv_normal; 
    world_pos = world_position.xyz;
    view_pos = view_position.xyz;
}