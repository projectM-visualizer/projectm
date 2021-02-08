/*
 * MilkdropWaveform.cpp
 *
 *  Created on: Jun 25, 2008
 *      Author: pete
 */
#include <iostream>
#include "projectM-opengl.h"
#include <cmath>
#include "MilkdropWaveform.hpp"
#include "math.h"
#include "BeatDetect.hpp"
#include "ShaderEngine.hpp"
#include <glm/gtc/type_ptr.hpp>

MilkdropWaveform::MilkdropWaveform(): RenderItem(),
    x(0.5), y(0.5), r(1), g(0), b(0), a(1), mystery(0), mode(Line), additive(false), dots(false), thick(false),
    modulateAlphaByVolume(false), maximizeColors(false), scale(10), smoothing(0),
    modOpacityStart(0), modOpacityEnd(1), rot(0), samples(0), loop(false) {

    Init();
}

MilkdropWaveform::~MilkdropWaveform() {
}

void MilkdropWaveform::InitVertexAttrib() {

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glDisableVertexAttribArray(1);

}


void MilkdropWaveform::Draw(RenderContext &context)
{
    WaveformMath(context);

    for (int waveno=1 ; waveno<=(two_waves?2:1) ; waveno++)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * samples * 2, NULL, GL_DYNAMIC_DRAW);
        if (waveno == 1)
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * samples * 2, wavearray, GL_DYNAMIC_DRAW);
        else
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * samples * 2, wavearray2, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glUseProgram(context.programID_v2f_c4f);

        glm::mat4 mat_first_translation = glm::mat4(1.0);
        mat_first_translation[3][0] = -0.5;
        mat_first_translation[3][1] = -0.5;

        glm::mat4 mat_scale = glm::mat4(1.0);
        mat_scale[0][0] = aspectScale;

        float s = glm::sin(glm::radians(-rot));
        float c = glm::cos(glm::radians(-rot));
        glm::mat4 mat_rotation = glm::mat4(c, -s, 0, 0,
                                           s, c, 0, 0,
                                           0, 0, 1, 0,
                                           0, 0, 0, 1);

        glm::mat4 mat_second_translation = glm::mat4(1.0);
        mat_second_translation[3][0] = 0.5;
        mat_second_translation[3][1] = 0.5;

        glm::mat4 mat_vertex = context.mat_ortho;
        mat_vertex = mat_first_translation * mat_vertex;
        mat_vertex = mat_scale * mat_vertex;
        mat_vertex = mat_rotation * mat_vertex;
        mat_vertex = mat_second_translation * mat_vertex;
        glUniformMatrix4fv(context.uniform_v2f_c4f_vertex_tranformation, 1, GL_FALSE, glm::value_ptr(mat_vertex));

        if (modulateAlphaByVolume) ModulateOpacityByVolume(context);
        else temp_a = a;
        MaximizeColors(context);

#ifndef GL_TRANSITION
        if (dots == 1) glEnable(GL_LINE_STIPPLE);
#endif

        //Thick wave drawing
        if (thick == 1) glLineWidth((context.texsize < 512) ? 2 : 2 * context.texsize / 512);
        else glLineWidth((context.texsize < 512) ? 1 : context.texsize / 512);

        //Additive wave drawing (vice overwrite)
        if (additive == 1)glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        else glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindVertexArray(m_vaoID);

        if (loop)
            glDrawArrays(GL_LINE_LOOP, 0, samples);
        else
            glDrawArrays(GL_LINE_STRIP, 0, samples);

        glBindVertexArray(0);
    }

#ifndef GL_TRANSITION
		if(dots==1) glDisable(GL_LINE_STIPPLE);
#endif
}

void MilkdropWaveform::ModulateOpacityByVolume(RenderContext &context)
{

	//modulate volume by opacity
	//
	//set an upper and lower bound and linearly
	//calculate the opacity from 0=lower to 1=upper
	//based on current volume

	if (context.beatDetect->vol<= modOpacityStart)  temp_a=0.0;
	 else if (context.beatDetect->vol>=modOpacityEnd) temp_a=a;
	 else temp_a=a*((context.beatDetect->vol-modOpacityStart)/(modOpacityEnd-modOpacityStart));

}

void MilkdropWaveform::MaximizeColors(RenderContext &context)
{
	float wave_r_switch=0, wave_g_switch=0, wave_b_switch=0;
	//wave color brightening
	//
	//forces max color value to 1.0 and scales
	// the rest accordingly
	if(mode==Blob2 || mode==Blob5)
		switch(context.texsize)
		{
			case 256:  temp_a *= 0.07f; break;
			case 512:  temp_a *= 0.09f; break;
			case 1024: temp_a *= 0.11f; break;
			case 2048: temp_a *= 0.13f; break;
		}
	else if(mode==Blob3)
	{
		switch(context.texsize)
		{
			case 256:  temp_a *= 0.075f; break;
			case 512:  temp_a *= 0.15f; break;
			case 1024: temp_a *= 0.22f; break;
			case 2048: temp_a *= 0.33f; break;
		}
		temp_a*=1.3f;
		temp_a*=std::pow(context.beatDetect->treb , 2.0f);
	}

	if (maximizeColors==true)
	{
		if(r>=g && r>=b)   //red brightest
		{
			wave_b_switch=b*(1/r);
			wave_g_switch=g*(1/r);
			wave_r_switch=1.0;
		}
		else if   (b>=g && b>=r)         //blue brightest
		{
			wave_r_switch=r*(1/b);
			wave_g_switch=g*(1/b);
			wave_b_switch=1.0;

		}

		else  if (g>=b && g>=r)         //green brightest
		{
			wave_b_switch=b*(1/g);
			wave_r_switch=r*(1/g);
			wave_g_switch=1.0;
		}


        glVertexAttrib4f(1, wave_r_switch, wave_g_switch, wave_b_switch, temp_a * masterAlpha);
	}
	else
	{
        glVertexAttrib4f(1, r, g, b, temp_a * masterAlpha);
	}
}


void MilkdropWaveform::WaveformMath(RenderContext &context)
{
	float *pcmdataR = context.beatDetect->pcm->pcmdataR;
	float *pcmdataL = context.beatDetect->pcm->pcmdataL;
	// scale PCM data based on vol_history to make it more or less independent of the application output volume
    const float  vol_scale = context.beatDetect->getPCMScale();

    float r2, theta;

	float wave_x_temp=0;
	float wave_y_temp=0;

	float cos_rot;
	float sin_rot;

    const float temp_y = -1*(y-1.0f);

	two_waves = false;
	loop = false;

	switch(mode)
	{

		case Circle:
	  	{
 		    loop = true;
			rot =   0;
			aspectScale=1.0;

			samples = context.beatDetect->pcm->numsamples;

			float inv_nverts_minus_one = 1.0f/(float)(samples);

	        float last_value =  vol_scale * (pcmdataR[samples-1]+pcmdataL[samples-1]);
			float first_value = vol_scale * (pcmdataR[0]+pcmdataL[0]);
			float offset = first_value-last_value;

			for ( int i=0;i<samples;i++)
			{

			  float value = vol_scale * (pcmdataR[i]+pcmdataL[i]);
			  value += offset * (i/(float)samples);

              r2=(0.5f + 0.4f*.12f*value*scale + mystery)*.5f;
              theta=i*inv_nverts_minus_one*6.28f + context.time*0.2f;

              wavearray[i][0]=(r2*cos(theta)*(context.aspectCorrect? context.aspectRatio : 1.0f)+x);
              wavearray[i][1]=(r2*sin(theta)+temp_y);
			}
	    }
		break;

		case RadialBlob://circularly moving waveform
        {
			rot = 0;
			aspectScale = context.aspectRatio;

			samples = 512-32;
			for ( int i=0;i<512-32;i++)
			{
                theta=vol_scale*pcmdataL[i+32]*0.06f*scale * 1.57f + context.time*2.3f;
                r2=(0.53f + 0.43f*vol_scale*pcmdataR[i]*0.12f*scale+ mystery)*.5f;

                wavearray[i][0]=(r2*cos(theta)*(context.aspectCorrect ? context.aspectRatio : 1.0f)+x);
                wavearray[i][1]=(r2*sin(theta)+temp_y);
			}
        }
        break;

		case Blob2://EXPERIMENTAL

			rot =   0;
			aspectScale =1.0;
			samples = 512-32;

			for ( int i=0;i<512-32;i++)
			{
				wavearray[i][0]=(vol_scale*pcmdataR[i]*scale*0.5f*(context.aspectCorrect ? context.aspectRatio : 1.0f) + x);
				wavearray[i][1]=(vol_scale*pcmdataL[i+32]*scale*0.5f + temp_y);
			}

			break;

		case Blob3://EXPERIMENTAL

			rot =   0;
			aspectScale =1.0;

			samples = 512-32;

			for ( int i=0;i<512-32;i++)
			{
				wavearray[i][0]=(vol_scale*pcmdataR[i] * scale*0.5f + x);
				wavearray[i][1]=( (vol_scale*pcmdataL[i+32]*scale*0.5f + temp_y));
			}

			break;

		case DerivativeLine://single x-axis derivative waveform
		{
			rot =-mystery*90;
			aspectScale=1.0;

			float w1 = 0.45f + 0.5f*(mystery*0.5f + 0.5f);
			float w2 = 1.0f - w1;
			float xx[512], yy[512];
			samples = 512-32;

			for (int i=0; i<512-32; i++)
			{
				xx[i] = -1.0f + 2.0f*(i/(512.0f-32.0f)) + x;
				yy[i] =  0.4f* vol_scale*pcmdataL[i]*0.47f*scale + temp_y;
				xx[i] += 0.4f* vol_scale*pcmdataR[i]*0.44f*scale;

				if (i>1)
				{
					xx[i] = xx[i]*w2 + w1*(xx[i-1]*2.0f - xx[i-2]);
					yy[i] = yy[i]*w2 + w1*(yy[i-1]*2.0f - yy[i-2]);
				}
				wavearray[i][0]=xx[i];
				wavearray[i][1]=yy[i];
			}											   		}
		break;

		case Blob5://EXPERIMENTAL

			rot = 0;
			aspectScale =1.0;

			cos_rot = cosf(context.time*0.3f);
			sin_rot = sinf(context.time*0.3f);
			samples = 512-32;

			for ( int i=0;i<512-32;i++)
			{
				float x0 = (vol_scale*pcmdataR[i]*vol_scale*pcmdataL[i+32] + vol_scale*pcmdataL[i+32]*vol_scale*pcmdataR[i]);
				float y0 = (vol_scale*pcmdataR[i]*vol_scale*pcmdataR[i] - vol_scale*pcmdataL[i+32]*vol_scale*pcmdataL[i+32]);
				wavearray[i][0]=((x0*cos_rot - y0*sin_rot)*scale*0.5f*(context.aspectCorrect ? context.aspectRatio : 1.0f) + x);
				wavearray[i][1]=( (x0*sin_rot + y0*cos_rot)*scale*0.5f + temp_y);
			}
			break;

		case Line://single waveform


			wave_x_temp=-2*0.4142f*(fabs(fabs(mystery)-.5f)-.5f);

			rot = -mystery*90;
			aspectScale =1.0f+wave_x_temp;
			wave_x_temp=-1*(x-1.0f);
			samples = context.beatDetect->pcm->numsamples;

			for ( int i=0;i<  samples;i++)
			{

				wavearray[i][0]=i/(float)  samples;
				wavearray[i][1]=vol_scale*pcmdataR[i]*.04f*scale+wave_x_temp;

			}
			//	  printf("%f %f\n",renderTarget->texsize*wave_y_temp,wave_y_temp);

			break;

		case DoubleLine://dual waveforms
		{

			wave_x_temp=-2*0.4142f*(fabs(fabs(mystery)-.5f)-.5f);

			rot = -mystery*90;
			aspectScale =1.0f+wave_x_temp;


			samples = context.beatDetect->pcm->numsamples;
			two_waves = true;

			const float y_adj = y*y*.5f;

			wave_y_temp=-1*(x-1);

			for ( int i=0;i<samples;i++)
			{
				wavearray[i][0]=i/((float)  samples);
				wavearray[i][1]= vol_scale*pcmdataL[i]*.04f*scale+(wave_y_temp+y_adj);
			}

			for ( int i=0;i<samples;i++)
			{
				wavearray2[i][0]=i/((float)  samples);
				wavearray2[i][1]=vol_scale*pcmdataR[i]*.04f*scale+(wave_y_temp-y_adj);
			}

			break;
		}
		case last:
            break;
	}
}
