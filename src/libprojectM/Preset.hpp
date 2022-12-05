#pragma once

#include <string>

class BeatDetect;
class Pipeline;
class PipelineContext;

class Preset
{
public:
    virtual ~Preset() = default;

    virtual Pipeline& pipeline() = 0;
    virtual void Render(const BeatDetect& music, const PipelineContext& context) = 0;

    inline void SetFilename(const std::string& filename)
    {
        m_filename = filename;
    }

    inline const std::string& Filename() const
    {
        return m_filename;
    }

private:
    std::string m_filename;
};
