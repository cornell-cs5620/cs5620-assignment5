/*
 * object.cpp
 *
 *  Created on: Jan 2, 2015
 *      Author: nbingham
 */

#include "object.h"

rigidhdl::rigidhdl()
{

}

rigidhdl::~rigidhdl()
{

}

/* draw
 *
 * Draw a rigid body.
 */
void rigidhdl::draw(float animation_time)
{
	map<float, vec3f>::iterator position_frame = positions.lower_bound(animation_time);
	map<float, vec3f>::iterator orientation_frame = orientations.lower_bound(animation_time);

	glPushMatrix();
	glTranslatef(position_frame->second[0], position_frame->second[1], position_frame->second[2]);
	glRotatef(radtodeg(orientation_frame->second[0]), 1.0, 0.0, 0.0);
	glRotatef(radtodeg(orientation_frame->second[1]), 0.0, 1.0, 0.0);
	glRotatef(radtodeg(orientation_frame->second[2]), 0.0, 0.0, 1.0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(float)*8, ((float*)geometry.data()));
	glNormalPointer(GL_FLOAT, sizeof(float)*8, ((float*)geometry.data()) + 3);
	glTexCoordPointer(2, GL_FLOAT, sizeof(float)*8, ((float*)geometry.data()) + 6);

	glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, indices.data());

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glPopMatrix();
}

objecthdl::objecthdl()
{
	position = vec3f(0.0, 0.0, 0.0);
	orientation = vec3f(0.0, 0.0, 0.0);
	bound = vec6f(1.0e6, -1.0e6, 1.0e6, -1.0e6, 1.0e6, -1.0e6);
	scale = 1.0;
}

objecthdl::objecthdl(const objecthdl &o)
{
	position = o.position;
	orientation = o.orientation;
	bound = o.bound;
	scale = o.scale;
	rigid = o.rigid;
	for (map<string, materialhdl*>::const_iterator i = o.material.begin(); i != o.material.end(); i++)
		material.insert(pair<string, materialhdl*>(i->first, i->second->clone()));
}

objecthdl::~objecthdl()
{
	for (map<string, materialhdl*>::iterator i = material.begin(); i != material.end(); i++)
		if (i->second != NULL)
		{
			delete i->second;
			i->second = NULL;
		}

	material.clear();
}

/* draw
 *
 * Draw the model. Don't forget to apply the transformations necessary
 * for position, orientation, and scale.
 */
void objecthdl::draw(const vector<lighthdl*> &lights, float animation_time)
{
	glPushMatrix();
	glTranslatef(position[0], position[1], position[2]);
	glRotatef(radtodeg(orientation[0]), 1.0, 0.0, 0.0);
	glRotatef(radtodeg(orientation[1]), 0.0, 1.0, 0.0);
	glRotatef(radtodeg(orientation[2]), 0.0, 0.0, 1.0);
	glScalef(scale, scale, scale);

	for (int i = 0; i < rigid.size(); i++)
	{
		if (material.find(rigid[i].material) != material.end())
			material[rigid[i].material]->apply(lights);
		else
			whitehdl().apply(lights);
		rigid[i].draw(animation_time);
	}

	glPopMatrix();
}

/* draw_bound
 *
 * Create a representation for the bounding box and
 * render it.
 */
void objecthdl::draw_bound()
{
	glPushMatrix();
	glTranslatef(position[0], position[1], position[2]);
	glRotatef(radtodeg(orientation[0]), 1.0, 0.0, 0.0);
	glRotatef(radtodeg(orientation[1]), 0.0, 1.0, 0.0);
	glRotatef(radtodeg(orientation[2]), 0.0, 0.0, 1.0);
	glScalef(scale, scale, scale);

	bound[0] -= 0.005;
	bound[1] += 0.005;
	bound[2] -= 0.005;
	bound[3] += 0.005;
	bound[4] -= 0.005;
	bound[5] += 0.005;

	vector<vec8f> bound_geometry;
	vector<int> bound_indices;
	bound_geometry.reserve(8);
	bound_geometry.push_back(vec8f(bound[0], bound[2], bound[4], 0.0, 0.0, 0.0, 0.0, 0.0));
	bound_geometry.push_back(vec8f(bound[1], bound[2], bound[4], 0.0, 0.0, 0.0, 0.0, 0.0));
	bound_geometry.push_back(vec8f(bound[1], bound[3], bound[4], 0.0, 0.0, 0.0, 0.0, 0.0));
	bound_geometry.push_back(vec8f(bound[0], bound[3], bound[4], 0.0, 0.0, 0.0, 0.0, 0.0));
	bound_geometry.push_back(vec8f(bound[0], bound[2], bound[5], 0.0, 0.0, 0.0, 0.0, 0.0));
	bound_geometry.push_back(vec8f(bound[1], bound[2], bound[5], 0.0, 0.0, 0.0, 0.0, 0.0));
	bound_geometry.push_back(vec8f(bound[1], bound[3], bound[5], 0.0, 0.0, 0.0, 0.0, 0.0));
	bound_geometry.push_back(vec8f(bound[0], bound[3], bound[5], 0.0, 0.0, 0.0, 0.0, 0.0));
	bound_indices.reserve(24);
	for (int i = 0; i < 4; i++)
	{
		bound_indices.push_back(i);
		bound_indices.push_back((i+1)%4);
		bound_indices.push_back(4+i);
		bound_indices.push_back(4+(i+1)%4);
		bound_indices.push_back(i);
		bound_indices.push_back(4+i);
	}

	whitehdl().apply(vector<lighthdl*>());

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(float)*8, ((float*)bound_geometry.data()));
	glNormalPointer(GL_FLOAT, sizeof(float)*8, ((float*)bound_geometry.data()) + 3);
	glTexCoordPointer(2, GL_FLOAT, sizeof(float)*8, ((float*)bound_geometry.data()) + 6);

	glDrawElements(GL_LINES, (int)bound_indices.size(), GL_UNSIGNED_INT, bound_indices.data());

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glPopMatrix();

	bound[0] += 0.005;
	bound[1] -= 0.005;
	bound[2] += 0.005;
	bound[3] -= 0.005;
	bound[4] += 0.005;
	bound[5] -= 0.005;
}

/* draw_normals
 *
 * create a representation of the normals for this object.
 * If face is false, render the vertex normals. Otherwise,
 * calculate the normals for each face and render those.
 */
void objecthdl::draw_normals(bool face)
{
	float radius = 0.0;
	for (int i = 0; i < 6; i++)
		if (abs(bound[i]) > radius)
			radius = abs(bound[i]);

	vector<vec8f> normal_geometry;
	vector<int> normal_indices;

	glPushMatrix();
	glTranslatef(position[0], position[1], position[2]);
	glRotatef(radtodeg(orientation[0]), 1.0, 0.0, 0.0);
	glRotatef(radtodeg(orientation[1]), 0.0, 1.0, 0.0);
	glRotatef(radtodeg(orientation[2]), 0.0, 0.0, 1.0);
	glScalef(scale, scale, scale);

	for (int i = 0; i < rigid.size(); i++)
	{
		if (!face)
		{
			for (int j = 0; j < rigid[i].geometry.size(); j++)
			{
				normal_indices.push_back(normal_geometry.size());
				normal_geometry.push_back(rigid[i].geometry[j]);
				normal_geometry.back().set(3,6,vec3f(0.0, 0.0, 0.0));
				normal_indices.push_back(normal_geometry.size());
				normal_geometry.push_back(rigid[i].geometry[j]);
				normal_geometry.back().set(0,3,(vec3f)(normal_geometry.back()(0,3) + radius*0.1f*normal_geometry.back()(3,6)));
				normal_geometry.back().set(3,6,vec3f(0.0, 0.0, 0.0));
			}
		}
		else
		{
			for (int j = 0; j < rigid[i].indices.size(); j+=3)
			{
				vec3f normal = norm((vec3f)rigid[i].geometry[rigid[i].indices[j + 0]](3,6) +
									(vec3f)rigid[i].geometry[rigid[i].indices[j + 1]](3,6) +
									(vec3f)rigid[i].geometry[rigid[i].indices[j + 2]](3,6));
				vec3f center = ((vec3f)rigid[i].geometry[rigid[i].indices[j + 0]](0,3) +
								(vec3f)rigid[i].geometry[rigid[i].indices[j + 1]](0,3) +
								(vec3f)rigid[i].geometry[rigid[i].indices[j + 2]](0,3))/3.0f;
				normal_indices.push_back(normal_geometry.size());
				normal_geometry.push_back(center);
				normal_geometry.back().set(3,8,vec5f(0.0, 0.0, 0.0, 0.0, 0.0));
				normal_indices.push_back(normal_geometry.size());
				normal_geometry.push_back(center + radius*0.1f*normal);
				normal_geometry.back().set(3,8,vec5f(0.0, 0.0, 0.0, 0.0, 0.0));
			}
		}

		whitehdl().apply(vector<lighthdl*>());

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, sizeof(float)*8, ((float*)normal_geometry.data()));
		glNormalPointer(GL_FLOAT, sizeof(float)*8, ((float*)normal_geometry.data()) + 3);
		glTexCoordPointer(2, GL_FLOAT, sizeof(float)*8, ((float*)normal_geometry.data()) + 6);

		glDrawElements(GL_LINES, (int)normal_indices.size(), GL_UNSIGNED_INT, normal_indices.data());

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		normal_geometry.clear();
		normal_indices.clear();
	}

	glPopMatrix();
}
