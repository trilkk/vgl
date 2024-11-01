#ifndef VGL_TEXTURE_HPP
#define VGL_TEXTURE_HPP

#include "vgl_extern_opengl.hpp"

#if defined(USE_LD)
#include "vgl_throw_exception.hpp"
#endif

namespace vgl
{

namespace detail
{

#if !defined(VGL_DISABLE_ANISOTROPY) || !VGL_DISABLE_ANISOTROPY

/// Anisotropy level for textures.
static const float ANISOTROPY_LEVEL = 4.0f;

#endif

}

/// Filtering mode.
enum class FilteringMode
{
  /// Nearest-neighbor filtering.
  NEAREST,

  /// Bilinear filtering.
  BILINEAR,

  /// Trilinear filtering.
  TRILINEAR,

#if !defined(VGL_DISABLE_ANISOTROPY) || !VGL_DISABLE_ANISOTROPY
  /// Trilinear filtering with anisotropy.
  ANISOTROPIC,
#endif
};

/// Wrap mode.
enum class WrapMode
{
  /// Wrap at edge.
  WRAP,

  /// Clamp to edge.
  CLAMP 
};

/// Texture class.
class Texture
{
private:
    /// Number of texture units managed by this class.
    static unsigned const MAX_TEXTURE_UNITS = 8;

private:
    /// Currently bound textures. One for each texture unit.
    static const Texture* g_current_texture[MAX_TEXTURE_UNITS];

    /// Currently active texture unit.
    static unsigned g_active_texture_unit;

private:
    /// OpenGL texture type.
    GLenum m_type;

    /// OpenGL texture name.
    GLuint m_id;

private:
    /// Deleted copy constructor.
    Texture(const Texture&) = delete;
    /// Deleted assignment.
    Texture& operator=(const Texture&) = delete;

public:
    /// Constructor.
    explicit Texture(GLenum type) :
        m_type(type)
    {
        dnload_glGenTextures(1, &m_id);
    }

    /// Destructor.
    ~Texture()
    {
#if defined(USE_LD)
        unbind();

        glDeleteTextures(1, &m_id);
#endif
    }

protected:
    /// Set filtering mode for currently bound texture.
    ///
    /// \param data Data passed Filtering mode.
    /// \param filtering Filtering mode.
    /// \return True if mipmaps in use, false if not.
    bool setFiltering(const void* data, FilteringMode filtering) const
    {
        // 'nearest' -filtering forced.
        if(FilteringMode::NEAREST == filtering)
        {
            dnload_glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            dnload_glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            return false;
        }

        // Image textures may use mipmaps.
        if(data)
        {
            // Magnification always linear.
            dnload_glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Bilinear.
            if(FilteringMode::BILINEAR == filtering)
            {
                dnload_glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                return false;
            }

            // Trilinear is on even if anisotropy is not.
            dnload_glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

#if !defined(VGL_DISABLE_ANISOTROPY) || !VGL_DISABLE_ANISOTROPY
            if (FilteringMode::ANISOTROPIC == filtering)
            {
                dnload_glTexParameterf(m_type, GL_TEXTURE_MAX_ANISOTROPY_EXT, detail::ANISOTROPY_LEVEL);
            }
#endif

            // Mipmaps on.
            dnload_glGenerateMipmap(m_type);
            return true;
        }

        // FBOs use bilinar.
        dnload_glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        dnload_glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        return false;
    }

    /// Set wrap mode for currently bound texture.
    ///
    /// \param wrap Wrap mode.
    void setWrapMode(WrapMode wrap) const
    {
        GLint mode = (wrap == WrapMode::WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE;

        dnload_glTexParameteri(m_type, GL_TEXTURE_WRAP_S, mode);
        dnload_glTexParameteri(m_type, GL_TEXTURE_WRAP_T, mode);

#if !defined(DNLOAD_GLESV2)
        /// Set 3rd axis as well if necessary.
        if((m_type == GL_TEXTURE_3D) || (m_type == GL_TEXTURE_CUBE_MAP))
        {
            dnload_glTexParameteri(m_type, GL_TEXTURE_WRAP_R, mode);
        }
#endif
    }

    /// Begin update operation.
    ///
    /// \return Texture to restore to active texture unit.
    const Texture* updateBegin()
    {
        const Texture* prev_texture = g_current_texture[g_active_texture_unit];

        bind(g_active_texture_unit);

        return prev_texture;
    }

    /// End update operation.
    ///
    /// \param texture Texture to restore to active texture unit.
    void updateEnd(const Texture* texture)
    {
        if(texture)
        {
            texture->bind(g_active_texture_unit);
        }
#if defined(USE_LD)
        else
        {
            unbind();
        }
#endif
    }

#if defined(USE_LD)
    /// Unbind texture from whichever texture unit it's bound to.
    void unbind() const
    {
        unsigned original_texture_unit = g_active_texture_unit;

        for(unsigned ii = 0; (ii < MAX_TEXTURE_UNITS); ++ii)
        {
            if(g_current_texture[ii] && (g_current_texture[ii]->m_id == m_id))
            {
                select_texture_unit(ii);
                glBindTexture(m_type, 0);
                g_current_texture[ii] = nullptr;
            }
        }

        select_texture_unit(original_texture_unit);
    }
#endif

public:
    /// Bind texture to given texture unit 
    ///
    /// \param op Unit to bind to.
    void bind(unsigned op) const
    {
#if defined(USE_LD)
        if(op >= MAX_TEXTURE_UNITS)
        {
            VGL_THROW_RUNTIME_ERROR("trying to bind to texture unit index " + to_string(op) + " of maximum " +
                    to_string(MAX_TEXTURE_UNITS));
        }
#endif

        if(g_current_texture[op] != this)
        {
            select_texture_unit(op);
            dnload_glBindTexture(m_type, m_id);
            g_current_texture[op] = this;
        }
    }

    /// Accessor.
    ///
    /// \return Texture id.
    unsigned getId() const
    {
        return m_id;
    }

    /// Accessor.
    ///
    /// \return Texture type.
    GLenum getType() const
    {
        return m_type;
    }

private:
    /// Activate a texture unit.
    ///
    /// \param op Texture unit to activate.
    static void select_texture_unit(unsigned op)
    {
        if(g_active_texture_unit != op)
        {
            dnload_glActiveTexture(GL_TEXTURE0 + op);
            g_active_texture_unit = op;
        }
    }
};

}

#if !defined(USE_LD)
#include "vgl_texture.cpp"
#endif

#endif
