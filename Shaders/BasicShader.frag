#version 430 core

float rand(float n){return fract(sin(n) * 43758.5453123);}

float noise(float p)
{
	float fl = floor(p);
    float fc = fract(p);
	return mix(rand(fl), rand(fl + 1.0), fc);
}

in float id;
in vec3 normal;
in vec3 world_pos;
in vec3 view_pos;

out vec4 out_color;

vec3 light_position  = vec3(0.0f, 4.5f, 0.0f);
float light_intensity = 100.0f;
vec3 camera_position = vec3(32.5f, 32.5, 32.5);

void main()
{
	vec3 light_direction = light_position - world_pos;
	float magnitude = length(light_direction);
	light_direction = normalize(light_direction);

	float dp = dot(normal, light_direction);
	vec3 color = vec3(0.2, 0.5, 0.8);
	if(dp < 0)
	{
		color *= 0.0;
	}
	
	float attenuation = 1.0 / (magnitude * magnitude);
	color *= attenuation * light_intensity;

	out_color = vec4(color, 1.0);
}