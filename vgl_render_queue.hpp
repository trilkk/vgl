#ifndef VGL_RENDER_QUEUE_HPP
#define VGL_RENDER_QUEUE_HPP

#include "vgl_mat4.hpp"
#include "vgl_mesh.hpp"

namespace vgl
{

/// Render command queue contains precalculated rendering commands to send meshes to GPU.
///
/// Only objects with no constructors may be added to the rendering queue.
class RenderQueue
{
private:
    /// Semantic for elements.
    ///
    /// All uniform data follows the following data order:
    /// - Enumeration value (type).
    /// - Semantic.
    /// - Name.
    /// - Value.
    enum RenderValueType
    {
        /// View switch.
        /// - projection matrix and camera matrix.
        VIEW,

        /// Program switch.
        PROGRAM,

        /// Mesh render.
        MESH,

        /// Uniform data (int).
        UNIFORM_INT,

        /// Uniform data (float).
        UNIFORM_FLOAT,

        /// Uniform data (vec2).
        UNIFORM_VEC2,

        /// Uniform data (vec3).
        UNIFORM_VEC3,

        /// Uniform data (vec4).
        UNIFORM_VEC4,

        /// Uniform data (mat2).
        UNIFORM_MAT2,

        /// Uniform data (mat3).
        UNIFORM_MAT3,

        /// Uniform data (mat4).
        UNIFORM_MAT4,

        /// Uniform data (texture).
        UNIFORM_TEXTURE,
    };

private:
    /// Rendering data.
    vector<uint8_t> m_data;

    /// Current camera settings.
#if defined(USE_LD)
    optional<mat4> m_camera;
#else
    mat4 m_camera;
#endif

    /// Current projection + camera settings.
#if defined(USE_LD)
    optional<mat4> m_projection_camera;
#else
    mat4 m_projection_camera;
#endif

public:
    /// Constructor.
    constexpr explicit RenderQueue() = default noexcept;

public:
    /// Clear the render queue.
    ///
    /// After clearing, the queue is ready to receive settings for a new frame.
    void clear()
    {
        m_data.clear();
#if defined(USE_LD)
        m_camera.reset();
        m_projection_camera.reset;
#endif
    }

    /// Push view settings switch.
    ///
    /// \param proj Projection matrix.
    /// \param cam Camera matrix.
    void pushView(const mat4& proj, const mat4& cam)
    {
        m_camera = vgl::viewify(cam);
#if defined(USE_LD)
        m_projection_camera = proj * (*m_camera);
#else
        m_projection_camera = proj * m_camera;
#endif

        addData(RenderValueType::VIEW);
        addData(proj);
#if defined(USE_LD)
        addData(*m_camera);
        addData(*m_projection_camera);
#else
        addData(m_camera);
        addData(m_projection_camera);
#endif
    }

    /// Push program switch.
    ///
    /// \param op Program to use starting from this point.
    void push(const GlslProgram& op)
    {
        addData(RenderValueType::PROGRAM);
        addData(&op);
    }

    /// Push mesh render.
    ///
    /// \param msh Mesh to render.
    /// \param modelview Mesh modelview matrix.
    void push(const Mesh& msh, const mat4& modelview)
    {
        addData(RenderValueType::MESH);
        addData(&msh);
        addData(modelview);
    }
};

}

#endif
