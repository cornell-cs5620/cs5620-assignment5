/*
 * light.cpp
 *
 *  Created on: Dec 17, 2014
 *      Author: nbingham
 */

#include "light.h"
#include "object.h"
#include "canvas.h"

lighthdl::lighthdl()
{
	model = NULL;
	type = "light";
}

lighthdl::lighthdl(const vec3f &ambient, const vec3f &diffuse, const vec3f &specular)
{
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	model = NULL;
	type = "light";
}

lighthdl::~lighthdl()
{

}

directionalhdl::directionalhdl() : lighthdl(white*0.1f, white*0.5f, white)
{
	type = "directional";
}

directionalhdl::directionalhdl(const vec3f &direction, const vec3f &ambient, const vec3f &diffuse, const vec3f &specular) : lighthdl(ambient, diffuse, specular)
{
	type = "directional";
}

directionalhdl::~directionalhdl()
{

}

void directionalhdl::update(canvashdl *canvas)
{
	if (canvas != NULL && model != NULL)
	{
		canvas->translate(model->position);
		canvas->rotate(model->orientation[0], vec3f(1.0, 0.0, 0.0));
		canvas->rotate(model->orientation[1], vec3f(0.0, 1.0, 0.0));
		canvas->rotate(model->orientation[2], vec3f(0.0, 0.0, 1.0));

		canvas->update_normal_matrix();
		direction = canvas->matrices[canvashdl::normal_matrix]*vec4f(0.0, 0.0, -1.0, 0.0);

		canvas->rotate(-model->orientation[2], vec3f(0.0, 0.0, 1.0));
		canvas->rotate(-model->orientation[1], vec3f(0.0, 1.0, 0.0));
		canvas->rotate(-model->orientation[0], vec3f(1.0, 0.0, 0.0));
		canvas->translate(-model->position);
	}
}

void directionalhdl::shade(vec3f &ambient, vec3f &diffuse, vec3f &specular, vec3f vertex, vec3f normal, float shininess) const
{
	vec3f eye_direction = -vertex;
	float eye_distance = mag(eye_direction);
	eye_direction /= eye_distance;

	vec3f half_vector = norm(direction + eye_direction);

	float normal_dot_light_direction = max(0.0f, dot(normal, direction));
	float normal_dot_half_vector = max(0.0f, dot(normal, half_vector));

	float power_factor = 0.0;
	if (normal_dot_light_direction > 0.0)
		power_factor = pow(normal_dot_half_vector, shininess);

	ambient += this->ambient;
	diffuse += this->diffuse*normal_dot_light_direction;
	specular += this->specular*power_factor;
}

pointhdl::pointhdl() : lighthdl(white*0.1f, white*0.5f, white)
{
	this->attenuation = vec3f(1.0, 0.14, 0.7);
	type = "point";
}

pointhdl::pointhdl(const vec3f &position, const vec3f &attenuation, const vec3f &ambient, const vec3f &diffuse, const vec3f &specular) : lighthdl(ambient, diffuse, specular)
{
	this->attenuation = attenuation;
	type = "point";
}

pointhdl::~pointhdl()
{

}

void pointhdl::update(canvashdl *canvas)
{
	if (canvas != NULL && model != NULL)
	{
		canvas->translate(model->position);
		canvas->rotate(model->orientation[0], vec3f(1.0, 0.0, 0.0));
		canvas->rotate(model->orientation[1], vec3f(0.0, 1.0, 0.0));
		canvas->rotate(model->orientation[2], vec3f(0.0, 0.0, 1.0));

		vec4f p = canvas->matrices[canvashdl::modelview_matrix]*vec4f(0.0, 0.0, 0.0, 1.0);
		position = p(0,3)/p[3];

		canvas->rotate(-model->orientation[2], vec3f(0.0, 0.0, 1.0));
		canvas->rotate(-model->orientation[1], vec3f(0.0, 1.0, 0.0));
		canvas->rotate(-model->orientation[0], vec3f(1.0, 0.0, 0.0));
		canvas->translate(-model->position);
	}
}

void pointhdl::shade(vec3f &ambient, vec3f &diffuse, vec3f &specular, vec3f vertex, vec3f normal, float shininess) const
{
	vec3f light_direction = position - vertex;
	float light_distance = mag(light_direction);
	light_direction /= light_distance;

	vec3f eye_direction = -vertex;
	float eye_distance = mag(eye_direction);
	eye_direction /= eye_distance;

	float att = 1.0/(attenuation[0] + attenuation[1]*light_distance + attenuation[2]*light_distance*light_distance);

	vec3f half_vector = norm(light_direction + eye_direction);

	float normal_dot_light_direction = max(0.0f, dot(normal, light_direction));
	float normal_dot_half_vector = max(0.0f, dot(normal, half_vector));

	float power_factor = 0.0;
	if (normal_dot_light_direction > 0.0)
		power_factor = pow(normal_dot_half_vector, shininess);
    
	ambient += this->ambient*att;
	diffuse += this->diffuse*normal_dot_light_direction*att;
	specular += this->specular*power_factor*att;
}

spothdl::spothdl() : lighthdl(white*0.1f, white*0.5f, white)
{
	this->attenuation = vec3f(1.0, 0.14, 0.7);
	this->cutoff = 0.5;
	this->exponent = 1.0;
	type = "spot";
}

spothdl::spothdl(const vec3f &attenuation, const float &cutoff, const float &exponent, const vec3f &ambient, const vec3f &diffuse, const vec3f &specular) : lighthdl(ambient, diffuse, specular)
{
	this->attenuation = attenuation;
	this->cutoff = cutoff;
	this->exponent = exponent;
	type = "spot";
}

spothdl::~spothdl()
{

}

void spothdl::update(canvashdl *canvas)
{
	if (canvas != NULL && model != NULL)
	{
		canvas->translate(model->position);
		canvas->rotate(model->orientation[0], vec3f(1.0, 0.0, 0.0));
		canvas->rotate(model->orientation[1], vec3f(0.0, 1.0, 0.0));
		canvas->rotate(model->orientation[2], vec3f(0.0, 0.0, 1.0));

		vec4f p = canvas->matrices[canvashdl::modelview_matrix]*vec4f(0.0, 0.0, 0.0, 1.0);
		position = p(0,3)/p[3];
		canvas->update_normal_matrix();
		direction = canvas->matrices[canvashdl::normal_matrix]*vec4f(0.0, 0.0, -1.0, 0.0);

		canvas->rotate(-model->orientation[2], vec3f(0.0, 0.0, 1.0));
		canvas->rotate(-model->orientation[1], vec3f(0.0, 1.0, 0.0));
		canvas->rotate(-model->orientation[0], vec3f(1.0, 0.0, 0.0));
		canvas->translate(-model->position);
	}
}

void spothdl::shade(vec3f &ambient, vec3f &diffuse, vec3f &specular, vec3f vertex, vec3f normal, float shininess) const
{
	vec3f light_direction = position - vertex;
	float light_distance = mag(light_direction);
	light_direction /= light_distance;

	vec3f eye_direction = -vertex;
	float eye_distance = mag(eye_direction);
	eye_direction /= eye_distance;

	float att = 1.0/(attenuation[0] + attenuation[1]*light_distance + attenuation[2]*light_distance*light_distance);
	float spotdot = dot(-light_direction, direction);

	float spotatt = 0.0;
	if (spotdot >= cutoff)
		spotatt = pow(spotdot, exponent);

	att *= spotatt;

	vec3f half_vector = norm(light_direction + norm(-vertex));
	float normal_dot_light_direction = max(0.0f, dot(normal, light_direction));
	float normal_dot_half_vector = max(0.0f, dot(normal, half_vector));

	float power_factor = 0.0;
	if (normal_dot_light_direction > 0.0)
		power_factor = pow(normal_dot_half_vector, shininess);

	ambient += this->ambient*att;
	diffuse += this->diffuse*normal_dot_light_direction*att;
	specular += this->specular*power_factor*att;
}
