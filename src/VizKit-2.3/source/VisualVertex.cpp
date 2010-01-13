/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualVertex.cpp
 *
 */

/***************************************************************************

Copyright (c) 2004-2009 Heiko Wichmann (http://www.imagomat.de/vizkit)


This software is provided 'as-is', without any expressed or implied warranty. 
In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; 
   you must not claim that you wrote the original software. 
   If you use this software in a product, an acknowledgment 
   in the product documentation would be appreciated 
   but is not required.

2. Altered source versions must be plainly marked as such, 
   and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.

 ***************************************************************************/

#include "VisualVertex.h"



using namespace VizKit;


VisualVertex::VisualVertex(Coord coordPoint) {
	vertexPosition.coord.x = coordPoint.x;
	vertexPosition.coord.y = coordPoint.y;
	vertexPosition.coord.z = coordPoint.z;
	vertexPosition.relCoord.x = -1.0;
	vertexPosition.relCoord.y = -1.0;
	vertexPosition.relCoord.z = -1.0;

	vertexColor = black;
	colorIsSet = false;
	
	texCoordPosition.coord.s = 0.0;
	texCoordPosition.coord.t = 0.0;
	texCoordPosition.relCoord.s = -1.0;
	texCoordPosition.relCoord.t = -1.0;	
}


VisualVertex::VisualVertex(Coord coordPoint, VertexColor anRGBAColor) {
	vertexPosition.coord.x = coordPoint.x;
	vertexPosition.coord.y = coordPoint.y;
	vertexPosition.coord.z = coordPoint.z;
	vertexPosition.relCoord.x = -1.0;
	vertexPosition.relCoord.y = -1.0;
	vertexPosition.relCoord.z = -1.0;

	vertexColor = anRGBAColor;
	colorIsSet = true;
	
	texCoordPosition.coord.s = 0.0;
	texCoordPosition.coord.t = 0.0;
	texCoordPosition.relCoord.s = -1.0;
	texCoordPosition.relCoord.t = -1.0;
}


VisualVertex::VisualVertex(Coord coordPoint, Coord relativeCoordPoint, VertexColor anRGBAColor) {

	vertexPosition.coord.x = coordPoint.x;
	vertexPosition.coord.y = coordPoint.y;
	vertexPosition.coord.z = coordPoint.z;
	vertexPosition.relCoord.x = relativeCoordPoint.x;
	vertexPosition.relCoord.y = relativeCoordPoint.y;
	vertexPosition.relCoord.z = relativeCoordPoint.z;
	
	vertexColor = anRGBAColor;
	colorIsSet = true;

	texCoordPosition.coord.s = 0.0;
	texCoordPosition.coord.t = 0.0;
	texCoordPosition.relCoord.s = -1.0;
	texCoordPosition.relCoord.t = -1.0;
}


VisualVertex::VisualVertex(Coord coordPoint, TexCoord texCoordPoint, VertexColor anRGBAColor) {

	vertexPosition.coord.x = coordPoint.x;
	vertexPosition.coord.y = coordPoint.y;
	vertexPosition.coord.z = coordPoint.z;
	vertexPosition.relCoord.x = -1.0;
	vertexPosition.relCoord.y = -1.0;
	vertexPosition.relCoord.z = -1.0;
	
	vertexColor = anRGBAColor;
	colorIsSet = true;

	texCoordPosition.coord.s = texCoordPoint.s;
	texCoordPosition.coord.t = texCoordPoint.t;
	texCoordPosition.relCoord.s = -1.0;
	texCoordPosition.relCoord.t = -1.0;
}


VisualVertex::VisualVertex(Coord coordPoint, Coord relativeCoordPoint, TexCoord texCoordPoint, VertexColor anRGBAColor) {

	vertexPosition.coord.x = coordPoint.x;
	vertexPosition.coord.y = coordPoint.y;
	vertexPosition.coord.z = coordPoint.z;
	vertexPosition.relCoord.x = relativeCoordPoint.x;
	vertexPosition.relCoord.y = relativeCoordPoint.y;
	vertexPosition.relCoord.z = relativeCoordPoint.z;
	
	vertexColor = anRGBAColor;
	colorIsSet = true;

	texCoordPosition.coord.s = texCoordPoint.s;
	texCoordPosition.coord.t = texCoordPoint.t;
	texCoordPosition.relCoord.s = -1.0;
	texCoordPosition.relCoord.t = -1.0;
}


VisualVertex::VisualVertex(Coord coordPoint, Coord relativeCoordPoint, TexCoord texCoordPoint, TexCoord relTexCoordPoint, VertexColor anRGBAColor) {

	vertexPosition.coord.x = coordPoint.x;
	vertexPosition.coord.y = coordPoint.y;
	vertexPosition.coord.z = coordPoint.z;
	vertexPosition.relCoord.x = relativeCoordPoint.x;
	vertexPosition.relCoord.y = relativeCoordPoint.y;
	vertexPosition.relCoord.z = relativeCoordPoint.z;
	
	vertexColor = anRGBAColor;
	colorIsSet = true;

	texCoordPosition.coord.s = texCoordPoint.s;
	texCoordPosition.coord.t = texCoordPoint.t;
	texCoordPosition.relCoord.s = relTexCoordPoint.s;
	texCoordPosition.relCoord.t = relTexCoordPoint.t;

}


VisualVertex::VisualVertex(VertexPosition aVertexPosition, VertexColor anRGBAColor) {
	vertexPosition = aVertexPosition;
	
	vertexColor = anRGBAColor;
	colorIsSet = true;
	
	texCoordPosition.coord.s = 0.0;
	texCoordPosition.coord.t = 0.0;
	texCoordPosition.relCoord.s = -1.0;
	texCoordPosition.relCoord.t = -1.0;
	
}


VisualVertex::~VisualVertex() {
	// NULL
}


VisualVertex::VisualVertex(const VisualVertex& other) {
	copy(other);
}


VisualVertex& VisualVertex::operator=(const VisualVertex& other) {
	if (this != &other) {
		this->copy(other);
	}
	return *this;
}


void VisualVertex::copy(const VisualVertex& other) {
	this->vertexColor = other.vertexColor;
	this->vertexPosition = other.vertexPosition;
	this->texCoordPosition = other.texCoordPosition;
	this->colorIsSet = other.colorIsSet;
}


void VisualVertex::setRelativeVertexPosition(Coord coordPoint) {
	this->vertexPosition.relCoord.x = coordPoint.x;
	this->vertexPosition.relCoord.y = coordPoint.y;
	this->vertexPosition.relCoord.z = coordPoint.z;
}


void VisualVertex::setRelativeTexCoordPosition(TexCoord aTexCoord) {
	this->texCoordPosition.relCoord.s = aTexCoord.s;
	this->texCoordPosition.relCoord.t = aTexCoord.t;
}


bool VisualVertex::colorHasBeenSet() {
	return colorIsSet;
}
