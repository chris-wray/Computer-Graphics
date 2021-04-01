#version 120

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;

varying vec3 normal;
varying vec3 vpos;

struct light
{
	vec3 position;
	float intensity;
};

uniform vec3 light0_position;
uniform float light0_intensity;
uniform vec3 light1_position;
uniform float light1_intensity;



void main()
{
	light lights[2];
	lights[0].position = light0_position;
	lights[0].intensity = light0_intensity;

	vec3 C = vec3(0.0f, 0.0f, 0.0f);
	vec3 E = normalize(C - vpos);
	vec3 N = normalize(normal);
	vec3 color =  vec3(0.0f, 0.0f, 0.0f);

		vec3 L = normalize(lights[0].position - vpos);
		vec3 H = normalize(E+L);
		vec3 diffuse = vec3(kd*max(0, dot(L, N)));
		vec3 spec = vec3(ks*pow(max(0, dot(H, N)), s));
		vec3 col = lights[0].intensity*(ka + diffuse + spec);
		color = color + col;

	gl_FragColor = vec4(color.r, color.g, color.b, 1.0);
}
