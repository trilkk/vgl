#ifndef VGL_FONT_HPP
#define VGL_FONT_HPP

#include "vgl_character.hpp"

#if defined(USE_LD)
#include "vgl_filesystem.hpp"
#endif

namespace vgl
{

/// Font.
class Font
{
private:
    /// FreeType library reference.
    static FT_Library g_freetype_library;

#if defined(USE_LD)
    /// Number of fonts initialized.
    static unsigned g_freetype_count;
#endif

private:
    /// Font structure.
    FT_Face m_face = nullptr;

    /// Characters loaded.
    vector<Character> m_characters;

    /// 'M' size for the face (pixels).
    unsigned m_font_size;

private:
    /// Deleted copy constructor.
    Font(const Font&) = delete;
    /// Deleted assignment.
    Font& operator=(const Font&) = delete;

public:
    /// Constructor.
    ///
    /// \param fs Font size.
    /// \param fnames Filename array.
    explicit Font(unsigned fs, const char **fnames) :
        m_font_size(fs)
    {
        freetype_increment();

        for(const char **iter = fnames; true; ++iter)
        {
#if defined(USE_LD)
            if(!(*iter))
            {
                std::ostringstream sstr;
                sstr << "no suitable font found, tried:";
                for(const char **ii = fnames; *ii; ++ii)
                {
                    sstr << ((ii != fnames) ? " '" : ", '") << (*ii) << "'";
                }
                VGL_THROW_RUNTIME_ERROR(sstr.str().c_str());
            }
#endif
            if(load(*iter))
            {
                break;
            }
        }

        FT_Error err = dnload_FT_Set_Pixel_Sizes(m_face, 0, m_font_size);
#if defined(USE_LD)
        if(err)
        {
            VGL_THROW_RUNTIME_ERROR("could not set font size to " + to_string(m_font_size) + " on " +
                    to_string(m_face));
        }
#else
        (void)err;
#endif
    }

    /// Destructor.
    ~Font()
    {
#if defined(USE_LD)
        if(m_face)
        {
            FT_Done_Face(m_face);
        }

        // Decrement FreeType usage count.
        freetype_decrement();
#endif
    }

private:
    /// Attempt to load a font.
    ///
    /// \param fname Filename.
    /// \return True on success, false on failure.
    bool load(const char* fname)
    {
        FT_Error err;
#if defined(USE_LD)
        path pth = find_file(fname);
        if(pth.empty())
        {
            return false;
        }
        err = dnload_FT_New_Face(g_freetype_library, pth.getString().c_str(), 0, &m_face);
#else
        err = dnload_FT_New_Face(g_freetype_library, fname, 0, &m_face);
#endif
        return (0 == err);
    }

public:
    /// Create one character (for later use).
    ///
    /// \param unicode Unicode character id.
    void createCharacter(unsigned unicode)
    {
        unsigned idx = dnload_FT_Get_Char_Index(m_face, unicode);
#if defined(USE_LD)
        if(0 >= idx)
        {
            VGL_THROW_RUNTIME_ERROR("no character " + to_string(unicode)+ " found in font");
        }
#endif

        FT_Error err = dnload_FT_Load_Glyph(m_face, idx, FT_LOAD_DEFAULT);
#if defined(USE_LD)
        if(err)
        {
            VGL_THROW_RUNTIME_ERROR("error loading character " + to_string(unicode) + " at index " + to_string(idx) +
                    ": " + to_string(err));
        }
#else
        (void)err;
#endif

        FT_GlyphSlot glyph = m_face->glyph;
        if(glyph->format != FT_GLYPH_FORMAT_BITMAP)
        {
            err = dnload_FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL);
#if defined(USE_LD)
            if(err)
            {
                VGL_THROW_RUNTIME_ERROR("error rendering glyph " + to_string(unicode) + " at index " +
                        to_string(idx) + ": " + to_string(err));
            }
#endif
        }

        float fsize = static_cast<float>(m_font_size);

        // No Y advance - only left-right text supported.
        if (m_characters.size() <= unicode)
        {
            m_characters.resize(unicode + 1);
        }
        m_characters[unicode] = Character(idx,
                &(glyph->bitmap),
                static_cast<float>(glyph->bitmap_left) / fsize,
                static_cast<float>(glyph->bitmap_top) / fsize,
                (static_cast<float>(glyph->advance.x) / fsize) * (1.0f / 64.0f),
                fsize);
    }

    /// Accessor.
    ///
    /// \param unicode Character by unicode.
    /// \return Reference to the character.
    const Character& getCharacter(unsigned unicode) const
    {
#if defined(USE_LD)
        if(m_characters.size() <= unicode)
        {
            VGL_THROW_RUNTIME_ERROR("unicode index outside of range: " + to_string(unicode));
        }
        if(!m_characters[unicode])
        {
            VGL_THROW_RUNTIME_ERROR("character " + to_string(unicode) + " has not been created");
        }
#endif
        return m_characters[unicode];
    }

#if defined(FONT_ENABLE_KERNING)
    /// Get kerning between 2 character indices.
    ///
    /// \param prev Previous index.
    /// \param next Next index.
    /// \return Kerning value.
    float getKerning(unsigned prev, unsigned next) const
    {
        FT_Vector delta;

        FT_Error err = dnload_FT_Get_Kerning(m_face, prev, next, FT_KERNING_DEFAULT, &delta);
#if defined(USE_LD)
        if(err)
        {
            VGL_THROW_RUNTIME_ERROR("could not get kerning information for " + to_string(prev) + " and " +
                    to_string(next));
        }
#else
        (void)err;
#endif

        return static_cast<float>(delta.x) * (1.0f / 4096.0f); // Where does the scale come from?
    }
#endif

private:
    /// Increment FreeType usage count.
    ///
    /// If this is the first call, initialize FreeType.
    static void freetype_increment()
    {
#if defined(USE_LD)
        VGL_ASSERT(static_cast<bool>(g_freetype_count) == static_cast<bool>(g_freetype_library));
        ++g_freetype_count;
#endif
        if(g_freetype_library)
        {
            return;
        }

        FT_Error err = dnload_FT_Init_FreeType(&g_freetype_library);
#if defined(USE_LD)
        if(0 != err)
        {
            VGL_THROW_RUNTIME_ERROR("could not init FreeType: " + to_string(err));
        }
#else
        (void)err;
#endif
    }

public:
    /// Create a font.
    ///
    /// \param fs Font size.
    /// \param fnames Filename array.
    /// \return Font created.
    static unique_ptr<Font> create(unsigned fs, const char **fnames)
    {
        return unique_ptr<Font>(new Font(fs, fnames));
    }

#if defined(USE_LD)
    /// Decrement FreeType usage count.
    ///
    /// Deinitialize FreeType if the usage count reaches zero.
    static void freetype_decrement()
    {
        VGL_ASSERT(static_cast<bool>(g_freetype_count) == static_cast<bool>(g_freetype_library));
        if(!g_freetype_count)
        {
            return;
        }
        if(--g_freetype_count)
        {
            return;
        }

        FT_Error err = FT_Done_FreeType(g_freetype_library);
        if(0 != err)
        {
            VGL_THROW_RUNTIME_ERROR("could not close FreeType: " + to_string(err));
        }
        g_freetype_library = nullptr;
    }
#endif
};

/// Font unique pointer type.
using FontUptr = unique_ptr<Font>;

}

#if !defined(USE_LD)
#include "vgl_font.cpp"
#endif

#endif
