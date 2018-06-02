/*
 * Waveform.hpp
 *
 *  Created on: Jun 25, 2008
 *      Author: pete
 */

#include "projectM-opengl.h"
#include "Waveform.hpp"
#include <algorithm>
#include "BeatDetect.hpp"

typedef float floatPair[2];
typedef float floatTriple[3];
typedef float floatQuad[4];

Waveform::Waveform(int samples)
: RenderItem(),samples(samples), points(samples), pointContext(samples)
{
	spectrum = false; /* spectrum data or pcm data */
	dots = false; /* draw wave as dots or lines */
	thick = false; /* draw thicker lines */
	additive = false; /* add color values together */

	scaling = 1; /* scale factor of waveform */
	smoothing = 0; /* smooth factor of waveform */
	sep = 0;
}

void Waveform::Draw(RenderContext &context)
{
    
    //if (samples > 2048) samples = 2048;
    
    
    if (additive)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    else
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    if (thick)
    {
        glLineWidth(context.texsize <= 512 ? 2 : 2*context.texsize/512);
        glPointSize(context.texsize <= 512 ? 2 : 2*context.texsize/512);
        
    }
    else glPointSize(context.texsize <= 512 ? 1 : context.texsize/512);
    
    float *value1 = new float[samples];
    float *value2 = new float[samples];
    context.beatDetect->pcm->getPCM( value1, samples, 0, spectrum, smoothing, 0);
    context.beatDetect->pcm->getPCM( value2, samples, 1, spectrum, smoothing, 0);
    // printf("%f\n",pcmL[0]);
    
    
    float mult = scaling * (spectrum ? 0.015f :1.0f);

    std::transform(&value1[0],&value1[samples],&value1[0],std::bind2nd(std::multiplies<float>(),mult));
    std::transform(&value2[0],&value2[samples],&value2[0],std::bind2nd(std::multiplies<float>(),mult));
    
    WaveformContext waveContext(samples, context.beatDetect);
    
    for(int x=0;x< samples;x++)
    {
        waveContext.sample = x/(float)(samples - 1);
        waveContext.sample_int = x;
        waveContext.left = value1[x];
        waveContext.right = value2[x];
        
        points[x] = PerPoint(points[x],waveContext);
    }
    
    floatQuad *colors = new float[samples][4];
    floatPair *p = new float[samples][2];
    
    for(int x=0;x< samples;x++)
    {
        colors[x][0] = points[x].r;
        colors[x][1] = points[x].g;
        colors[x][2] = points[x].b;
        colors[x][3] = points[x].a * masterAlpha;
        
        p[x][0] = points[x].x;
        p[x][1] = -(points[x].y-1);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, samples * 2, p, GL_DYNAMIC_DRAW);
    check_gl_error();

//    glVertexPointer(2,GL_FLOAT,0,p);
//    glColorPointer(4,GL_FLOAT,0,colors);
    
    if (dots)
        glDrawArrays(GL_POINTS,0,samples);
    else
        glDrawArrays(GL_LINE_STRIP,0,samples);
    
    check_gl_error();
    
    glPointSize(context.texsize < 512 ? 1 : context.texsize/512);
    glLineWidth(context.texsize < 512 ? 1 : context.texsize/512);
//    glDisable(GL_LINE_STIPPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    delete[] colors;
    delete[] p;
    delete[] value1;
    delete[] value2;
}
