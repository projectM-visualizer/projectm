/*
 * Project: VizKit
 * Version: 2.3
 
 * Date: 20090823
 * File: VisualNurbs.cpp
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

#include "VisualNurbs.h"
#include "VisualErrorHandling.h"
#include "VisualGraphicsCore.h"
#include "VisualGraphics.h"



using namespace VizKit;


#ifdef __cplusplus
extern "C" {
#endif
	GLvoid nurbsError(GLenum errorCode);
#ifdef __cplusplus
}
#endif

GLvoid nurbsError(GLenum errorCode) {
	const GLubyte* estring;
	estring = gluErrorString(errorCode);
	writeLog((char*)estring);
}


VisualNurbs::VisualNurbs(uint8 sKnotsCount, float* sKnotsPointer, uint8 tKnotsCount, float* tKnotsPointer, uint16 s_Stride, uint16 t_Stride, uint16 s_Order, uint16 t_Order) {

	sNumKnots = sKnotsCount;
	sKnots = (GLfloat*)malloc(sKnotsCount * sizeof(GLfloat));
	for (int i = 0; i < sKnotsCount; i++) {
		sKnots[i] = sKnotsPointer[i];
	}
	tNumKnots = tKnotsCount;
	tKnots = (GLfloat*)malloc(tKnotsCount * sizeof(GLfloat));
	for (int i = 0; i < tKnotsCount; i++) {
		tKnots[i] = tKnotsPointer[i];
	}
	
	sStride = s_Stride;
	tStride = t_Stride;
	sOrder = s_Order;
	tOrder = t_Order;
	
	nurbsObject = gluNewNurbsRenderer();

	controlPointsVisibility = kInvisible;
	controlPoints = NULL;
	nurbsTexture = NULL;
	
}


VisualNurbs::~VisualNurbs() {
	if (nurbsTexture != NULL) {
		delete nurbsTexture;
	}
	if (sKnots != NULL) {
		free(sKnots);
	}
	if (tKnots != NULL) {
		free(tKnots);
	}
	if (controlPoints != NULL) {
		free(controlPoints);
	}
	gluDeleteNurbsRenderer(nurbsObject);
}


VisualNurbs::VisualNurbs(const VisualNurbs& other) {
	copy(other);
}


VisualNurbs& VisualNurbs::operator=(const VisualNurbs& other) {
	
	if (this == &other) return *this;
	
	if (this->sKnots != NULL) {
		free(this->sKnots);
	}
	if (this->tKnots != NULL) {
		free(this->tKnots);
	}
	if (this->nurbsObject) {
		gluDeleteNurbsRenderer(this->nurbsObject);
	}
	if (this->controlPoints != NULL) {
		free(this->controlPoints);
	}

	this->copy(other);

	return *this;
}


void VisualNurbs::copy(const VisualNurbs& other) {
	this->sNumKnots = other.sNumKnots;
	if (other.sKnots != NULL) {
		this->sKnots = (GLfloat*)malloc(this->sNumKnots * sizeof(GLfloat));
		for (int i = 0; i < this->sNumKnots; i++) {
			this->sKnots[i] = other.sKnots[i];
		}
	} else {
		this->sKnots = NULL;
	}
	this->tNumKnots = other.tNumKnots;
	if (other.tKnots != NULL) {
		this->tKnots = (GLfloat*)malloc(this->tNumKnots * sizeof(GLfloat));
		for (int i = 0; i < this->tNumKnots; i++) {
			this->tKnots[i] = other.tKnots[i];
		}
	} else {
		this->tKnots = NULL;
	}
	
	this->sStride = other.sStride;
	this->tStride = other.tStride;
	this->sOrder = other.sOrder;
	this->tOrder = other.tOrder;
	
	this->nurbsObject = gluNewNurbsRenderer();
	
	this->controlPointsVisibility = other.controlPointsVisibility;
	uint16 numberOfControlPoints = (this->sNumKnots - this->sOrder) * (this->tNumKnots - this->tOrder);
	this->controlPoints = (GLfloat*)malloc(numberOfControlPoints * sizeof(GLfloat));
	for (int i = 0; i < numberOfControlPoints; i++) {
		this->controlPoints[i] = other.controlPoints[i];
	}
	if (other.nurbsTexture) {
		this->nurbsTexture = new NurbsTexture(*(other.nurbsTexture));
	} else {
		this->nurbsTexture = NULL;
	}
}


VisualNurbs::NurbsTexture::NurbsTexture() {
	sNumKnots = 0;
	sKnots = NULL;
	tNumKnots = 0;
	tKnots = NULL;
	sStride = 0;
	tStride = 0;
	sOrder = 0;
	tOrder = 0;
	textureName = 0;
	canUseRectExtension = false;
	texturePoints = NULL;
}


VisualNurbs::NurbsTexture::~NurbsTexture() {
	free(sKnots);
	free(tKnots);
	if (texturePoints) {
		free(texturePoints);
	}
}


VisualNurbs::NurbsTexture::NurbsTexture(const NurbsTexture& other) {
	copy(other);
}


VisualNurbs::NurbsTexture& VisualNurbs::NurbsTexture::operator=(const VisualNurbs::NurbsTexture& other) {
	
	if (this == &other) return *this;
	
	if (this->sKnots != NULL) {
		free(this->sKnots);
	}
	if (this->tKnots != NULL) {
		free(this->tKnots);
	}
	if (this->texturePoints != NULL) {
		free(this->texturePoints);
	}
	
	this->copy(other);

	return *this;
}


void VisualNurbs::NurbsTexture::copy(const VisualNurbs::NurbsTexture& other) {
	this->sNumKnots = other.sNumKnots;
	if (other.sKnots != NULL) {
		this->sKnots = (GLfloat*)malloc(this->sNumKnots * sizeof(GLfloat));
		for (int i = 0; i < this->sNumKnots; i++) {
			this->sKnots[i] = other.sKnots[i];
		}
	} else {
		this->sKnots = NULL;
	}
	this->tNumKnots = other.tNumKnots;
	if (other.tKnots != NULL) {
		this->tKnots = (GLfloat*)malloc(this->tNumKnots * sizeof(GLfloat));
		for (int i = 0; i < this->tNumKnots; i++) {
			this->tKnots[i] = other.tKnots[i];
		}
	} else {
		this->tKnots = NULL;
	}
	this->sStride = other.sStride;
	this->tStride = other.tStride;
	this->sOrder = other.sOrder;
	this->tOrder = other.tOrder;
	this->textureName = other.textureName;
	this->canUseRectExtension = other.canUseRectExtension;
	uint16 numberOfTextureControlPoints = (this->sNumKnots - this->sOrder) * (this->tNumKnots - this->tOrder);
	this->texturePoints = (GLfloat*)malloc(numberOfTextureControlPoints * sizeof(GLfloat));
	for (int i = 0; i < numberOfTextureControlPoints; i++) {
		this->texturePoints[i] = other.texturePoints[i];
	}
}


void VisualNurbs::setNurbsProperty(uint32 property, float value) {

/*
	//gluNurbsCallback(this->coverNurb, GLU_ERROR, nurbsError);
*/
	
	gluNurbsProperty(this->nurbsObject, (GLenum)property, (GLfloat)value);
}


void VisualNurbs::setSamplingTolerance(double samplingTolerance) {
	this->setNurbsProperty(GLU_SAMPLING_TOLERANCE, static_cast<float>(samplingTolerance));
}


void VisualNurbs::setDisplayMode(NurbsDisplayMode requestedDisplayMode) {
	if (requestedDisplayMode == kFill) {
		this->setNurbsProperty(GLU_DISPLAY_MODE, GLU_FILL);
	} else if (requestedDisplayMode == kOutlinePolygon) {
		this->setNurbsProperty(GLU_DISPLAY_MODE, GLU_OUTLINE_POLYGON);
	}
}


void VisualNurbs::setControlPointsVisibility(Visibility visibility) {
	this->controlPointsVisibility = visibility;
}


void VisualNurbs::setControlPoints(uint16 numberOfControlPoints, double* someControlPoints) {
	if (this->controlPoints != NULL) {
		free(this->controlPoints);
		this->controlPoints = NULL;
	}
	this->controlPoints = (GLfloat*)malloc(numberOfControlPoints * sizeof(GLfloat));
	for (int i = 0; i < numberOfControlPoints; i++) {
		this->controlPoints[i] = (GLfloat)(someControlPoints[i]);
	}
}


void VisualNurbs::setTexture(uint8 sKnotsCount, float* sKnotsPointer, uint8 tKnotsCount, float* tKnotsPointer, uint16 s_Stride, uint16 t_Stride, uint16 s_Order, uint16 t_Order, uint32 aTextureName, bool canUseRectExtensionBool) {

	if (this->nurbsTexture != NULL) {
		delete this->nurbsTexture;
		this->nurbsTexture = NULL;
	}
	this->nurbsTexture = new NurbsTexture;
	this->nurbsTexture->sNumKnots = sKnotsCount;
	this->nurbsTexture->sKnots = (GLfloat*)malloc(sKnotsCount * sizeof(GLfloat));
	for (int i = 0; i < sKnotsCount; i++) {
		this->nurbsTexture->sKnots[i] = sKnotsPointer[i];
	}
	this->nurbsTexture->tNumKnots = tKnotsCount;
	this->nurbsTexture->tKnots = (GLfloat*)malloc(tKnotsCount * sizeof(GLfloat));
	for (int i = 0; i < tKnotsCount; i++) {
		this->nurbsTexture->tKnots[i] = tKnotsPointer[i];
	}

	this->nurbsTexture->sStride = s_Stride;
	this->nurbsTexture->tStride = t_Stride;
	this->nurbsTexture->sOrder = s_Order;
	this->nurbsTexture->tOrder = t_Order;
	
	this->nurbsTexture->textureName = aTextureName;
	this->nurbsTexture->canUseRectExtension = canUseRectExtensionBool;
		
}


void VisualNurbs::setTextureControlPoints(uint16 numberOfTextureControlPoints, float* someTextureControlPoints) {

	if (this->nurbsTexture != NULL) {
		if (this->nurbsTexture->texturePoints != NULL) {
			free(this->nurbsTexture->texturePoints);
			this->nurbsTexture->texturePoints = NULL;
		}
		this->nurbsTexture->texturePoints = (GLfloat*)malloc(numberOfTextureControlPoints * sizeof(GLfloat));
		for (int i = 0; i < numberOfTextureControlPoints; i++) {
			this->nurbsTexture->texturePoints[i] = someTextureControlPoints[i];
		}
	} else {
		char errLog[256];
		sprintf(errLog, "no NurbsTexture available in file: %s (line: %d) [%s])", __FILE__, __LINE__, __FUNCTION__);
		writeLog(errLog);
	}
}


void VisualNurbs::draw() {
	
	if (this->nurbsTexture != NULL) {
		if (this->nurbsTexture->canUseRectExtension == false) {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, (GLuint)this->nurbsTexture->textureName);
		} else {
#if TARGET_OS_MAC
			glEnable(GL_TEXTURE_RECTANGLE_EXT);
			glBindTexture(GL_TEXTURE_RECTANGLE_EXT, (GLuint)this->nurbsTexture->textureName);
#endif
		}
	}

	gluBeginSurface(this->nurbsObject);
		if (this->nurbsTexture != NULL) {
			gluNurbsSurface(this->nurbsObject, 
								this->nurbsTexture->sNumKnots, 
								this->nurbsTexture->sKnots, 
								this->nurbsTexture->tNumKnots, 
								this->nurbsTexture->tKnots, 
								this->nurbsTexture->sStride, 
								this->nurbsTexture->tStride,
								this->nurbsTexture->texturePoints,
								this->nurbsTexture->sOrder,
								this->nurbsTexture->tOrder,
								GL_MAP2_TEXTURE_COORD_2);
		}
		gluNurbsSurface(this->nurbsObject, 
							this->sNumKnots, 
							this->sKnots, 
							this->tNumKnots, 
							this->tKnots, 
							this->sStride, 
							this->tStride, 
							this->controlPoints,
							this->sOrder, 
							this->tOrder, 
							GL_MAP2_VERTEX_3);
	gluEndSurface(this->nurbsObject);

	if (this->nurbsTexture != NULL) {
		if (this->nurbsTexture->canUseRectExtension == false) {
			glDisable(GL_TEXTURE_2D);
		} else {
#if TARGET_OS_MAC
			glDisable(GL_TEXTURE_RECTANGLE_EXT);
#endif
		}
	}

	if (this->controlPointsVisibility == kVisible) {
		int cnt = 0;
		glPointSize(3.0f);
		glColor3d(1.0, 1.0, 0.0);
		glBegin(GL_POINTS);
		int i, j;
		for (i = 0; i < 8; i++) {
			for (j = 0; j < 8; j++) {
				glVertex3f(this->controlPoints[cnt], this->controlPoints[cnt + 1], this->controlPoints[cnt + 2]);
				cnt += 3;
			}
		}
		glEnd();
	}

}

