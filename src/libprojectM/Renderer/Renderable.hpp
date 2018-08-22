#ifndef Renderable_HPP
#define Renderable_HPP
#include <vector>
#include <typeinfo>
#include "TextureManager.hpp"
#include "projectM-opengl.h"
#include <glm/mat4x4.hpp>

class BeatDetect;


class RenderContext
{
public:
	float time;
	int texsize;
	float aspectRatio;
	bool aspectCorrect;
	BeatDetect *beatDetect;
	TextureManager *textureManager;
    GLuint programID_v2f_c4f;
    GLuint programID_v2f_c4f_t2f;
    GLint uniform_v2f_c4f_vertex_tranformation;
    GLint uniform_v2f_c4f_vertex_point_size;
    GLint uniform_v2f_c4f_t2f_vertex_tranformation;
    GLint uniform_v2f_c4f_t2f_frag_texture_sampler;
    glm::mat4 mat_ortho;

	RenderContext();
};

class RenderItem
{
public:
    RenderItem();
    ~RenderItem();

	float masterAlpha;
    virtual void InitVertexAttrib() = 0;
	virtual void Draw(RenderContext &context) = 0;

protected:
    virtual void Init();

    GLuint m_vboID;
    GLuint m_vaoID;
};

typedef std::vector<RenderItem*> RenderItemList;

class DarkenCenter : public RenderItem
{
public:
	DarkenCenter();
    void InitVertexAttrib();
	void Draw(RenderContext &context);
};

class Shape : public RenderItem
{
public:
    std::string imageUrl;
    int sides;
    bool thickOutline;
    bool enabled;
    bool additive;
    bool textured;

    float tex_zoom;
    float tex_ang;

    float x; /* x position  */
    float y; /* y position  */
    float radius;
    float ang;

    float r; /* red color value */
    float g; /* green color value */
    float b; /* blue color value */
    float a; /* alpha color value */

    float r2; /* red color value */
    float g2; /* green color value */
    float b2; /* blue color value */
    float a2; /* alpha color value */

    float border_r; /* red color value */
    float border_g; /* green color value */
    float border_b; /* blue color value */
    float border_a; /* alpha color value */


    Shape();
    ~Shape();
    void InitVertexAttrib();
    virtual void Draw(RenderContext &context);

private:

    struct struct_data {
        float point_x;
        float point_y;
        float color_r;
        float color_g;
        float color_b;
        float color_a;
        float tex_x;
        float tex_y;
    };

    GLuint m_vboID_texture;
    GLuint m_vaoID_texture;

    GLuint m_vboID_not_texture;
    GLuint m_vaoID_not_texture;
};

class Text : RenderItem
{
};

class MotionVectors : public RenderItem
{
public:
    float r;
    float g;
    float b;
    float a;
    float length;
    float x_num;
    float y_num;
    float x_offset;
    float y_offset;

    void InitVertexAttrib();
    void Draw(RenderContext &context);
    MotionVectors();
};

class Border : public RenderItem
{
public:
    float outer_size;
    float outer_r;
    float outer_g;
    float outer_b;
    float outer_a;

    float inner_size;
    float inner_r;
    float inner_g;
    float inner_b;
    float inner_a;

    void InitVertexAttrib();
    void Draw(RenderContext &context);
    Border();
};

struct TypeIdPair {
	TypeIdPair(const std::type_info & info1, const std::type_info & info2): id1(info1.name()), id2(info2.name()) {}
	TypeIdPair(const std::string & id1, const std::string & id2): id1(id1), id2(id2) {}
	std::string id1;
	std::string id2;
	inline bool operator<(const TypeIdPair & rhs) const {
		return this->id1 < rhs.id1 || (this->id1 == rhs.id1 && this->id2 < rhs.id2);
	}

	inline bool operator>(const TypeIdPair & rhs) const {
		return !operator<(rhs) && !operator==(rhs);
	}

	inline bool operator==(const TypeIdPair & rhs) const {
			return this->id1 == rhs.id1 && this->id2 == rhs.id2;
	}
};

#endif
