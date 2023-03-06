#pragma once

#include "PerFrameContext.hpp"
#include "PresetState.hpp"

#include "projectM-opengl.h"

#include <Renderer/RenderItem.hpp>

class VideoEcho: public RenderItem
{
public:
	VideoEcho() = delete;
    explicit VideoEcho(PresetState& presetState);

    void InitVertexAttrib() override;

	void Draw(PerFrameContext& perFrameContext);
private:
    PresetState& m_presetState; //!< The global preset state.
};
