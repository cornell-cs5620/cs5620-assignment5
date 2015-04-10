/*
 * material.cpp
 *
 *  Created on: Dec 18, 2014
 *      Author: nbingham
 */

#include "material.h"
#include "light.h"
#include "lodepng.h"

GLuint whitehdl::vertex = 0;
GLuint whitehdl::fragment = 0;
GLuint whitehdl::program = 0;

GLuint solidhdl::vertex = 0;
GLuint solidhdl::fragment = 0;
GLuint solidhdl::program = 0;

GLuint brickhdl::vertex = 0;
GLuint brickhdl::fragment = 0;
GLuint brickhdl::program = 0;

GLuint texturehdl::vertex = 0;
GLuint texturehdl::fragment = 0;
GLuint texturehdl::program = 0;
GLuint texturehdl::texture = 0;

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

texturehdl::texturehdl()
{
	type = "texture";

	shininess = 1.0;

	if (vertex == 0 && fragment == 0 && program == 0)
	{
		glEnable(GL_TEXTURE_2D);
		program = glCreateProgram();
		vertex = load_shader_file(working_directory + "res/"+ type + ".vx", GL_VERTEX_SHADER);
		fragment = load_shader_file(working_directory + "res/"+ type + ".ft", GL_FRAGMENT_SHADER);
		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glLinkProgram(program);

		unsigned error;
		unsigned char* image;
		unsigned int width, height;
		const char* filename = (working_directory + "res/texture.png").c_str();

		error = lodepng_decode32_file(&image, &width, &height, filename);

		if (error)
			printf("decoder error %u: %s\n", error, lodepng_error_text(error));

		 // Create one OpenGL texture
		glGenTextures(1, &texture);

		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, texture);

		// Nice trilinear filtering.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		free(image);
	}
}

texturehdl::~texturehdl()
{

}

void texturehdl::apply(const vector<lighthdl*> &lights)
{
	glUseProgram(program);

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

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(program, "tex"), 0);
}

materialhdl *texturehdl::clone() const
{
	texturehdl *result = new texturehdl();
	result->type = type;
	result->shininess = shininess;
	return result;
}
