#ifndef VGL_RENDER_QUEUE_HPP
#define VGL_RENDER_QUEUE_HPP

#include "vgl_mesh.hpp"
#include "vgl_packed_data_reader.hpp"

namespace vgl
{

namespace detail
{

/// Semantic for elements in render queue.
///
/// All uniform data follows the following data order:
/// - Enumeration value (type).
/// - Semantic.
/// - Name.
/// - Value.
///
/// Other commands have specific formats.
enum RenderCommand
{
    /// View switch.
    /// - Projection matrix.
    /// - Camera matrix.
    /// - Projection camera matrix.
    VIEW,

    /// Program switch.
    /// - Program reference.
    PROGRAM,

    /// Mesh render.
    /// - Mesh reference.
    /// - Modelview matrix.
    /// - Camera modelview matrix.
    /// - Projection camera modelview matrix.
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

/// Get a render value type for an uniform type.
///
/// \return Render value type.
template<typename T> constexpr RenderCommand getUniformRenderCommandType() noexcept;
/// \cond
template<> constexpr RenderCommand getUniformRenderCommandType<int>() noexcept
{
    return UNIFORM_INT;
}
template<> constexpr RenderCommand getUniformRenderCommandType<float>() noexcept
{
    return UNIFORM_FLOAT;
}
template<> constexpr RenderCommand getUniformRenderCommandType<vec2>() noexcept
{
    return UNIFORM_VEC2;
}
template<> constexpr RenderCommand getUniformRenderCommandType<vec3>() noexcept
{
    return UNIFORM_VEC3;
}
template<> constexpr RenderCommand getUniformRenderCommandType<vec4>() noexcept
{
    return UNIFORM_VEC4;
}
template<> constexpr RenderCommand getUniformRenderCommandType<mat2>() noexcept
{
    return UNIFORM_MAT2;
}
template<> constexpr RenderCommand getUniformRenderCommandType<mat3>() noexcept
{
    return UNIFORM_MAT3;
}
template<> constexpr RenderCommand getUniformRenderCommandType<mat4>() noexcept
{
    return UNIFORM_MAT4;
}
template<> constexpr RenderCommand getUniformRenderCommandType<Texture>() noexcept
{
    return UNIFORM_TEXTURE;
}
/// \endcond

}

/// Render command queue contains precalculated rendering commands to send meshes to GPU.
///
/// Only objects with no constructors may be added to the rendering queue.
class RenderQueue
{
private:
    /// Internal state for render operation.
    class RenderState
    {
    private:
        /// Current projection matrix.
        const mat4* m_projection;

        /// Current camera matrix.
        const mat4* m_camera;

        /// Current projection camera matrix.
        const mat4* m_projection_camera;

        /// Current modelview matrix.
        const mat4* m_modelview;

        /// Current camera modelview matrix.
        const mat4* m_camera_modelview;

        /// Current projection camera modelview matrix.
        const mat4* m_projection_camera_modelview;

        /// Current program.
        GlslProgram* m_program;

        /// Current mesh.
        Mesh* m_mesh = nullptr;

        /// Current texture unit.
        unsigned m_texture_unit = 0;

    public:
        /// Constructor.
        ///
        /// \param op Render queue to render.
        explicit RenderState()
        {
#if defined(USE_LD)
            m_projection = nullptr;
            m_camera = nullptr;
            m_projection_camera = nullptr;
            m_modelview = nullptr;
            m_camera_modelview = nullptr;
            m_projection_camera_modelview = nullptr;
            m_program = nullptr;
#endif
        }

    private:
        /// Apply a mesh.
        ///
        /// Done when encountering a new render value type
        /// Clears the mesh state.
        void applyMesh()
        {
            if(!m_mesh)
            {
                return;
            }

#if defined(USE_LD)
            if(!m_projection)
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("cannot apply mesh before view settings have been read"));
            }
#endif

            // Apply projection uniforms based on semantic only.
            m_program->uniform(UniformSemantic::PROJECTION, *m_projection);
            m_program->uniform(UniformSemantic::CAMERA, *m_camera);
            m_program->uniform(UniformSemantic::PROJECTION_CAMERA, *m_projection_camera);
            m_program->uniform(UniformSemantic::MODELVIEW, *m_modelview);
            m_program->uniform(UniformSemantic::CAMERA_MODELVIEW, *m_camera_modelview);
            m_program->uniform(UniformSemantic::PROJECTION_CAMERA_MODELVIEW, *m_camera_modelview);

            m_mesh->draw(*m_program);
            m_mesh = nullptr;
            m_texture_unit = 0;
        }

        /// Get uniform location from packed data reader.
        ///
        /// \param iter Read iterator.
        /// \return Uniform location.
        GLint getUniformLocation(PackedDataReader& iter)
        {
#if defined(USE_LD)
            if(!m_program)
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("cannot get uniform locations before a program has been read"));
            }
#endif

            string_view name = iter.read<string_view>();
            UniformSemantic semantic = iter.read<UniformSemantic>();

            if(semantic != NONE)
            {
                return m_program->getUniformLocation(semantic);
            }
            return m_program->getUniformLocation(name);
        }

        /// Apply uniform.
        ///
        /// \param iter Read iterator.
        template<typename T> void applyUniform(PackedDataReader& iter)
        {
            GLint location = getUniformLocation(iter);
            GlslProgram::applyUniform(location, iter.read<T>());
        }

    public:
        /// Render using the packed data reader.
        ///
        /// \param op Render queue to render.
        void render(const RenderQueue& op)
        {
            /// Packed data reader.
            PackedDataReader iter(op.m_data);

            for(;;)
            {
                // Abort if no data remaining.
                if(iter.remaining() < sizeof(detail::RenderCommand))
                {
                    break;
                }

                // Take an action based on value type extracted.
                detail::RenderCommand command = iter.read<detail::RenderCommand>();
                switch(command)
                {
                case detail::RenderCommand::VIEW:
                    applyMesh();
                    m_projection = &(iter.read<mat4>());
                    m_camera = &(iter.read<mat4>());
                    m_projection_camera = &(iter.read<mat4>());
                    break;

                case detail::RenderCommand::PROGRAM:
                    applyMesh();
                    m_program = iter.read<GlslProgram*>();
                    m_program->bind();
                    break;

                case detail::RenderCommand::MESH:
                    applyMesh();
                    m_mesh = iter.read<Mesh*>();
                    m_modelview = &(iter.read<mat4>());
                    m_camera_modelview = &(iter.read<mat4>());
                    m_projection_camera_modelview = &(iter.read<mat4>());
                    break;

                case detail::RenderCommand::UNIFORM_INT:
                    applyUniform<int>(iter);
                    break;

                case detail::RenderCommand::UNIFORM_FLOAT:
                    applyUniform<float>(iter);
                    break;

                case detail::RenderCommand::UNIFORM_VEC2:
                    applyUniform<vec2>(iter);
                    break;

                case detail::RenderCommand::UNIFORM_VEC3:
                    applyUniform<vec3>(iter);
                    break;

                case detail::RenderCommand::UNIFORM_VEC4:
                    applyUniform<vec4>(iter);
                    break;

                case detail::RenderCommand::UNIFORM_MAT2:
                    applyUniform<mat2>(iter);
                    break;

                case detail::RenderCommand::UNIFORM_MAT3:
                    applyUniform<mat3>(iter);
                    break;

                case detail::RenderCommand::UNIFORM_MAT4:
                    applyUniform<mat4>(iter);
                    break;

                    // Texture uniforms are stateful and require more work.
                case detail::RenderCommand::UNIFORM_TEXTURE:
#if !defined(USE_LD)
                default:
#endif
                    {
                        GLint location = getUniformLocation(iter);
                        Texture* tex = iter.read<Texture*>();
                        GlslProgram::applyUniform(location, *tex, m_texture_unit);
                        ++m_texture_unit;
                    }
                    break;

#if defined(USE_LD)
                default:
                    BOOST_THROW_EXCEPTION(std::runtime_error("unknown render command: " +
                                std::to_string(static_cast<int>(command))));
#endif
                }
            }

            // Apply last mesh that may be remaining.
            applyMesh();
        }
    };

private:
    /// Rendering data.
    PackedData m_data;

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
    constexpr explicit RenderQueue() noexcept = default;

private:
    /// Push uniform.
    ///
    /// \param name Name of the uniform.
    /// \param semantic Uniform semantic.
    /// \param value Value of the uniform.
    template<typename T> void push(string_view name, UniformSemantic semantic, const T& value)
    {
        m_data.push(detail::getUniformRenderCommandType<T>());
        m_data.push(name);
        m_data.push(semantic);
        m_data.push(value);
    }

public:
    /// Clear the render queue.
    ///
    /// After clearing, the queue is ready to receive settings for a new frame.
    void clear()
    {
        m_data.clear();
#if defined(USE_LD)
        m_camera = nullopt;
        m_projection_camera = nullopt;
#endif
    }

    /// Push view settings switch.
    ///
    /// \param proj Projection matrix.
    /// \param cam Camera matrix.
    void push(const mat4& proj, const mat4& cam)
    {
        m_camera = vgl::viewify(cam);
#if defined(USE_LD)
        m_projection_camera = proj * (*m_camera);
#else
        m_projection_camera = proj * m_camera;
#endif

        m_data.push(detail::RenderCommand::VIEW);
        m_data.push(proj);
#if defined(USE_LD)
        m_data.push(*m_camera);
        m_data.push(*m_projection_camera);
#else
        m_data.push(m_camera);
        m_data.push(m_projection_camera);
#endif
    }

    /// Push program switch.
    ///
    /// \param op Program to use starting from this point.
    void push(const GlslProgram& op)
    {
        m_data.push(detail::RenderCommand::PROGRAM);
        m_data.push(&op);
    }

    /// Push mesh render.
    ///
    /// \param msh Mesh to render.
    /// \param modelview Mesh modelview matrix.
    void push(const Mesh& msh, const mat4& modelview)
    {
        m_data.push(detail::RenderCommand::MESH);
        m_data.push(&msh);
        m_data.push(modelview);
#if defined(USE_LD)
        m_data.push((*m_camera) * modelview);
        m_data.push((*m_projection_camera) * modelview);
#else
        m_data.push(m_camera * modelview);
        m_data.push(m_projection_camera * modelview);
#endif
    }

    /// Push uniform.
    ///
    /// \param name Name of the uniform.
    /// \param value Value of the uniform.
    template<typename T> void push(string_view name, const T& value)
    {
        pushUniform(name, UniformSemantic::NONE, value);
    }
    /// Push uniform.
    ///
    /// \param semantic Uniform semantic.
    /// \param value Value of the uniform.
    template<typename T> void push(UniformSemantic semantic, const T& value)
    {
        pushUniform(string_view(), semantic, value);
    }

    /// Render the contents in the queue.
    void render()
    {
        RenderState state;
        state.render(*this);
    }
};

}

#endif
