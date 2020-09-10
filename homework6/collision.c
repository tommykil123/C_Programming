#include "collision.h"
#include "graphics.h"

void print_poly(object_t *polygons) {
    // Print out the (x,y) coordinates
    for (int i = 0; i < 2; i++) {
        printf("For Polygon %d\n the coordinates are: ", i);
        for (int j = 0; j < polygons[i].num_pts; j++) {
            printf("(%lf, %lf) ", polygons[i].pt_array[j][0], polygons[i].pt_array[j][1]);
        }
        printf("\n");
    }
}

double cross_product(line_t line, point_t pt) {
    double diff_line_x = line.end_pt.x - line.st_pt.x;
    double diff_line_y = line.end_pt.y - line.st_pt.y;
    return (diff_line_x * (pt.y - line.end_pt.y) - diff_line_y * (pt.x - line.end_pt.x));
}

int onSegment(line_t line, point_t pt) {
    point_t p = line.st_pt;
    point_t r = line.end_pt;
    point_t q = pt;
    if (q.x <= fmax(p.x, r.x) && q.x >= fmin(p.x, r.x) &&
        q.y <= fmax(p.y, r.y) && q.y >= fmin(p.y, r.y)) {
        return 1;
    }
    return 0;
}

int chekcPolyIntersection(object_t poly0, object_t poly1) {
    int polygon0_size = poly0.num_pts;
    int polygon1_size = poly1.num_pts;
    for (int i = 0; i < polygon0_size; i++) {
        for (int j = 0; j < polygon1_size; j++) {
            line_t line0;
            line_t line1;

            line0.st_pt.x = poly0.pt_array[i][0];
            line0.st_pt.y = poly0.pt_array[i][1];
            line1.st_pt.x = poly1.pt_array[j][0];
            line1.st_pt.y = poly1.pt_array[j][1];

            if (i == poly0.num_pts - 1) {
                line0.end_pt.x = poly0.pt_array[0][0];
                line0.end_pt.y = poly0.pt_array[0][1];
            } else {
                line0.end_pt.x = poly0.pt_array[i + 1][0];
                line0.end_pt.y = poly0.pt_array[i + 1][1];
            }
            if (j == poly1.num_pts - 1) {
                line1.end_pt.x = poly1.pt_array[0][0];
                line1.end_pt.y = poly1.pt_array[0][1];
            } else {
                line1.end_pt.x = poly1.pt_array[j + 1][0];
                line1.end_pt.y = poly1.pt_array[j + 1][1];
            }

            int intersect = chekcLineIntersection(line0, line1);
            if (intersect) {
                return 1;
            }
        }
    }
    return 0;
}

int chekcLineIntersection(line_t line0, line_t line1) {
    double orientation1 = cross_product(line0, line1.st_pt);
    double orientation2 = cross_product(line0, line1.end_pt);
    double orientation3 = cross_product(line1, line0.st_pt);
    double orientation4 = cross_product(line1, line0.end_pt);
    if (orientation1 * orientation2 < 0 &&
        orientation3 * orientation4 < 0) {
        return 1; // Intersects!!
    }
    // Check is collinear and if point lies between line
    if (orientation1 == 0 && (onSegment(line0, line1.st_pt))) {
        return 1;
    }
    if (orientation2 == 0 && (onSegment(line0, line1.end_pt))) {
        return 1;
    }
    if (orientation3 == 0 && (onSegment(line1, line0.st_pt))) {
        return 1;
    }
    if (orientation4 == 0 && (onSegment(line1, line0.end_pt))) {
        return 1;
    }
    return 0;
}

int checkPolyContain(object_t poly0, object_t poly1) {
    object_t polygons[2];
    polygons[0] = poly0;
    polygons[1] = poly1;
    for (int i = 0; i < 2; i++) {
        point_t pt;
        pt.x = polygons[(i + 1) % 2].pt_array[0][0];
        pt.y = polygons[(i + 1) % 2].pt_array[0][1];
        int contained = 1;
        int first_iter = 1;
        int is_positive = 0;
        for (int j = 0; j < polygons[i].num_pts; j++) {
            line_t line;
            line.st_pt.x = polygons[i].pt_array[j][0];
            line.st_pt.y = polygons[i].pt_array[j][1];
            if (j == polygons[i].num_pts - 1) {
                line.end_pt.x = polygons[i].pt_array[0][0];
                line.end_pt.y = polygons[i].pt_array[0][1];
            } else {
                line.end_pt.x = polygons[i].pt_array[j + 1][0];
                line.end_pt.y = polygons[i].pt_array[j + 1][1];
            }
            if (first_iter) {
                if (cross_product(line, pt) > 0) {
                    is_positive = 1;
                }
                first_iter = 0;
            } else {
                if (is_positive && cross_product(line, pt) < 0) {
                    contained = 0;
                }
                if (!is_positive && cross_product(line, pt) > 0) {
                    contained = 0;
                }
            }
        }
        if (contained) {
            return 1;
        }
    }
    return 0;
}

int collision(vector_t poly0, vector_t poly1) {
    // Initialize array of object_t
    object_t polygons[2];
    object_t obj0 = {0};
    object_t obj1 = {0};

    int poly0_size = poly0.size;
    obj0.num_pts = poly0_size;
    for (int i = 0; i < poly0_size; i++) {
        obj0.pt_array[i][0] = poly0.buffer_x[i];
        obj0.pt_array[i][1] = poly0.buffer_y[i];
    }
    polygons[0] = obj0;

    int poly1_size = poly1.size;
    obj1.num_pts = poly1_size;
    for (int i = 0; i < poly1_size; i++) {
        obj1.pt_array[i][0] = poly1.buffer_x[i];
        obj1.pt_array[i][1] = poly1.buffer_y[i];
    }
    polygons[1] = obj1;

    int intersects = chekcPolyIntersection(polygons[0], polygons[1]);
    if (intersects) {
        return 1;
    }

    int contains = checkPolyContain(polygons[0], polygons[1]);
    if (contains) {
        return 1;
    }
    return 0;
}
