///*
// * Nuklear - 1.32.0 - public domain
// * no warranty implied; use at your own risk.
// * authored from 2015-2016 by Micha Mettke
// */
//#pragma once
//#include "pch.h"
//#include <GL/glew.h> 
//#include <GLFW/glfw3.h>
//
//#define NK_INCLUDE_FIXED_TYPES
//#define NK_INCLUDE_STANDARD_IO
//#define NK_INCLUDE_STANDARD_VARARGS
//#define NK_INCLUDE_DEFAULT_ALLOCATOR
//#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
//#define NK_INCLUDE_FONT_BAKING
//#define NK_INCLUDE_DEFAULT_FONT
//#define NK_KEYSTATE_BASED_INPUT
//#include "nuklear.h"
//
//
//enum nk_glfw_init_state {
//    NK_GLFW3_DEFAULT = 0,
//    NK_GLFW3_INSTALL_CALLBACKS
//};
//
//#ifndef NK_GLFW_TEXT_MAX
//#define NK_GLFW_TEXT_MAX 256
//#endif
//
//struct nk_glfw_device {
//    struct nk_buffer cmds;
//    struct nk_draw_null_texture tex_null;
//    GLuint vbo, vao, ebo;
//    GLuint prog;
//    GLuint vert_shdr;
//    GLuint frag_shdr;
//    GLint attrib_pos;
//    GLint attrib_uv;
//    GLint attrib_col;
//    GLint uniform_tex;
//    GLint uniform_proj;
//    GLuint font_tex;
//};
//
//struct nk_glfw {
//    GLFWwindow* win;
//    int width, height;
//    int display_width, display_height;
//    struct nk_glfw_device ogl;
//    struct nk_context ctx;
//    struct nk_font_atlas atlas;
//    struct nk_vec2 fb_scale;
//    unsigned int text[NK_GLFW_TEXT_MAX];
//    int text_len;
//    struct nk_vec2 scroll;
//    double last_button_click;
//    int is_double_click_down;
//    struct nk_vec2 double_click_pos;
//    float delta_time_seconds_last;
//};
//
//// Function declarations
//NK_API struct nk_context* nk_glfw3_init(struct nk_glfw* glfw, GLFWwindow* win, enum nk_glfw_init_state init_state);
//NK_API void nk_glfw3_shutdown(struct nk_glfw* glfw);
//NK_API void nk_glfw3_font_stash_begin(struct nk_glfw* glfw, struct nk_font_atlas** atlas);
//NK_API void nk_glfw3_font_stash_end(struct nk_glfw* glfw);
//NK_API void nk_glfw3_new_frame(struct nk_glfw* glfw);
//NK_API void nk_glfw3_render(struct nk_glfw* glfw, enum nk_anti_aliasing AA, int max_vertex_buffer, int max_element_buffer);
//NK_API void nk_glfw3_device_destroy(struct nk_glfw* glfw);
//NK_API void nk_glfw3_device_create(struct nk_glfw* glfw);
//NK_API void nk_glfw3_char_callback(GLFWwindow* win, unsigned int codepoint);
//NK_API void nk_gflw3_scroll_callback(GLFWwindow* win, double xoff, double yoff);
//NK_API void nk_glfw3_mouse_button_callback(GLFWwindow* win, int button, int action, int mods);
//
//
