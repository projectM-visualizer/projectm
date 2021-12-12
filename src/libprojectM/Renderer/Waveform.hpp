/*
 * Waveform.hpp
 *
 *  Created on: Jun 25, 2008
 *      Author: pete
 */

#ifndef WAVEFORM_HPP_
#define WAVEFORM_HPP_

#include "RenderItem.hpp"

#include <vector>


class Waveform : public RenderItem
{
public:
    struct ColoredPoint
    {
        float x{ 0.5f }; //!< Vertex X coordinate.
        float y{ 0.5f }; //!< Vertex Y coordinate.
        float r{ 1.0f }; //!< Vertex red color value.
        float g{ 1.0f }; //!< Vertex green color value.
        float b{ 1.0f }; //!< Vertex blue color value.
        float a{ 1.0f }; //!< Vertex alpha value.
    };

    class Context
    {
    public:
        float sample{ 0.0f }; //!< Current sample value.
        int samples{ 0 }; //!< Sample number
        int sample_int{ 0 }; //!< Sample integer value.
        float left{ 0.0f }; //!< Left channel value.
        float right{ 0.0f }; //!< Right channe value.
        BeatDetect* music{ nullptr }; //!< Beat detection instance.

        /**
         * @brief Create a new context instance.
         * @param _samples Number of samples.
         * @param _music Beat detection instance, also used to access sample PCM data.
         */
        Context(int _samples, BeatDetect* _music)
            : samples(_samples)
            , music(_music)
        {
        }
    };

    /**
     * @brief Creates a new waveform with the given number of samples.
     * @param _samples The number of samples this waveform should display.
     */
    explicit Waveform(int _samples);

    /**
     * @brief Initializes the waveform's vertex buffer and attribute data.
     */
    void InitVertexAttrib() override;

    /**
     * @brief Renders the waveform.
     * @param context The rendering context instance.
     */
    void Draw(RenderContext& context) override;

    int samples{ 0 }; //!< Number of samples associated with this wave form. Usually powers of 2.
    bool spectrum{ false }; //!< Spectrum data or PCM data.
    bool dots{ false }; //!< If true, draw wave as dots instead of lines.
    bool thick{ false }; //!< Draw thicker lines.
    bool additive{ false }; //!< Add color values together.

    float scaling{ 1.0f }; //!< Scale factor of waveform.
    float smoothing{ 0.0f }; //!< Smooth factor of waveform.
    int sep{ 0 };  //!< Separation distance of dual waveforms.


protected:

    /**
     * @brief Does a better-than-linear smooth on a wave.
     *
     * Roughly doubles the number of points.
     *
     * @param inputVertices Pointer to an array of vertices to be smoothed.
     * @param vertexCount Number of vertices/points in the input data.
     * @param outputVertices Pointer to a buffer that will receive the smoothed data. Must be able to hold 2 * vertexCount vertices.
     * @return The number of vertices in outputVertices after smoothing.
     */
    static int SmoothWave(const ColoredPoint* inputVertices, int vertexCount, ColoredPoint* outputVertices);

    virtual ColoredPoint PerPoint(ColoredPoint p, const Context& context) = 0;

    std::vector<ColoredPoint> points; //!< Points in this waveform.
};

#endif /* WAVEFORM_HPP_ */
