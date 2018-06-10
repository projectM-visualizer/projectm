
#include "Common.hpp"
#include "Renderable.hpp"
#include <math.h>
#include "ShaderEngine.hpp"
#include <glm/gtc/type_ptr.hpp>

typedef float floatPair[2];
typedef float floatTriple[3];
typedef float floatQuad[4];

RenderContext::RenderContext()
	: time(0),texsize(512), aspectRatio(1), aspectCorrect(false){};

RenderItem::RenderItem():masterAlpha(1){}

void RenderItem::Init() {
    glGenVertexArrays(1, &m_vaoID);
    glGenBuffers(1, &m_vboID);

    glBindVertexArray(m_vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    InitVertexAttrib();

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

RenderItem::~RenderItem() {
    glDeleteBuffers(1, &m_vboID);
    glDeleteVertexArrays(1, &m_vaoID);
}


DarkenCenter::DarkenCenter():RenderItem(){
    Init();
}

MotionVectors::MotionVectors():RenderItem() {
    Init();
}

Border::Border():RenderItem() {
    Init();
}

void DarkenCenter::InitVertexAttrib() {
    float points_colors[6][6] = {
        { 0.5,  0.5,      0, 0, 0, (3.0f/32.0f) * masterAlpha},
        { 0.45, 0.5,      0, 0, 0, 0},
        { 0.5,  0.45,     0, 0, 0, 0},
        { 0.55, 0.5,      0, 0, 0, 0},
        { 0.5,  0.55,     0, 0, 0, 0},
        { 0.45, 0.5,      0, 0, 0, 0}};

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)0); // points
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)(sizeof(float)*2)); // colors

    glBufferData(GL_ARRAY_BUFFER, sizeof(points_colors), points_colors, GL_STATIC_DRAW);

}


void DarkenCenter::Draw(RenderContext &context)
	{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(context.programID_v2f_c4f);

    glUniformMatrix4fv(ShaderEngine::Uniform_V2F_C4F_VertexTranformation(), 1, GL_FALSE, glm::value_ptr(context.mat_ortho));

    glBindVertexArray(m_vaoID);

	glDrawArrays(GL_TRIANGLE_FAN,0,6);

    glBindVertexArray(0);
}

Shape::Shape():RenderItem()
{
	 std::string imageUrl = "";
	     sides = 4;
	     thickOutline = false;
	     enabled = true;
	     additive = false;
	     textured = false;

	     tex_zoom = 1.0;
	     tex_ang = 0.0;

	     x = 0.5;
	     y = 0.5;
	     radius = 1.0;
	     ang = 0.0;

	     r = 0.0; /* red color value */
	     g = 0.0; /* green color value */
	     b = 0.0; /* blue color value */
	     a = 0.0; /* alpha color value */

	     r2 = 0.0; /* red color value */
	     g2 = 0.0; /* green color value */
	     b2 = 0.0; /* blue color value */
	     a2 = 0.0; /* alpha color value */

	     border_r = 0.0; /* red color value */
	     border_g = 0.0; /* green color value */
	     border_b = 0.0; /* blue color value */
	     border_a = 0.0; /* alpha color value */


    glGenVertexArrays(1, &m_vaoID_texture);
    glGenBuffers(1, &m_vboID_texture);

    glGenVertexArrays(1, &m_vaoID_not_texture);
    glGenBuffers(1, &m_vboID_not_texture);

    glBindVertexArray(m_vaoID_texture);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID_texture);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct_data), (void*)0);     // Positions
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(struct_data), (void*)(sizeof(float)*2));     // Colors
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct_data), (void*)(sizeof(float)*6)); // Textures

    glBindVertexArray(m_vaoID_not_texture);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID_not_texture);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct_data), (void*)0);   // points
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(struct_data), (void*)(sizeof(float)*2));     // Colors

    Init();
}

Shape::~Shape() {
    glDeleteBuffers(1, &m_vboID_texture);
    glDeleteVertexArrays(1, &m_vaoID_texture);

    glDeleteBuffers(1, &m_vboID_not_texture);
    glDeleteVertexArrays(1, &m_vaoID_not_texture);
}

void Shape::InitVertexAttrib() {

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);   // points
    glDisableVertexAttribArray(1);
}


void Shape::Draw(RenderContext &context)
{

	float xval, yval;
	float t;

	float temp_radius= radius*(.707*.707*.707*1.04);

	//Additive Drawing or Overwrite
	if ( additive==0)  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	else    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	xval= x;
	yval= -(y-1);

    struct_data *buffer_data = new struct_data[sides+2];

	if ( textured)
	{
		if (imageUrl !="")
		{
			GLuint tex= context.textureManager->getTexture(imageUrl);
			if (tex != 0)
			{
				glBindTexture(GL_TEXTURE_2D, tex);
				context.aspectRatio=1.0;
			}
		}


		//Define the center point of the shape
        buffer_data[0].color_r = r;
        buffer_data[0].color_g = g;
        buffer_data[0].color_b = b;
        buffer_data[0].color_a = a * masterAlpha;
        buffer_data[0].tex_x = 0.5;
        buffer_data[0].tex_y = 0.5;
        buffer_data[0].point_x = xval;
        buffer_data[0].point_y = yval;

		for ( int i=1;i< sides+2;i++)
		{
            buffer_data[i].color_r=r2;
            buffer_data[i].color_g=g2;
            buffer_data[i].color_b=b2;
            buffer_data[i].color_a=a2 * masterAlpha;

		  t = (i-1)/(float) sides;
            buffer_data[i].tex_x =0.5f + 0.5f*cosf(t*3.1415927f*2 +  tex_ang + 3.1415927f*0.25f)*(context.aspectCorrect ? context.aspectRatio : 1.0)/ tex_zoom;
            buffer_data[i].tex_y =  0.5f + 0.5f*sinf(t*3.1415927f*2 +  tex_ang + 3.1415927f*0.25f)/ tex_zoom;
            buffer_data[i].point_x=temp_radius*cosf(t*3.1415927f*2 +  ang + 3.1415927f*0.25f)*(context.aspectCorrect ? context.aspectRatio : 1.0)+xval;
            buffer_data[i].point_y=temp_radius*sinf(t*3.1415927f*2 +  ang + 3.1415927f*0.25f)+yval;

		}

        glBindBuffer(GL_ARRAY_BUFFER, m_vboID_texture);

        glBufferData(GL_ARRAY_BUFFER, sizeof(struct_data)*(sides+2), NULL, GL_DYNAMIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, sizeof(struct_data)*(sides+2), buffer_data, GL_DYNAMIC_DRAW);

        glUseProgram(context.programID_v2f_c4f_t2f);

        glActiveTexture(GL_TEXTURE0);

        glUniformMatrix4fv(ShaderEngine::Uniform_V2F_C4F_T2F_VertexTranformation(), 1, GL_FALSE, glm::value_ptr(context.mat_ortho));

        glUniform1i(ShaderEngine::Uniform_V2F_C4F_T2F_FragTextureSampler(), 0);

        glBindVertexArray(m_vaoID_texture);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sides+2);
        glBindVertexArray(0);
	}
	else
	{//Untextured (use color values)

	  //Define the center point of the shape
        buffer_data[0].color_r=r;
        buffer_data[0].color_g=g;
        buffer_data[0].color_b=b;
        buffer_data[0].color_a=a * masterAlpha;
        buffer_data[0].point_x=xval;
        buffer_data[0].point_y=yval;


	  for ( int i=1;i< sides+2;i++)
	    {
            buffer_data[i].color_r=r2;
            buffer_data[i].color_g=g2;
            buffer_data[i].color_b=b2;
            buffer_data[i].color_a=a2 * masterAlpha;
	      t = (i-1)/(float) sides;
            buffer_data[i].point_x=temp_radius*cosf(t*3.1415927f*2 +  ang + 3.1415927f*0.25f)*(context.aspectCorrect ? context.aspectRatio : 1.0)+xval;
            buffer_data[i].point_y=temp_radius*sinf(t*3.1415927f*2 +  ang + 3.1415927f*0.25f)+yval;
	    }

        glBindBuffer(GL_ARRAY_BUFFER, m_vboID_not_texture);

        glBufferData(GL_ARRAY_BUFFER, sizeof(struct_data)*(sides+2), NULL, GL_DYNAMIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, sizeof(struct_data)*(sides+2), buffer_data, GL_DYNAMIC_DRAW);

        glUseProgram(context.programID_v2f_c4f);

        glUniformMatrix4fv(ShaderEngine::Uniform_V2F_C4F_VertexTranformation(), 1, GL_FALSE, glm::value_ptr(context.mat_ortho));

        glBindVertexArray(m_vaoID_not_texture);
	  	glDrawArrays(GL_TRIANGLE_FAN,0,sides+2);
        glBindVertexArray(0);
	}


	floatPair *points = new float[sides+1][2];

	for ( int i=0;i< sides;i++)
	{
		t = (i-1)/(float) sides;
		points[i][0]= temp_radius*cosf(t*3.1415927f*2 +  ang + 3.1415927f*0.25f)*(context.aspectCorrect ? context.aspectRatio : 1.0)+xval;
		points[i][1]=  temp_radius*sinf(t*3.1415927f*2 +  ang + 3.1415927f*0.25f)+yval;
	}

    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    glBufferData(GL_ARRAY_BUFFER, sizeof(floatPair)*(sides), NULL, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floatPair)*(sides), points, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(context.programID_v2f_c4f);

    glUniformMatrix4fv(ShaderEngine::Uniform_V2F_C4F_VertexTranformation(), 1, GL_FALSE, glm::value_ptr(context.mat_ortho));

    glVertexAttrib4f(1, border_r, border_g, border_b, border_a * masterAlpha);

	if (thickOutline==1)  glLineWidth(context.texsize < 512 ? 1 : 2*context.texsize/512);

    glBindVertexArray(m_vaoID);
	glDrawArrays(GL_LINE_LOOP,0,sides);
    glBindVertexArray(0);

	if (thickOutline==1)  glLineWidth(context.texsize < 512 ? 1 : context.texsize/512);

    delete[] buffer_data;
	delete[] points;
}

void MotionVectors::InitVertexAttrib() {
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glDisableVertexAttribArray(1);
}

void MotionVectors::Draw(RenderContext &context)
{
	float  intervalx=1.0/x_num;
	float  intervaly=1.0/y_num;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (x_num + y_num < 600)
	{
		int size = x_num * y_num ;

		floatPair *points = new float[size][2];

		for (int x=0;x<(int)x_num;x++)
		{
			for(int y=0;y<(int)y_num;y++)
			{
				float lx, ly, lz;
				lx = x_offset+x*intervalx;
				ly = y_offset+y*intervaly;

				points[(x * (int)y_num) + y][0] = lx;
				points[(x * (int)y_num) + y][1] = ly;
			}
		}

        glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

        glBufferData(GL_ARRAY_BUFFER, sizeof(floatPair) * size, NULL, GL_DYNAMIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, sizeof(floatPair) * size, points, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        delete[] points;


		glUseProgram(context.programID_v2f_c4f);

        glUniformMatrix4fv(ShaderEngine::Uniform_V2F_C4F_VertexTranformation(), 1, GL_FALSE, glm::value_ptr(context.mat_ortho));

		#ifndef GL_TRANSITION
        if (length <= 0.0) {
            glPointSize(1.0);
        } else {
            glPointSize(length);
        }
        #endif

        glUniform1f(ShaderEngine::Uniform_V2F_C4F_VertexPointSize(), length);
		glVertexAttrib4f(1, r, g, b, a * masterAlpha);

        glBindVertexArray(m_vaoID);

		glDrawArrays(GL_POINTS,0,size);

        glBindVertexArray(0);
	  }
}

void Border::InitVertexAttrib() {
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glDisableVertexAttribArray(1);
}

void Border::Draw(RenderContext &context)
{
    //Draw Borders
    float of=outer_size*.5;
    float iff=inner_size*.5;
    float texof=1.0-of;

    float points[40] = {
        // Outer
        0,0,            of,0,
        0,1,            of,texof,
        1,1,            texof,texof,
        1,0,            texof,of,
        of,0,           of,of,

        // Inner
        of,of,          of+iff,of,
        of,texof,       of+iff,texof-iff,
        texof,texof,    texof-iff,texof-iff,
        texof,of,       texof-iff,of+iff,
        of+iff,of,      of+iff,of+iff,
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 40, NULL, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 40, points, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(context.programID_v2f_c4f);

    glUniformMatrix4fv(ShaderEngine::Uniform_V2F_C4F_VertexTranformation(), 1, GL_FALSE, glm::value_ptr(context.mat_ortho));

    glVertexAttrib4f(1, outer_r, outer_g, outer_b, outer_a * masterAlpha);

    //no additive drawing for borders
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(m_vaoID);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);

    glVertexAttrib4f(1, inner_r, inner_g, inner_b, inner_a * masterAlpha);

    // 1st pass for inner
    glDrawArrays(GL_TRIANGLE_STRIP, 10, 10);

    // 2nd pass for inner
    glDrawArrays(GL_TRIANGLE_STRIP, 10, 10);

    glBindVertexArray(0);
}
