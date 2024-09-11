#version 430 core

float rand(float n){return fract(sin(n) * 43758.5453123);}

float noise(float p)
{
	float fl = floor(p);
    float fc = fract(p);
	return mix(rand(fl), rand(fl + 1.0), fc);
}

in float id;

out vec4 out_color;

void main()
{
	float c = noise(id);
	out_color = vec4(c,c,c, 1.0);
}