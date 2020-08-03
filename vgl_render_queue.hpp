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
enum class RenderCommand
{
    /// Framebuffer switch.
    /// - Framebuffer reference.
    FBO,

    /// Clear.
    /// - Color is optional uvec4.
    /// - Depth is optional float.
    /// - Stencil is optional integer.
    CLEAR,

    /// Blend mode setting.
    /// - Blending mode.
    BLEND_MODE,

    /// Cull face setting.
    /// - Culling mode.
    CULL_FACE,

    /// Depth test setting.
    /// - Testing mode.
    /// - Write enable.
    DEPTH_TEST,

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

/// RenderCommand or operation
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Operation result.
RenderCommand operator|(const RenderCommand& lhs, const RenderCommand& rhs)
{
    return static_cast<RenderCommand>(static_cast<int>(lhs) | static_cast<int>(rhs));
}
/// RenderCommand or into operation
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Operation result.
RenderCommand& operator|=(RenderCommand& lhs, const RenderCommand& rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

/// Get a render value type for an uniform type.
///
/// \return Render value type.
template<typename T> constexpr RenderCommand get_uniform_render_command_type() noexcept;
/// \cond
template<> constexpr RenderCommand get_uniform_render_command_type<int>() noexcept
{
    return RenderCommand::UNIFORM_INT;
}
template<> constexpr RenderCommand get_uniform_render_command_type<float>() noexcept
{
    return RenderCommand::UNIFORM_FLOAT;
}
template<> constexpr RenderCommand get_uniform_render_command_type<vec2>() noexcept
{
    return RenderCommand::UNIFORM_VEC2;
}
template<> constexpr RenderCommand get_uniform_render_command_type<vec3>() noexcept
{
    return RenderCommand::UNIFORM_VEC3;
}
template<> constexpr RenderCommand get_uniform_render_command_type<vec4>() noexcept
{
    return RenderCommand::UNIFORM_VEC4;
}
template<> constexpr RenderCommand get_uniform_render_command_type<mat2>() noexcept
{
    return RenderCommand::UNIFORM_MAT2;
}
template<> constexpr RenderCommand get_uniform_render_command_type<mat3>() noexcept
{
    return RenderCommand::UNIFORM_MAT3;
}
template<> constexpr RenderCommand get_uniform_render_command_type<mat4>() noexcept
{
    return RenderCommand::UNIFORM_MAT4;
}
template<> constexpr RenderCommand get_uniform_render_command_type<Texture>() noexcept
{
    return RenderCommand::UNIFORM_TEXTURE;
}
/// \endcond

/// \cond
const int& get_uniform_push_value_type(const int& op)
{
    return op;
}
const float& get_uniform_push_value_type(const float& op)
{
    return op;
}
const vec2& get_uniform_push_value_type(const vec2& op)
{
    return op;
}
const vec3& get_uniform_push_value_type(const vec3& op)
{
    return op;
}
const vec4& get_uniform_push_value_type(const vec4& op)
{
    return op;
}
const mat2& get_uniform_push_value_type(const mat2& op)
{
    return op;
}
const mat3& get_uniform_push_value_type(const mat3& op)
{
    return op;
}
const mat4& get_uniform_push_value_type(const mat4& op)
{
    return op;
}
const Texture* get_uniform_push_value_type(const Texture& op)
{
    return &op;
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

        /// Current normal matrix.
        const mat3* m_normal;

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
            m_normal = nullptr;
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
            m_program->uniform(UniformSemantic::NORMAL, *m_normal);
            m_program->uniform(UniformSemantic::CAMERA_MODELVIEW, *m_camera_modelview);
            m_program->uniform(UniformSemantic::PROJECTION_CAMERA_MODELVIEW, *m_projection_camera_modelview);

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

            if(semantic != UniformSemantic::NONE)
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
        void draw(const RenderQueue& op)
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
                case detail::RenderCommand::FBO:
                    {
                        applyMesh();
                        FrameBuffer* fbo = iter.read<FrameBuffer*>();
                        fbo->bind();
                    }
                    break;

                case detail::RenderCommand::CLEAR:
                    {
                        optional<uvec4> color;
                        if(iter.read<int>())
                        {
                            color = iter.read<uvec4>();
                        }
                        optional<float> depth;
                        if(iter.read<int>())
                        {
                            depth = iter.read<float>();
                        }
                        optional<uint8_t> stencil;
                        if(iter.read<int>())
                        {
                            stencil = static_cast<uint8_t>(iter.read<int>());
                        }
                        clear_buffers(color, depth, stencil);
                    }
                    break;

                case detail::RenderCommand::BLEND_MODE:
                    {
                        OperationMode mode = static_cast<OperationMode>(iter.read<int>());
                        blend_mode(mode);
                    }
                    break;

                case detail::RenderCommand::CULL_FACE:
                    {
                        GLenum mode = static_cast<GLenum>(iter.read<int>());
                        cull_face(mode);
                    }
                    break;

                case detail::RenderCommand::DEPTH_TEST:
                    {
                        GLenum mode = static_cast<GLenum>(iter.read<int>());
                        bool write = static_cast<bool>(iter.read<int>());
                        depth_test(mode);
                        depth_write(write);
                    }
                    break;

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
                    m_normal = &(iter.read<mat3>());
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
    template<typename T> void pushUniform(string_view name, UniformSemantic semantic, const T& value)
    {
        m_data.push(static_cast<int>(detail::get_uniform_render_command_type<T>()));
        m_data.push(name);
        m_data.push(static_cast<int>(semantic));
        m_data.push(detail::get_uniform_push_value_type(value));
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

    /// Push fbo switch.
    ///
    /// \param op Framebuffer ptr.
    void push(const FrameBuffer& op)
    {
        m_data.push(static_cast<int>(detail::RenderCommand::FBO));
        m_data.push(&op);
    }

    /// Push clear.
    ///
    /// \param color Optional clear color.
    /// \param depth Optional clear depth.
    /// \param stencil Optional clear stencil.
    void push(optional<uvec4> color, optional<float> depth, optional<uint8_t> stencil = nullopt)
    {
        m_data.push(static_cast<int>(detail::RenderCommand::CLEAR));
        {
            int color_enable = static_cast<int>(static_cast<bool>(color));
            if(color_enable)
            {
                m_data.push(static_cast<int>(1));
                m_data.push(*color);
            }
        }
        {
            int depth_enable = static_cast<int>(static_cast<bool>(depth));
            m_data.push(depth_enable);
            if(depth_enable)
            {
                m_data.push(*depth);
            }
        }
        {
            int stencil_enable = static_cast<int>(static_cast<bool>(stencil));
            m_data.push(stencil_enable);
            if(stencil)
            {
                m_data.push(static_cast<int>(*stencil));
            }
        }
    }

    /// Push view settings switch.
    ///
    /// \param proj Projection matrix.
    /// \param cam Camera matrix.
    void push(const mat4& proj, const mat4& cam)
    {
        mat4 camera = viewify(cam);
        m_camera = camera;
#if defined(USE_LD)
        m_projection_camera = proj * (*m_camera);
#else
        m_projection_camera = proj * m_camera;
#endif

        m_data.push(static_cast<int>(detail::RenderCommand::VIEW));
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
        m_data.push(static_cast<int>(detail::RenderCommand::PROGRAM));
        m_data.push(&op);
    }

    /// Push mesh render.
    ///
    /// \param msh Mesh to render.
    /// \param modelview Mesh modelview matrix.
    void push(const Mesh& msh, const mat4& modelview)
    {
        m_data.push(static_cast<int>(detail::RenderCommand::MESH));
        m_data.push(&msh);
        m_data.push(modelview);
        m_data.push(normalify(modelview));
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

    /// Push blend mode switch.
    ///
    /// \param op Blend mode.
    void pushBlend(OperationMode op)
    {
        m_data.push(static_cast<int>(detail::RenderCommand::BLEND_MODE));
        m_data.push(static_cast<int>(op));
    }

    /// Push cull mode switch.
    ///
    /// \param op Cull mode.
    void pushCull(GLenum op)
    {
        m_data.push(static_cast<int>(detail::RenderCommand::CULL_FACE));
        m_data.push(static_cast<int>(op));
    }

    /// Push depth test switch.
    ///
    /// \param mode Depth test mode.
    /// \param write Depth write enabled.
    void pushDepth(GLenum mode, bool write)
    {
        m_data.push(static_cast<int>(detail::RenderCommand::DEPTH_TEST));
        m_data.push(static_cast<int>(mode));
        m_data.push(static_cast<int>(write));
    }

    /// Render the contents in the queue.
    void draw() const
    {
        RenderState state;
        state.draw(*this);
    }
};

}

#endif
