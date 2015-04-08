/*
 * light.cpp
 *
 *  Created on: Dec 17, 2014
 *      Author: nbingham
 */

#include "light.h"
#include "object.h"
#include "opengl.h"

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

void directionalhdl::update()
{
	if (model != NULL)
	{
		glPushMatrix();
		glTranslatef(model->position[0], model->position[1], model->position[2]);
		glRotatef(radtodeg(model->orientation[0]), 1.0, 0.0, 0.0);
		glRotatef(radtodeg(model->orientation[1]), 0.0, 1.0, 0.0);
		glRotatef(radtodeg(model->orientation[2]), 0.0, 0.0, 1.0);

		mat4f normal;
		glGetFloatv(GL_TRANSPOSE_MODELVIEW_MATRIX, (float*)normal.data);
		normal = transpose(inverse(normal));
		direction = normal*vec4f(0.0, 0.0, -1.0, 0.0);

		glPopMatrix();
	}
}

void directionalhdl::apply(string name, GLuint program)
{
	glUniform3fv(glGetUniformLocation(program, (name + ".ambient").c_str()), 1, ambient.data);
	glUniform3fv(glGetUniformLocation(program, (name + ".diffuse").c_str()), 1, diffuse.data);
	glUniform3fv(glGetUniformLocation(program, (name + ".specular").c_str()), 1, specular.data);
	glUniform3fv(glGetUniformLocation(program, (name + ".direction").c_str()), 1, direction.data);
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

void pointhdl::update()
{
	if (model != NULL)
	{
		glPushMatrix();
		glTranslatef(model->position[0], model->position[1], model->position[2]);
		glRotatef(radtodeg(model->orientation[0]), 1.0, 0.0, 0.0);
		glRotatef(radtodeg(model->orientation[1]), 0.0, 1.0, 0.0);
		glRotatef(radtodeg(model->orientation[2]), 0.0, 0.0, 1.0);

		mat4f modelview;
		glGetFloatv(GL_TRANSPOSE_MODELVIEW_MATRIX, (float*)modelview.data);
		vec4f p = modelview*vec4f(0.0, 0.0, 0.0, 1.0);
		position = p(0,3)/p[3];

		glPopMatrix();
	}
}

void pointhdl::apply(string name, GLuint program)
{
	glUniform3fv(glGetUniformLocation(program, (name + ".ambient").c_str()), 1, ambient.data);
	glUniform3fv(glGetUniformLocation(program, (name + ".diffuse").c_str()), 1, diffuse.data);
	glUniform3fv(glGetUniformLocation(program, (name + ".specular").c_str()), 1, specular.data);
	glUniform3fv(glGetUniformLocation(program, (name + ".position").c_str()), 1, position.data);
	glUniform3fv(glGetUniformLocation(program, (name + ".attenuation").c_str()), 1, attenuation.data);
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

void spothdl::update()
{
	if (model != NULL)
	{
		glPushMatrix();
		glTranslatef(model->position[0], model->position[1], model->position[2]);
		glRotatef(radtodeg(model->orientation[0]), 1.0, 0.0, 0.0);
		glRotatef(radtodeg(model->orientation[1]), 0.0, 1.0, 0.0);
		glRotatef(radtodeg(model->orientation[2]), 0.0, 0.0, 1.0);

		mat4f normal, modelview;
		glGetFloatv(GL_TRANSPOSE_MODELVIEW_MATRIX, (float*)modelview.data);
		normal = transpose(inverse(modelview));

		vec4f p = modelview*vec4f(0.0, 0.0, 0.0, 1.0);
		position = p(0,3)/p[3];
		direction = normal*vec4f(0.0, 0.0, -1.0, 0.0);

		glPopMatrix();
	}
}

void spothdl::apply(string name, GLuint program)
{
	glUniform3fv(glGetUniformLocation(program, (name + ".ambient").c_str()), 1, ambient.data);
	glUniform3fv(glGetUniformLocation(program, (name + ".diffuse").c_str()), 1, diffuse.data);
	glUniform3fv(glGetUniformLocation(program, (name + ".specular").c_str()), 1, specular.data);
	glUniform3fv(glGetUniformLocation(program, (name + ".direction").c_str()), 1, direction.data);
	glUniform3fv(glGetUniformLocation(program, (name + ".position").c_str()), 1, position.data);
	glUniform3fv(glGetUniformLocation(program, (name + ".attenuation").c_str()), 1, attenuation.data);
	glUniform1f(glGetUniformLocation(program, (name + ".cutoff").c_str()), cutoff);
	glUniform1f(glGetUniformLocation(program, (name + ".exponent").c_str()), exponent);
}
