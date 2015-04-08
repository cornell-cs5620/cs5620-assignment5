/*
 * material.cpp
 *
 *  Created on: Dec 18, 2014
 *      Author: nbingham
 */

#include "material.h"
#include "light.h"

GLuint whitehdl::vertex = 0;
GLuint whitehdl::fragment = 0;
GLuint whitehdl::program = 0;

GLuint solidhdl::vertex = 0;
GLuint solidhdl::fragment = 0;
GLuint solidhdl::program = 0;

GLuint brickhdl::vertex = 0;
GLuint brickhdl::fragment = 0;
GLuint brickhdl::program = 0;

extern string working_directory;

materialhdl::materialhdl()
{
	type = "material";
}

materialhdl::~materialhdl()
{
}

solidhdl::solidhdl()
{
	type = "solid";
	emission = vec3f(0.0, 0.0, 0.0);
	ambient = vec3f(0.1, 0.1, 0.1);
	diffuse = vec3f(1.0, 1.0, 1.0);
	specular = vec3f(1.0, 1.0, 1.0);
	shininess = 1.0;

	if (vertex == 0 && fragment == 0 && program == 0)
	{
		program = glCreateProgram();
		vertex = load_shader_file(working_directory + "res/"+ type + ".vx", GL_VERTEX_SHADER);
		fragment = load_shader_file(working_directory + "res/"+ type + ".ft", GL_FRAGMENT_SHADER);
		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glLinkProgram(program);
	}
}

solidhdl::~solidhdl()
{

}

void solidhdl::apply(const vector<lighthdl*> &lights)
{
	glUseProgram(program);
	glUniform3fv(glGetUniformLocation(program, "emission"), 1, emission.data);
	glUniform3fv(glGetUniformLocation(program, "ambient"), 1, ambient.data);
	glUniform3fv(glGetUniformLocation(program, "diffuse"), 1, diffuse.data);
	glUniform3fv(glGetUniformLocation(program, "specular"), 1, specular.data);
	glUniform1f(glGetUniformLocation(program, "shininess"), shininess);

	int d = 0, p = 0, s = 0;
	for (int i = 0; i < (int)lights.size(); i++)
		if (lights[i] != NULL)
		{
			if (lights[i]->type[0] == 'd')
			{
				lights[i]->apply(string(1, lights[i]->type[0]) + "lights[" + to_string(d) + "]", program);
				d++;
			}
			else if (lights[i]->type[0] == 'p')
			{
				lights[i]->apply(string(1, lights[i]->type[0]) + "lights[" + to_string(p) + "]", program);
				p++;
			}
			else if (lights[i]->type[0] == 's')
			{
				lights[i]->apply(string(1, lights[i]->type[0]) + "lights[" + to_string(s) + "]", program);
				s++;
			}
		}

	glUniform1i(glGetUniformLocation(program, "num_dlights"), d);
	glUniform1i(glGetUniformLocation(program, "num_plights"), p);
	glUniform1i(glGetUniformLocation(program, "num_slights"), s);
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

whitehdl::whitehdl()
{
	type = "white";

	if (vertex == 0 && fragment == 0 && program == 0)
	{
		program = glCreateProgram();
		vertex = load_shader_file(working_directory + "res/"+ type + ".vx", GL_VERTEX_SHADER);
		fragment = load_shader_file(working_directory + "res/"+ type + ".ft", GL_FRAGMENT_SHADER);
		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glLinkProgram(program);
	}
}

whitehdl::~whitehdl()
{

}

void whitehdl::apply(const vector<lighthdl*> &lights)
{
	glUseProgram(program);
}

materialhdl *whitehdl::clone() const
{
	whitehdl *result = new whitehdl();
	result->type = type;
	return result;
}

brickhdl::brickhdl()
{
	type = "brick";

	if (vertex == 0 && fragment == 0 && program == 0)
	{
		program = glCreateProgram();
		vertex = load_shader_file(working_directory + "res/"+ type + ".vx", GL_VERTEX_SHADER);
		fragment = load_shader_file(working_directory + "res/"+ type + ".ft", GL_FRAGMENT_SHADER);
		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glLinkProgram(program);
	}
}

brickhdl::~brickhdl()
{

}

void brickhdl::apply(const vector<lighthdl*> &lights)
{
	glUseProgram(program);

	int d = 0, p = 0, s = 0;
	for (int i = 0; i < (int)lights.size(); i++)
		if (lights[i] != NULL)
		{
			if (lights[i]->type[0] == 'd')
			{
				lights[i]->apply(string(1, lights[i]->type[0]) + "lights[" + to_string(d) + "]", program);
				d++;
			}
			else if (lights[i]->type[0] == 'p')
			{
				lights[i]->apply(string(1, lights[i]->type[0]) + "lights[" + to_string(p) + "]", program);
				p++;
			}
			else if (lights[i]->type[0] == 's')
			{
				lights[i]->apply(string(1, lights[i]->type[0]) + "lights[" + to_string(s) + "]", program);
				s++;
			}
		}

	glUniform1i(glGetUniformLocation(program, "num_dlights"), d);
	glUniform1i(glGetUniformLocation(program, "num_plights"), p);
	glUniform1i(glGetUniformLocation(program, "num_slights"), s);
}

materialhdl *brickhdl::clone() const
{
	brickhdl *result = new brickhdl();
	result->type = type;
	return result;
}
