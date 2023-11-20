#ifndef VGL_RENDER_QUEUE_HPP
#define VGL_RENDER_QUEUE_HPP

#include "vgl_font.hpp"
#include "vgl_frame_buffer.hpp"
#include "vgl_mesh.hpp"
#include "vgl_packed_data_reader.hpp"

namespace vgl
{

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
        /// Render command function type.
        using RenderCommandFunc = void (*)(RenderState&, PackedDataReader&);

    private:
        /// Current projection matrix.
        const mat4* VGL_VOLUNTARY_MEMBER_VALUE(m_projection_matrix, nullptr);

        /// Current camera depth range.
        const vec2* VGL_VOLUNTARY_MEMBER_VALUE(m_projection_range, nullptr);

        /// Current camera matrix.
        const mat4* VGL_VOLUNTARY_MEMBER_VALUE(m_camera_matrix, nullptr);

        /// Current projection camera matrix.
        const mat4* VGL_VOLUNTARY_MEMBER_VALUE(m_projection_camera_matrix, nullptr);

        /// Current modelview matrix.
        const mat4* VGL_VOLUNTARY_MEMBER_VALUE(m_modelview_matrix, nullptr);

        /// Current normal matrix.
        const mat3* VGL_VOLUNTARY_MEMBER_VALUE(m_normal_matrix, nullptr);

        /// Current camera modelview matrix.
        const mat4* VGL_VOLUNTARY_MEMBER_VALUE(m_camera_modelview_matrix, nullptr);

        /// Current projection camera modelview matrix.
        const mat4* VGL_VOLUNTARY_MEMBER_VALUE(m_projection_camera_modelview_matrix, nullptr);

        /// Current camera position.
        const vec3* VGL_VOLUNTARY_MEMBER_VALUE(m_camera_position, nullptr);

        /// Current program.
        GlslProgram* VGL_VOLUNTARY_MEMBER_VALUE(m_program, nullptr);

        /// Current mesh.
        Mesh* m_mesh = nullptr;

        /// Current texture unit.
        unsigned m_texture_unit = 0;

        /// Current persistent texture unit.
        unsigned m_texture_unit_persistent = 0;

    public:
        /// Constructor.
        explicit RenderState() = default;

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
            if(!m_program)
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("cannot apply mesh before a program has been set"));
            }
            if(!m_projection_matrix)
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("cannot apply mesh before view settings have been read"));
            }
            VGL_ASSERT(m_projection_range);
            VGL_ASSERT(m_camera_matrix);
            VGL_ASSERT(m_camera_position);
#endif

            // Apply projection uniforms based on semantic only.
            m_program->uniform(UniformSemantic::PROJECTION_MATRIX, *m_projection_matrix);
            m_program->uniform(UniformSemantic::PROJECTION_RANGE, *m_projection_range);
            m_program->uniform(UniformSemantic::CAMERA_MATRIX, *m_camera_matrix);
            m_program->uniform(UniformSemantic::PROJECTION_CAMERA_MATRIX, *m_projection_camera_matrix);
            m_program->uniform(UniformSemantic::MODELVIEW_MATRIX, *m_modelview_matrix);
            m_program->uniform(UniformSemantic::NORMAL_MATRIX, *m_normal_matrix);
            m_program->uniform(UniformSemantic::CAMERA_MODELVIEW_MATRIX, *m_camera_modelview_matrix);
            m_program->uniform(UniformSemantic::PROJECTION_CAMERA_MODELVIEW_MATRIX, *m_projection_camera_modelview_matrix);
            m_program->uniform(UniformSemantic::CAMERA_POSITION, *m_camera_position);

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

    public:
        /// Blend mode command.
        ///
        /// \param iter Packed data iterator.
        void commandBlend(PackedDataReader& iter)
        {
            OperationMode mode = static_cast<OperationMode>(iter.read<int>());
            blend_mode(mode);
        }

        /// Clear command.
        ///
        /// \param iter Packed data iterator.
        void commandClear(PackedDataReader& iter)
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
#if !defined(VGL_DISABLE_STENCIL)
            optional<uint8_t> stencil;
            if(iter.read<int>())
            {
                stencil = static_cast<uint8_t>(iter.read<int>());
            }
#endif
            clear_buffers(color, depth
#if !defined(VGL_DISABLE_STENCIL)
                    , stencil
#endif
                    );
        }

        /// Cull face command.
        ///
        /// \param iter Packed data iterator.
        void commandCullFace(PackedDataReader& iter)
        {
            GLenum mode = static_cast<GLenum>(iter.read<int>());
            cull_face(mode);
        }

        /// Depth test command.
        ///
        /// \param iter Packed data iterator.
        void commandDepthTest(PackedDataReader& iter)
        {
            GLenum mode = static_cast<GLenum>(iter.read<int>());
            bool write = static_cast<bool>(iter.read<int>());
            depth_test(mode);
            depth_write(write);
        }

        /// FBO command.
        ///
        /// \param iter Packed data iterator.
        void commandFbo(PackedDataReader& iter)
        {
            applyMesh();
            FrameBuffer* fbo = iter.read<FrameBuffer*>();
            fbo->bind();
        }

        /// Mesh command.
        ///
        /// \param iter Packed data iterator.
        void commandMesh(PackedDataReader& iter)
        {
            applyMesh();
            m_mesh = iter.read<Mesh*>();
            m_modelview_matrix = &(iter.read<mat4>());
            m_normal_matrix = &(iter.read<mat3>());
            m_camera_modelview_matrix = &(iter.read<mat4>());
            m_projection_camera_modelview_matrix = &(iter.read<mat4>());
        }

        /// Program command.
        ///
        /// \param iter Packed data iterator.
        void commandProgram(PackedDataReader& iter)
        {
            applyMesh();
            m_program = iter.read<GlslProgram*>();
            m_program->bind();
            m_texture_unit_persistent = 0;
        }

        /// Uniform (texture) command.
        ///
        /// \param iter Packed data iterator.
        void commandUniformTexture(PackedDataReader& iter)
        {
            GLint location = getUniformLocation(iter);
            Texture* tex = iter.read<Texture*>();
            GlslProgram::applyUniform(location, *tex, m_texture_unit_persistent + m_texture_unit);
            ++m_texture_unit;
        }

        /// Uniform (texture, persistent) command.
        ///
        /// \param iter Packed data iterator.
        void commandUniformTexturePersistent(PackedDataReader& iter)
        {
            GLint location = getUniformLocation(iter);
            Texture* tex = iter.read<Texture*>();
            GlslProgram::applyUniform(location, *tex, m_texture_unit_persistent);
            ++m_texture_unit_persistent;
        }

        /// View command.
        ///
        /// \param iter Packed data iterator.
        void commandView(PackedDataReader& iter)
        {
            applyMesh();
            m_projection_matrix = &(iter.read<mat4>());
            m_projection_range = &(iter.read<vec2>());
            m_camera_matrix = &(iter.read<mat4>());
            m_projection_camera_matrix = &(iter.read<mat4>());
            m_camera_position = &(iter.read<vec3>());
        }

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
                if(iter.remaining() < sizeof(void*))
                {
                    break;
                }

                // Take an action based on value type extracted.
                RenderCommandFunc fptr = reinterpret_cast<RenderCommandFunc>(iter.read<void*>());
                fptr(*this, iter);
            }

            // Apply last mesh that may be remaining.
            applyMesh();
        }

    public:
        /// Uniform command.
        ///
        /// \param iter Read iterator.
        template<typename T> void commandUniform(PackedDataReader& iter)
        {
            GLint location = getUniformLocation(iter);
            unsigned count = static_cast<unsigned>(iter.read<int>());
            GlslProgram::applyUniform(location, &(iter.read<T>(count)), count);
        }
    };

private:
    /// Rendering data.
    PackedData m_data;

    /// Current camera settings.
#if defined(USE_LD)
    optional<mat4> m_camera_matrix;
#else
    mat4 m_camera_matrix;
#endif

    /// Current projection + camera settings.
#if defined(USE_LD)
    optional<mat4> m_projection_camera_matrix;
#else
    mat4 m_projection_camera_matrix;
#endif

    /// Current camera position.
#if defined(USE_LD)
    optional<vec3> m_camera_position;
#else
    vec3 m_camera_position;
#endif

public:
    /// Constructor.
    constexpr explicit RenderQueue() noexcept = default;

private:
    /// Push texture uniform.
    ///
    /// \param name Name of the uniform.
    /// \param value Texture value.
    void pushUniformTexture(string_view name, UniformSemantic semantic, const Texture2D& value)
    {
        pushCommand<&RenderState::commandUniformTexture>();
        m_data.push(name);
        m_data.push(static_cast<int>(semantic));
        m_data.push(&value);
    }

    /// Push persistent texture uniform.
    ///
    /// \param name Name of the uniform.
    /// \param value Texture value.
    void pushUniformTexturePersistent(string_view name, UniformSemantic semantic, const Texture2D& value)
    {
        pushCommand<&RenderState::commandUniformTexturePersistent>();
        m_data.push(name);
        m_data.push(static_cast<int>(semantic));
        m_data.push(&value);
    }

private:
    /// Push uniform.
    ///
    /// \param name Name of the uniform.
    /// \param semantic Uniform semantic.
    /// \param ptr Pointer to uniform value array.
    /// \param count Number of uniforms to push.
    template<typename T> void pushUniform(string_view name, UniformSemantic semantic, const T* ptr, unsigned count)
    {
        pushCommand<&RenderState::commandUniform<T>>();
        m_data.push(name);
        m_data.push(static_cast<int>(semantic));
        m_data.push(static_cast<int>(count));
        for(unsigned ii = 0; (ii < count); ++ii)
        {
            m_data.push(ptr[ii]);
        }
    }

public:
    /// Clear the render queue.
    ///
    /// After clearing, the queue is ready to receive settings for a new frame.
    void clear()
    {
        m_data.clear();
#if defined(USE_LD)
        m_camera_matrix = nullopt;
        m_projection_camera_matrix = nullopt;
        m_camera_position = nullopt;
#endif
    }

    /// Render the contents in the queue.
    void draw() const
    {
        RenderState state;
        state.draw(*this);
    }

    /// Push fbo switch.
    ///
    /// \param op Framebuffer ptr.
    void push(const FrameBuffer& op)
    {
        pushCommand<&RenderState::commandFbo>();
        m_data.push(&op);
    }

    /// Push mesh render.
    ///
    /// \param msh Mesh to render.
    /// \param modelview Mesh modelview matrix.
    void push(const Mesh& msh, const mat4& modelview)
    {
        pushCommand<&RenderState::commandMesh>();
        m_data.push(&msh);
        m_data.push(modelview);
        m_data.push(normalify(modelview));
#if defined(USE_LD)
        m_data.push((*m_camera_matrix) * modelview);
        m_data.push((*m_projection_camera_matrix) * modelview);
#else
        m_data.push(m_camera_matrix * modelview);
        m_data.push(m_projection_camera_matrix * modelview);
#endif
    }

    /// Push program switch.
    ///
    /// \param op Program to use starting from this point.
    void push(const GlslProgram& op)
    {
        pushCommand<&RenderState::commandProgram>();
        m_data.push(&op);
    }

    /// Push view settings switch.
    ///
    /// \param proj Projection matrix.
    /// \param range Projection depth range.
    /// \param cam Camera matrix.
    void push(const mat4& proj, const vec2 range, const mat4& cam)
    {
        mat4 camera = viewify(cam);
        m_camera_matrix = camera;
#if defined(USE_LD)
        m_projection_camera_matrix = proj * (*m_camera_matrix);
#else
        m_projection_camera_matrix = proj * m_camera_matrix;
#endif
        m_camera_position = cam.getTranslation();

        pushCommand<&RenderState::commandView>();
        m_data.push(proj);
        m_data.push(range);
#if defined(USE_LD)
        m_data.push(*m_camera_matrix);
        m_data.push(*m_projection_camera_matrix);
        m_data.push(*m_camera_position);
#else
        m_data.push(m_camera_matrix);
        m_data.push(m_projection_camera_matrix);
        m_data.push(m_camera_position);
#endif
    }

    /// Push blend mode switch.
    ///
    /// \param op Blend mode.
    void pushBlend(OperationMode op)
    {
        pushCommand<&RenderState::commandBlend>();
        m_data.push(static_cast<int>(op));
    }

    /// Push clear.
    ///
    /// \param color Optional clear color.
    /// \param depth Optional clear depth.
    /// \param stencil Optional clear stencil.
    void pushClear(optional<uvec4> color, optional<float> depth
#if !defined(VGL_DISABLE_STENCIL)
            , optional<uint8_t> stencil = nullopt
#endif
            )
    {
        pushCommand<&RenderState::commandClear>();
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
#if !defined(VGL_DISABLE_STENCIL)
        {
            int stencil_enable = static_cast<int>(static_cast<bool>(stencil));
            m_data.push(stencil_enable);
            if(stencil)
            {
                m_data.push(static_cast<int>(*stencil));
            }
        }
#endif
    }

    /// Push cull mode switch.
    ///
    /// \param op Cull mode.
    void pushCull(GLenum op)
    {
        pushCommand<&RenderState::commandCullFace>();
        m_data.push(static_cast<int>(op));
    }

    /// Push depth test switch.
    ///
    /// \param mode Depth test mode.
    /// \param write Depth write enabled.
    void pushDepth(GLenum mode
#if !defined(VGL_DISABLE_DEPTH_WRITE)
            , bool write = true
#endif
            )
    {
        pushCommand<&RenderState::commandDepthTest>();
        m_data.push(static_cast<int>(mode));
#if !defined(VGL_DISABLE_DEPTH_WRITE)
        m_data.push(static_cast<int>(write));
#endif
    }

    /// Push texture uniform.
    ///
    /// \param name Name of the uniform.
    /// \param value Texture value.
    void push(string_view name, const Texture2D& value)
    {
        pushUniformTexture(name, UniformSemantic::NONE, value);
    }
    /// Push texture uniform.
    ///
    /// \param name Name of the uniform.
    /// \param value Texture value.
    void push(UniformSemantic semantic, const Texture2D& value)
    {
        pushUniformTexture(string_view(), semantic, value);
    }

    /// Push persistent texture uniform.
    ///
    /// \param name Name of the uniform.
    /// \param value Texture value.
    void pushPersistent(string_view name, const Texture2D& value)
    {
        pushUniformTexturePersistent(name, UniformSemantic::NONE, value);
    }
    /// Push persistent texture uniform.
    ///
    /// \param name Name of the uniform.
    /// \param value Texture value.
    void pushPersistent(UniformSemantic semantic, const Texture2D& value)
    {
        pushUniformTexturePersistent(string_view(), semantic, value);
    }

public:
    /// Push uniform.
    ///
    /// \param name Name of the uniform.
    /// \param value Value of the uniform.
    template<typename T> void push(string_view name, const T& value)
    {
        pushUniform(name, UniformSemantic::NONE, &value, 1);
    }
    /// Push uniform.
    ///
    /// \param semantic Uniform semantic.
    /// \param value Value of the uniform.
    template<typename T> void push(UniformSemantic semantic, const T& value)
    {
        pushUniform(string_view(), semantic, &value, 1);
    }
    /// Push uniform array.
    ///
    /// \param name Name of the uniform.
    /// \param ptr Pointer to uniform array.
    /// \param count Number of matrices.
    template<typename T> void push(string_view name, const T* ptr, unsigned count)
    {
        pushUniform(name, UniformSemantic::NONE, ptr, count);
    }
    /// Push uniform array.
    ///
    /// \param semantic Uniform semantic.
    /// \param ptr Pointer to uniform array.
    /// \param count Number of matrices.
    template<typename T> void push(UniformSemantic semantic, const T* ptr, unsigned count)
    {
        pushUniform(string_view(), semantic, ptr, count);
    }

    /// Templated push of a command.
    template<void (RenderState::*F)(PackedDataReader&)> void pushCommand()
    {
        m_data.push(reinterpret_cast<void*>(commandFunc<F>));
    }

private:
    /// Command function template.
    ///
    /// \param state Render state.
    /// \param iter Read iterator.
    template<void (RenderState::*F)(PackedDataReader&)> static void commandFunc(RenderState& state, PackedDataReader& iter)
    {
        (state.*F)(iter);
    }
};

}

#endif
