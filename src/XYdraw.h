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
#define FB_MAX_LINES    255
#define FB_MAX_POLYGONS 255

// ── Output defaults ──────────────────────────────────────────────────────────
// Default point rate in points (X+Y pairs) per second.  The ESP32 DAC itself
// can't usefully settle much faster than ~1 MHz into a scope cable; if corners
// look rounded or lines look shortened, lower it with set_sample_rate().
#define XY_DEFAULT_SAMPLE_RATE  1000000UL
// Default buffered-mode capacity (points per frame).  2 bytes per point, x2 buffers.
#define XY_DEFAULT_MAX_POINTS   16384UL


class XYdraw
{
  public:
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Setup functions

    XYdraw(int ch_x_pin, int ch_y_pin, unsigned int refine);

    // Immediate (direct) mode — same behaviour as the original library: every
    // drawing call writes to the DAC right away.  Uses direct register writes
    // instead of dacWrite(), so each point is several times cheaper.
    void begin();

    // Buffered mode — drawing calls append points to a back buffer; present()
    // hands the finished frame to a background task on the other CPU core,
    // which replays it to the DAC continuously at a fixed point rate.
    // The scope image therefore never flickers while the sketch is busy
    // computing the next frame, and brightness is even across the frame.
    // Call present() once at the end of every frame.
    // Returns false if the buffers could not be allocated (falls back to direct mode).
    bool begin_buffered(uint32_t max_points  = XY_DEFAULT_MAX_POINTS,
                        uint32_t sample_rate = XY_DEFAULT_SAMPLE_RATE);
    void end_buffered();     // stop the output task, free buffers, return to direct mode
    void present();          // buffered mode: publish the current frame (no-op in direct mode)

    void set_refine(unsigned int refine);

    // Point rate in points per second.  Direct mode: upper limit (0 = as fast
    // as the CPU can go).  Buffered mode: exact playback rate.
    void set_sample_rate(uint32_t points_per_second);

    // Line sampling.  0 (default) = original behaviour: every line gets
    // refine + 1 points regardless of length.  > 0 = adaptive: a line gets one
    // point every `spacing` DAC units (min. 2 points), so short lines (font
    // strokes, occlusion sub-segments, raster pixels) cost far fewer points and
    // all lines have the same brightness.  Typical values: 2.0 – 6.0.
    void set_point_spacing(float spacing);

    // Buffered-mode statistics for the most recently presented frame.
    uint32_t frame_points() const   { return _last_frame_points; }
    bool     frame_overflowed() const { return _last_frame_overflow; }  // true = points were dropped (raise max_points)
    float    frame_rate() const;    // refresh rate the scope is actually seeing (Hz)

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 2D rendering functions

    void line(int start_x, int start_y, int end_x, int end_y);
    void rectangle(int x, int y, int x_length, int y_length);
    void circle(int origin_x, int origin_y, int radius);
    void ellipse(int origin_x, int origin_y, int radius_x, int radius_y);
    void bezier_curve(int anchor1_x, int anchor1_y, int anchor2_x, int anchor2_y, 
                      int cp1_x,     int cp1_y,     int cp2_x,     int cp2_y);
    void render_raster(bool raster[], unsigned int x_size, unsigned int y_size);

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

    void render_voxels(bool voxels[], unsigned int x_size, unsigned int y_size, unsigned int z_size,
                       unsigned int voxel_size,
                       int x_pos,   int y_pos,   int z_pos,
                       int cam_x,   int cam_y,   int cam_z,
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

    // Stage a 3D tensor of voxels for the next render_scenebuffer() call.
    void add_voxels_to_scenebuffer(bool voxels[], unsigned int x_size, unsigned int y_size, unsigned int z_size,
                                   unsigned int voxel_size,
                                   int x_pos, int y_pos, int z_pos);

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
    // Fast DAC output state

    // Emits one point (clamped to 0..255) to the DAC or to the back buffer.
    inline void emit(int x, int y) __attribute__((always_inline));
    inline void dac_out(uint8_t x, uint8_t y) __attribute__((always_inline));
    void play_points(const uint16_t* pts, uint32_t n);   // paced playback (direct / unicore)
    void wait_for_swap();
    void update_cycles_per_point();
    void cam_trig(int cam_x_dir, int cam_y_dir, int cam_z_dir);
    static void output_task(void* arg);

    bool              _fast_dac;        // true = direct register writes available
    volatile uint32_t* _x_reg;
    volatile uint32_t* _y_reg;
    uint32_t          _x_base;          // register contents with the value field cleared
    uint32_t          _y_base;

    uint32_t          _sample_rate;
    uint32_t          _cycles_per_point; // 0 = unpaced
    uint32_t          _next_cycle;       // direct-mode pacing deadline
    float             _point_spacing;

    // Buffered mode
    bool              _buffered;
    uint16_t*         _buf[2];
    uint32_t          _cap;
    uint16_t*         _back;
    uint32_t          _back_n;
    bool              _overflow;
    bool              _need_wait;
    uint16_t* volatile _pending_buf;
    volatile uint32_t  _pending_n;
    volatile bool      _swap_req;
    volatile uint32_t  _shown_n;         // points in the frame the task is currently replaying
    void*             _task;             // TaskHandle_t
    uint32_t          _last_frame_points;
    bool              _last_frame_overflow;

    // Cached camera trig (recomputed only when the angles change)
    bool  _trig_valid;
    int   _trig_ax, _trig_ay, _trig_az;
    float _t_cp, _t_sp, _t_cy, _t_sy, _t_cr, _t_sr;

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