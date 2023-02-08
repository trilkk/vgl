if(NOT VGL_ROOT)
    set(VGL_ROOT ".")
endif()

set(VGL_HEADERS
    "${VGL_ROOT}/vgl_algorithm.hpp"
    "${VGL_ROOT}/vgl_animation.hpp"
    "${VGL_ROOT}/vgl_animation_frame.hpp"
    "${VGL_ROOT}/vgl_animation_state.hpp"
    "${VGL_ROOT}/vgl_armature.hpp"
    "${VGL_ROOT}/vgl_array.hpp"
    "${VGL_ROOT}/vgl_assert.hpp"
    "${VGL_ROOT}/vgl_bitset.hpp"
    "${VGL_ROOT}/vgl_bone.hpp"
    "${VGL_ROOT}/vgl_bone_state.hpp"
    "${VGL_ROOT}/vgl_bounding_box.hpp"
    "${VGL_ROOT}/vgl_buffer.hpp"
    "${VGL_ROOT}/vgl_character.hpp"
    "${VGL_ROOT}/vgl_cond.hpp"
    "${VGL_ROOT}/vgl_csg.hpp"
    "${VGL_ROOT}/vgl_csg_file.hpp"
    "${VGL_ROOT}/vgl_cstddef.hpp"
    "${VGL_ROOT}/vgl_extern_math.hpp"
    "${VGL_ROOT}/vgl_extern_opengl.hpp"
    "${VGL_ROOT}/vgl_extern_sdl.hpp"
    "${VGL_ROOT}/vgl_fence.hpp"
    "${VGL_ROOT}/vgl_font.hpp"
    "${VGL_ROOT}/vgl_frame_buffer.hpp"
    "${VGL_ROOT}/vgl_geometry_buffer.hpp"
    "${VGL_ROOT}/vgl_geometry_channel.hpp"
    "${VGL_ROOT}/vgl_geometry_handle.hpp"
    "${VGL_ROOT}/vgl_glsl_program.hpp"
    "${VGL_ROOT}/vgl_glsl_shader.hpp"
    "${VGL_ROOT}/vgl_image.hpp"
    "${VGL_ROOT}/vgl_image_2d.hpp"
    "${VGL_ROOT}/vgl_image_2d_gray.hpp"
    "${VGL_ROOT}/vgl_image_2d_la.hpp"
    "${VGL_ROOT}/vgl_image_2d_rgb.hpp"
    "${VGL_ROOT}/vgl_image_2d_rgba.hpp"
    "${VGL_ROOT}/vgl_index_block.hpp"
    "${VGL_ROOT}/vgl_ivec3.hpp"
    "${VGL_ROOT}/vgl_limits.hpp"
    "${VGL_ROOT}/vgl_logical_face.hpp"
    "${VGL_ROOT}/vgl_logical_mesh.hpp"
    "${VGL_ROOT}/vgl_logical_vertex.hpp"
    "${VGL_ROOT}/vgl_mat.hpp"
    "${VGL_ROOT}/vgl_mat2.hpp"
    "${VGL_ROOT}/vgl_mat3.hpp"
    "${VGL_ROOT}/vgl_mat4.hpp"
    "${VGL_ROOT}/vgl_math.hpp"
    "${VGL_ROOT}/vgl_mesh.hpp"
    "${VGL_ROOT}/vgl_mesh_data.hpp"
    "${VGL_ROOT}/vgl_mutex.hpp"
    "${VGL_ROOT}/vgl_optional.hpp"
    "${VGL_ROOT}/vgl_opus.hpp"
    "${VGL_ROOT}/vgl_packed_data.hpp"
    "${VGL_ROOT}/vgl_packed_data_reader.hpp"
    "${VGL_ROOT}/vgl_path.hpp"
    "${VGL_ROOT}/vgl_quat.hpp"
    "${VGL_ROOT}/vgl_queue.hpp"
    "${VGL_ROOT}/vgl_realloc.hpp"
    "${VGL_ROOT}/vgl_render_queue.hpp"
    "${VGL_ROOT}/vgl_scoped_lock.hpp"
    "${VGL_ROOT}/vgl_state.hpp"
    "${VGL_ROOT}/vgl_string.hpp"
    "${VGL_ROOT}/vgl_string_view.hpp"
    "${VGL_ROOT}/vgl_task.hpp"
    "${VGL_ROOT}/vgl_task_dispatcher.hpp"
    "${VGL_ROOT}/vgl_task_queue.hpp"
    "${VGL_ROOT}/vgl_texture.hpp"
    "${VGL_ROOT}/vgl_texture_2d.hpp"
    "${VGL_ROOT}/vgl_texture_3d.hpp"
    "${VGL_ROOT}/vgl_texture_cube.hpp"
    "${VGL_ROOT}/vgl_texture_format.hpp"
    "${VGL_ROOT}/vgl_thread.hpp"
    "${VGL_ROOT}/vgl_type_traits.hpp"
    "${VGL_ROOT}/vgl_unique_ptr.hpp"
    "${VGL_ROOT}/vgl_uvec4.hpp"
    "${VGL_ROOT}/vgl_vec.hpp"
    "${VGL_ROOT}/vgl_vec2.hpp"
    "${VGL_ROOT}/vgl_vec3.hpp"
    "${VGL_ROOT}/vgl_vec4.hpp"
    "${VGL_ROOT}/vgl_vector.hpp"
    "${VGL_ROOT}/vgl_wave.hpp")
set(VGL_SOURCES
    "${VGL_ROOT}/vgl_csg_file.cpp"
    "${VGL_ROOT}/vgl_path.cpp"
    "${VGL_ROOT}/vgl_wave.cpp")
