#include "vgl_frame_buffer.hpp"

namespace vgl
{

const FrameBuffer* FrameBuffer::g_current_frame_buffer = &g_default_frame_buffer;
FrameBuffer FrameBuffer::g_default_frame_buffer;

}

