#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define PI 3.14159265

typedef struct object {
    double loc[3];
    int num_pts;
    double double_array[16][2];
} object_t;

typedef struct point {
    double x;
    double y;
} point_t;

typedef struct line {
    point_t st_pt;
    point_t end_pt;
} line_t;

void print_poly(object_t *polygons) {
    // Print out the (x,y) coordinates
    for (int i = 0; i < 2; i++) {
        printf("For Polygon %d\n the coordinates are: ", i);
        for (int j = 0; j < polygons[i].num_pts; j++) {
            printf("(%lf, %lf) ", polygons[i].double_array[j][0], polygons[i].double_array[j][1]);
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

int checkIntersection(line_t line0, line_t line1) {
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

int main(void) {
    FILE *f = fopen("polygons.csv", "r");
    if (!f) {
        fprintf(stderr, "polygons.csv is missing\n");
        exit(1);
    }
    // Skip the first line in the csv file
    char buffer[100];
    fgets(buffer, 100, f);
    // Initialize array of object_t
    object_t polygons[2];

    while (1) {
        /*if (feof(f)) {
            break;
        }*/
        for (int i = 0; i < 2; i++) {
            object_t obj = {0};
            // Read objects x, y, and rotation
            int args_rd = fscanf(f, "%lf%lf%lf", &obj.loc[0], &obj.loc[1], &obj.loc[2]);
            if (args_rd != 3) {
                fprintf(stderr, "could not read all three values!\n");
                fclose(f);
                exit(1);
            }
            // Read the number of points
            args_rd = fscanf(f, "%d", &obj.num_pts);
            if (args_rd != 1) {
                fprintf(stderr, "could not read number of points!\n");
                fclose(f);
                exit(1);
            }
            if (obj.num_pts < 0) {
                fprintf(stderr, "negative number of points does not make sense!\n");
                fclose(f);
                exit(1);
            }
            if (obj.num_pts > 16) {
                fprintf(stderr, "Error, too many points!\n");
                fclose(f);
                exit(1);
            }
            for (int j = 0; j < obj.num_pts; j++) {
                args_rd = fscanf(f, "%lf", &obj.double_array[j][0]);
                if (args_rd != 1) {
                    fprintf(stderr, "a polygons.csv number is invalid\n");
                    fclose(f);
                    exit(1);
                }
            }
            for (int j = 0; j < obj.num_pts; j++) {
                args_rd = fscanf(f, "%lf", &obj.double_array[j][1]);
                if (args_rd != 1) {
                    fprintf(stderr, "a polygons.csv number is invalid\n");
                    fclose(f);
                    exit(1);
                }
            }
            polygons[i] = obj;
        }
        break;
    }

    // print_poly(polygons);
    // Apply rotation
    for (int i = 0; i < 2; i++) {
        int polygon_size = polygons[i].num_pts;
        for (int j = 0; j < polygon_size; j++) {
            double x_orig = polygons[i].double_array[j][0];
            double y_orig = polygons[i].double_array[j][1];
            double theta = polygons[i].loc[2] * PI / 180;
            double x_rot = x_orig * cos(theta) - y_orig * sin(theta);
            double y_rot = x_orig * sin(theta) + y_orig * cos(theta);
            polygons[i].double_array[j][0] = x_rot;
            polygons[i].double_array[j][1] = y_rot;
        }
    }

    //print_poly(polygons);
    // Apply translation
    for (int i = 0; i < 2; i++) {
        int polygon_size = polygons[i].num_pts;
        for (int j = 0; j < polygon_size; j++) {
            double x_rot = polygons[i].double_array[j][0];
            double y_rot = polygons[i].double_array[j][1];
            double x_trans = x_rot + polygons[i].loc[0];
            double y_trans = y_rot + polygons[i].loc[1];

            polygons[i].double_array[j][0] = x_trans;
            polygons[i].double_array[j][1] = y_trans;
        }
    }

    //print_poly(polygons);
    // Find the cross product for the two polygons
    int polygon0_size = polygons[0].num_pts;
    int polygon1_size = polygons[1].num_pts;
    for (int i = 0; i < polygon0_size; i++) {
        for (int j = 0; j < polygon1_size; j++) {
            line_t line0;
            line_t line1;

            line0.st_pt.x = polygons[0].double_array[i][0];
            line0.st_pt.y = polygons[0].double_array[i][1];
            line1.st_pt.x = polygons[1].double_array[j][0];
            line1.st_pt.y = polygons[1].double_array[j][1];

            if (i == polygons[0].num_pts - 1) {
                line0.end_pt.x = polygons[0].double_array[0][0];
                line0.end_pt.y = polygons[0].double_array[0][1];
            } else {
                line0.end_pt.x = polygons[0].double_array[i + 1][0];
                line0.end_pt.y = polygons[0].double_array[i + 1][1];
            }
            if (j == polygons[1].num_pts - 1) {
                line1.end_pt.x = polygons[1].double_array[0][0];
                line1.end_pt.y = polygons[1].double_array[0][1];
            } else {
                line1.end_pt.x = polygons[1].double_array[j + 1][0];
                line1.end_pt.y = polygons[1].double_array[j + 1][1];
            }

            int intersect = checkIntersection(line0, line1);
            if (intersect) {
                printf("collision!\n");
                fclose(f);
                return 0;
            }
        }
    }

    // Check to make sure that the polygon is not contained in the other
    for (int i = 0; i < 2; i++) {
        point_t pt;
        pt.x = polygons[(i + 1) % 2].double_array[0][0];
        pt.y = polygons[(i + 1) % 2].double_array[0][1];
        int contained = 1;
        int first_iter = 1;
        int is_positive = 0;
        for (int j = 0; j < polygons[i].num_pts; j++) {
            line_t line;
            line.st_pt.x = polygons[i].double_array[j][0];
            line.st_pt.y = polygons[i].double_array[j][1];
            if (j == polygons[i].num_pts - 1) {
                line.end_pt.x = polygons[i].double_array[0][0];
                line.end_pt.y = polygons[i].double_array[0][1];
            } else {
                line.end_pt.x = polygons[i].double_array[j + 1][0];
                line.end_pt.y = polygons[i].double_array[j + 1][1];
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
            printf("collision!\n");
            fclose(f);
            return 0;
        }
    }
    printf("no collision\n");
    fclose(f);
    return 0;
}
