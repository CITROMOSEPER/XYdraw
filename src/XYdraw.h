// XYdraw library header file
// ESP32
// Nemes Dániel


#ifndef XYdraw_h
#define XYdraw_h

#include <Arduino.h>


// ── Scenebuffer capacities ───────────────────────────────────────────────────
// Increase if your scene needs more objects; each entry is very small.
// FB_MAX_LINES: max 3D line segments in the scenebuffer (12 bytes each)
// FB_MAX_POLYGONS: max triangular polygon faces  (18 bytes each)
#define FB_MAX_LINES    64
#define FB_MAX_POLYGONS 32


class XYdraw
{
  public:
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Setup functions

    XYdraw(int ch_x_pin, int ch_y_pin, unsigned int refine);
    void begin();
    void set_refine(unsigned int refine);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 2D rendering functions

    void line(int start_x, int start_y, int end_x, int end_y);
    void rectangle(int x, int y, int x_length, int y_length);
    void circle(int origin_x, int origin_y, int radius);
    void ellipse(int origin_x, int origin_y, int radius_x, int radius_y);
    void bezier_curve(int anchor1_x, int anchor1_y, int anchor2_x, int anchor2_y, 
                      int cp1_x,     int cp1_y,     int cp2_x,     int cp2_y);
    void render_raster(bool raster[], int x_size, int y_size);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Regular 3D rendering functions

    void threeD_line(int start_x,   int start_y,   int start_z, 
                     int end_x,     int end_y,     int end_z, 
                     int cam_x,     int cam_y,     int cam_z, 
                     int cam_x_dir, int cam_y_dir, int cam_z_dir);
    void polygon(int p1_x,      int p1_y,      int p1_z,
                 int p2_x,      int p2_y,      int p2_z,
                 int p3_x,      int p3_y,      int p3_z,
                 int cam_x,     int cam_y,     int cam_z,
                 int cam_x_dir, int cam_y_dir, int cam_z_dir);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Scenebuffered (occlusion) 3D rendering functions

    // Discard all objects currently in the scenebuffer.
    void clear_scenebuffer();

    // Stage a 3-D line segment for the next render_scenebuffer() call.
    void add_threeD_line_to_scenebuffer(int start_x, int start_y, int start_z,
                                        int end_x,   int end_y,   int end_z);

    // Stage an opaque triangular polygon for the next render_scenebuffer() call.
    void add_polygon_to_scenebuffer(int p1_x, int p1_y, int p1_z,
                                    int p2_x, int p2_y, int p2_z,
                                    int p3_x, int p3_y, int p3_z);

    // Project, cull, clip lines against polygons, and draw everything.
    void render_scenebuffer(int cam_x,     int cam_y,     int cam_z,
                            int cam_x_dir, int cam_y_dir, int cam_z_dir);

    bool check_collision_with_camera(unsigned int radius);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // String rendering functions

    void render_left_justified_string(String input_string, int x, int y, int size, int letter_distance);
    void render_right_justified_string(String input_string, int x, int y, int size, int letter_distance);
    void render_centered_string(String input_string, int x, int y, int size, int letter_distance);
    void render_letter(char letter, int x, int y, int offset, int size);


  private:
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // State variables
    int _ch_x_pin;
    int _ch_y_pin;
    int _refine;
    int _cam_x;
    int _cam_y;
    int _cam_z;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // scenebuffer storage

    struct FBLine
    {
      int16_t sx, sy, sz;   // world-space start
      int16_t ex, ey, ez;   // world-space end
    };
    struct FBPoly
    {
      int16_t p1x, p1y, p1z;
      int16_t p2x, p2y, p2z;
      int16_t p3x, p3y, p3z;
    };
    FBLine  _fb_lines[FB_MAX_LINES];
    FBPoly  _fb_polys[FB_MAX_POLYGONS];
    uint8_t _fb_line_count;
    uint8_t _fb_poly_count;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Special character rendering functions

    void render_exclamationmark(int x, int y, int size);
    void render_doublequotes(int x, int y, int size);
    void render_numbersign(int x, int y, int size);
    void render_dollarsign(int x, int y, int size);
    void render_percentsign(int x, int y, int size);
    void render_ampersand(int x, int y, int size);
    void render_singlequote(int x, int y, int size);
    void render_openparenthesis(int x, int y, int size);
    void render_closeparenthesis(int x, int y, int size);
    void render_asterisk(int x, int y, int size);
    void render_plus(int x, int y, int size);
    void render_comma(int x, int y, int size);
    void render_hyphen(int x, int y, int size);
    void render_fullstop(int x, int y, int size);
    void render_slash(int x, int y, int size);
    void render_colon(int x, int y, int size);
    void render_semicolon(int x, int y, int size);
    void render_lessthansign(int x, int y, int size);
    void render_equalsign(int x, int y, int size);
    void render_greaterthansign(int x, int y, int size);
    void render_questionmark(int x, int y, int size);
    void render_atsign(int x, int y, int size);
    void render_openingbracket(int x, int y, int size);
    void render_backslash(int x, int y, int size);
    void render_closingbracket(int x, int y, int size);
    void render_caret(int x, int y, int size);
    void render_underscore(int x, int y, int size);
    void render_graveaccent(int x, int y, int size);
    void render_openingbrace(int x, int y, int size);
    void render_verticalbar(int x, int y, int size);
    void render_closingbrace(int x, int y, int size);
    void render_tilde(int x, int y, int size);
    void render_unknown_character(int x, int y, int size);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Number rendering functions

    void render_zero(int x, int y, int size);
    void render_one(int x, int y, int size);
    void render_two(int x, int y, int size);
    void render_three(int x, int y, int size);
    void render_four(int x, int y, int size);
    void render_five(int x, int y, int size);
    void render_six(int x, int y, int size);
    void render_seven(int x, int y, int size);
    void render_eight(int x, int y, int size);
    void render_nine(int x, int y, int size);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Non - capitalized letter rendering functions

    void render_a(int x, int y, int size);
    void render_b(int x, int y, int size);
    void render_c(int x, int y, int size);
    void render_d(int x, int y, int size);
    void render_e(int x, int y, int size);
    void render_f(int x, int y, int size);
    void render_g(int x, int y, int size);
    void render_h(int x, int y, int size);
    void render_i(int x, int y, int size);
    void render_j(int x, int y, int size);
    void render_k(int x, int y, int size);
    void render_l(int x, int y, int size);
    void render_m(int x, int y, int size);
    void render_n(int x, int y, int size);
    void render_o(int x, int y, int size);
    void render_p(int x, int y, int size);
    void render_q(int x, int y, int size);
    void render_r(int x, int y, int size);
    void render_s(int x, int y, int size);
    void render_t(int x, int y, int size);
    void render_u(int x, int y, int size);
    void render_v(int x, int y, int size);
    void render_w(int x, int y, int size);
    void render_x(int x, int y, int size);
    void render_y(int x, int y, int size);
    void render_z(int x, int y, int size);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Capitalized letter rendering functions

    void render_A(int x, int y, int size);
    void render_B(int x, int y, int size);
    void render_C(int x, int y, int size);
    void render_D(int x, int y, int size);
    void render_E(int x, int y, int size);
    void render_F(int x, int y, int size);
    void render_G(int x, int y, int size);
    void render_H(int x, int y, int size);
    void render_I(int x, int y, int size);
    void render_J(int x, int y, int size);
    void render_K(int x, int y, int size);
    void render_L(int x, int y, int size);
    void render_M(int x, int y, int size);
    void render_N(int x, int y, int size);
    void render_O(int x, int y, int size);
    void render_P(int x, int y, int size);
    void render_Q(int x, int y, int size);
    void render_R(int x, int y, int size);
    void render_S(int x, int y, int size);
    void render_T(int x, int y, int size);
    void render_U(int x, int y, int size);
    void render_V(int x, int y, int size);
    void render_W(int x, int y, int size);
    void render_X(int x, int y, int size);
    void render_Y(int x, int y, int size);
    void render_Z(int x, int y, int size);
};

#endif