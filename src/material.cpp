/*
 * material.cpp
 *
 *  Created on: Dec 18, 2014
 *      Author: nbingham
 */

#include "material.h"
#include "canvas.h"
#include "light.h"

materialhdl::materialhdl()
{
	type = "material";
}

materialhdl::~materialhdl()
{
}

solidhdl::solidhdl()
{
	type = "uniform";
	emission = vec3f(0.0, 0.0, 0.0);
	ambient = vec3f(0.1, 0.1, 0.1);
	diffuse = vec3f(1.0, 1.0, 1.0);
	specular = vec3f(1.0, 1.0, 1.0);
	shininess = 1.0;
}

solidhdl::~solidhdl()
{

}

vec3f solidhdl::shade_vertex(canvashdl *canvas, vec3f vertex, vec3f normal, vector<float> &varying) const
{
	vec4f eye_space_vertex = canvas->matrices[canvashdl::modelview_matrix]*homogenize(vertex);

	if (canvas->shade_model == canvashdl::flat || canvas->shade_model == canvashdl::gouraud)
	{
		const vector<lighthdl*> *lights;

		canvas->get_uniform("lights", lights);

		vec3f color(1.0, 1.0, 1.0);
		if (mag2(normal) > 0.0)
		{
			vec3f eye_space_normal = canvas->matrices[canvashdl::normal_matrix]*(vec4f)normal;

			vec3f light_ambient(0.0, 0.0, 0.0);
			vec3f light_diffuse(0.0, 0.0, 0.0);
			vec3f light_specular(0.0, 0.0, 0.0);

			for (int j = 0; j < lights->size(); j++)
				if (lights->at(j) != NULL)
					lights->at(j)->shade(light_ambient, light_diffuse, light_specular, eye_space_vertex, eye_space_normal, shininess);

			color = clamp(emission + ambient*light_ambient + diffuse*light_diffuse + specular*light_specular, 0.0f, 1.0f);
		}

		varying.push_back(color[0]);
		varying.push_back(color[1]);
		varying.push_back(color[2]);
	}
	else if (canvas->shade_model == canvashdl::phong)
    {
		for (int i = 0; i < 3; i++)
			varying.push_back(vertex[i]);
        for (int i = 0; i < 3; i++)
            varying.push_back(normal[i]);
    }

	eye_space_vertex = canvas->matrices[canvashdl::projection_matrix]*eye_space_vertex;
	eye_space_vertex /= eye_space_vertex[3];
	return eye_space_vertex;
}

vec3f solidhdl::shade_fragment(canvashdl *canvas, vector<float> &varying) const
{
	if (canvas->shade_model == canvashdl::flat || canvas->shade_model == canvashdl::gouraud)
		return vec3f(varying[0], varying[1], varying[2]);
	if (canvas->shade_model == canvashdl::phong)
	{
		vec3f vertex = vec3f(varying[0], varying[1], varying[2]);
		vec3f normal = vec3f(varying[3], varying[4], varying[5]);

		varying.erase(varying.begin(), varying.begin() + 6);

		vec3f light_ambient(0.0, 0.0, 0.0);
		vec3f light_diffuse(0.0, 0.0, 0.0);
		vec3f light_specular(0.0, 0.0, 0.0);

		const vector<lighthdl*> *lights;
		canvas->get_uniform("lights", lights);

		vec3f eye_coord_vertex = canvas->matrices[canvashdl::modelview_matrix]*homogenize(vertex);
		vec3f eye_coord_normal = canvas->matrices[canvashdl::normal_matrix]*(vec4f)normal;

		float m = mag2(eye_coord_normal);

		if (m > 0.0)
		{
			eye_coord_normal /= sqrt(m);
			for (int j = 0; j < lights->size(); j++)
				if (lights->at(j) != NULL)
                    lights->at(j)->shade(light_ambient, light_diffuse, light_specular, eye_coord_vertex, eye_coord_normal, shininess);
            
			return clamp(emission + ambient*light_ambient + diffuse*light_diffuse + specular*light_specular, 0.0f, 1.0f);
		}
	}
	else if (canvas->shade_model == canvashdl::none)
		return clamp(emission + ambient + diffuse, 0.0f, 1.0f);

	return vec3f(1.0, 1.0, 1.0);
}

materialhdl *solidhdl::clone() const
{
	solidhdl *result = new solidhdl();
	result->type = type;
	result->emission = emission;
	result->ambient = ambient;
	result->diffuse = diffuse;
	result->specular = specular;
	result->shininess = shininess;
	return result;
}

nonsolidhdl::nonsolidhdl()
{
	type = "non_uniform";
}

nonsolidhdl::~nonsolidhdl()
{

}

vec3f nonsolidhdl::shade_vertex(canvashdl *canvas, vec3f vertex, vec3f normal, vector<float> &varying) const
{
	vec4f eye_space_vertex = canvas->matrices[canvashdl::modelview_matrix]*homogenize(vertex);
	float mult = (float)(fmod(fabs(vertex[1] + 1000.0)*3.0, 1.0) > 0.5);
	vec3f scale(mult, 0.0f, 1.0-mult);

	if (canvas->shade_model == canvashdl::flat || canvas->shade_model == canvashdl::gouraud)
	{
		const vector<lighthdl*> *lights;

		canvas->get_uniform("lights", lights);

		vec3f color(1.0, 1.0, 1.0);
		if (mag2(normal) > 0.0)
		{
			vec3f eye_space_normal = canvas->matrices[canvashdl::normal_matrix]*(vec4f)normal;

			vec3f light_ambient(0.0, 0.0, 0.0);
			vec3f light_diffuse(0.0, 0.0, 0.0);
			vec3f light_specular(0.0, 0.0, 0.0);

			for (int j = 0; j < lights->size(); j++)
				if (lights->at(j) != NULL)
					lights->at(j)->shade(light_ambient, light_diffuse, light_specular, eye_space_vertex, eye_space_normal, 5.0);

			color = clamp(scale*light_ambient + scale*light_diffuse + scale*light_specular, 0.0f, 1.0f);
		}

		varying.push_back(color[0]);
		varying.push_back(color[1]);
		varying.push_back(color[2]);
	}
	else if (canvas->shade_model == canvashdl::phong)
		for (int i = 0; i < 6; i++)
			varying.push_back(vertex[i]);
	else if (canvas->shade_model == canvashdl::none)
		varying.push_back(vertex[1]);

	eye_space_vertex = canvas->matrices[canvashdl::projection_matrix]*eye_space_vertex;
	eye_space_vertex /= eye_space_vertex[3];
	return eye_space_vertex;
}

vec3f nonsolidhdl::shade_fragment(canvashdl *canvas, vector<float> &varying) const
{
	if (canvas->shade_model == canvashdl::flat || canvas->shade_model == canvashdl::gouraud)
		return vec3f(varying[0], varying[1], varying[2]);
	if (canvas->shade_model == canvashdl::phong)
	{
		vec3f vertex = vec3f(varying[0], varying[1], varying[2]);
		vec3f normal = vec3f(varying[3], varying[4], varying[5]);

		varying.erase(varying.begin(), varying.begin() + 6);

		float mult = (float)(fmod(fabs(vertex[1] + 1000.0)*3.0, 1.0) > 0.5);
		vec3f scale(mult, 0.0f, 1.0-mult);

		vec3f light_ambient(0.0, 0.0, 0.0);
		vec3f light_diffuse(0.0, 0.0, 0.0);
		vec3f light_specular(0.0, 0.0, 0.0);

		const vector<lighthdl*> *lights;
		canvas->get_uniform("lights", lights);

		vec3f eye_coord_vertex = canvas->matrices[canvashdl::modelview_matrix]*homogenize(vertex);
		vec3f eye_coord_normal = canvas->matrices[canvashdl::normal_matrix]*(vec4f)normal;

		float m = mag2(eye_coord_normal);

		if (m > 0.0)
		{
			eye_coord_normal /= sqrt(m);
			for (int j = 0; j < lights->size(); j++)
				if (lights->at(j) != NULL)
					lights->at(j)->shade(light_ambient, light_diffuse, light_specular, eye_coord_vertex, eye_coord_normal, 5.0);

			return clamp(scale*light_ambient + scale*light_diffuse + scale*light_specular, 0.0f, 1.0f);
		}
	}
	else if (canvas->shade_model == canvashdl::none)
	{
		float mult = (float)(fmod(fabs(varying[0] + 1000.0)*3.0, 1.0) > 0.5);
		return clamp(vec3f(mult, 0.0f, 1.0-mult), 0.0f, 1.0f);
	}

	return vec3f(1.0, 1.0, 1.0);
}

materialhdl *nonsolidhdl::clone() const
{
	nonsolidhdl *result = new nonsolidhdl();
	result->type = type;
	return result;
}
