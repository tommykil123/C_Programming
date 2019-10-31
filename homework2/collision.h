#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "vector.h"

#define PI 3.14159265

typedef struct object {
    int num_pts;
    float pt_array[16][2];
} object_t;

typedef struct point {
    float x;
    float y;
} point_t;

typedef struct line {
    point_t st_pt;
    point_t end_pt;
} line_t;

void print_poly(object_t *polygons);
float cross_product(line_t line, point_t pt);
int onSegment(line_t line, point_t pt);
int chekcPolyIntersection(object_t poly0, object_t poly1);
int chekcLineIntersection(line_t line0, line_t line1);
int checkPolyContain(object_t poly0, object_t poly1);
int collision(vector_t poly0, vector_t poly1);
