#ifndef _GRAPHICH_
#define _GRAPHICH_

#include <math.h>
#include <unistd.h>
#include <time.h>
#include "vector.h"
#include "bmp.h"
#include "image_server.h"

#define EPSILON 1e-6
#define PI 3.14159265

void updateGraphicsFinal(bitmap_t bmp);
void updateGraphics(bitmap_t bmp);
void drawRobot(bitmap_t *bmp, vector_t poly_pts, color_bgr_t color);
void plotLine(vector_t *data_points, int x0, int y0, int x1, int y1);
void drawRectangle(vector_t *points, int w, int h);
void drawTriangle(vector_t *points, double length);
void translate(vector_t *points, double x_trans, double y_trans);
void rotate(vector_t *points, double angle);
void roundPoints(vector_t points, vector_t *rounded_points);
void bmp_draw_points(bitmap_t *bmp, vector_t points, color_bgr_t color);
void bmp_fill_points(bitmap_t *bmp, vector_t points, color_bgr_t color);

#endif
