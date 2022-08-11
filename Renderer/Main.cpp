// Main.cpp

#include "tgaimage.h"
#include "tgaimage.cpp"
#include "model.h"
#include "model.cpp"
#include "geometry.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <iomanip>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0  , 0  , 255);
const TGAColor green = TGAColor(0,   255, 0  , 255);

void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color);
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);

int main(int argc, char** argv)
{
    Model model("./african_head.obj");
    TGAImage image(200, 200, TGAImage::RGB);

    Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)};
    Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)};
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};
    triangle(t0[0], t0[1], t0[2], image, red);
    triangle(t1[0], t1[1], t1[2], image, white); 
    triangle(t2[0], t2[1], t2[2], image, green);
    
    /*
    for (int i = 0; i < model.nfaces(); i++)
    {
        std::vector<int> face = model.face(i);
        for (int j = 0; j < 3; j++)
        {
            Vec3 v0 = model.vert(face[j]);
            Vec3 v1 = model.vert(face[(j + 1) % 3]);
            int p0.x = (v0.x + 1.) * image.get_width() / 2.;
            int p0.y = (v0.y + 1.) * image.get_height() / 2.;
            int p1.x = (v1.x + 1.) * image.get_width() / 2.;
            int p1.y = (v1.y + 1.) * image.get_height() / 2.;
            line(p0.x, p0.y, p1.x, p1.y, image, white);
        }
    }
    */
    image.flip_vertically(); // Convention is usually that top left is (0, 0) so flipping vertically sets bottom left to being origin
    image.write_tga_file("output.tga");

    return 0;
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
{
    // Sort y-values of each vertex in ascending order in order to create segments that will allow us to fill lines later
    if (t0.y > t1.y) { std::swap(t0, t1); }
    if (t0.y > t2.y) { std::swap(t0, t2); }
    if (t1.y > t2.y) { std::swap(t1, t2); }

    int total_height = t2.y - t0.y;
    for (int i = 0; i < total_height; i++) // For loop to fill in bottom segment of the triangles
    {
        bool second_half = i > t1.y - t0.y || t1.y == t0.y;
        int segment_height = second_half ? t2.y - t1.y + 1: t1.y - t0.y + 1;
        float alpha = (float) i/total_height;
        float beta  = (float) (i - (second_half ? t1.y - t0.y: 0)) /segment_height;
        Vec2i A =               t0 + (t2 - t0) * alpha;
        Vec2i B = second_half ? t1 + (t2 - t1) * beta: t0 + (t1 - t0) * beta;
        if (A.x > B.x) { std::swap(A, B); }
        for (int j = A.x; j < B.x; j++)
        {
            image.set(j, t0.y + i, color);
        }
    }

    /* alpha is the same for top segment as well so for loops can be combined
    segment_height = t2.y - t1.y + 1;
    for (int y = t1.y; y <= t2.y; y++) // For loop to fill in top segment of the triangles
    {
        float alpha = (float)(y - t0.y)/total_height;
        float beta  = (float) (y - t1.y)/segment_height;
        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = t1 + (t2 - t1) * beta;
        if (A.x > B.x) { std::swap(A, B); }
        for (int j = A.x; j < B.x; j++)
        {
            image.set(j, y, color);
        }
    }
    */

    /* Demo to create outlines of triangles and show boundaries
    line(t0, t1, image, green); // Drawing a line from the coordinate with the smallest y coord to the middle y coord. 
                                // This forms the bottom segment/half of the triangle which allows lines to be drawn horizontally between the final line and this line.
    line(t1, t2, image, green); // Upper segment of triangle
    line(t2, t0, image, red); // Final boundary of triangle which to start the line filling from
    */
}

void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color)
{

    bool steep = false;

    if (std::abs(p0.y - p1.y) > std::abs(p0.x - p1.x)) // If the line is steep we transpose the image so instead of making sure that every column has one dot, we make sure every row has one dot
    {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        steep = true;
    }

    if (p0.x > p1.x) // Make sure that a line can also be drawn in reverse
    {
        std::swap(p0.x, p1.x);
        std::swap(p0.y, p1.y);
    }
    
    int dy = p1.y - p0.y;
    int dx = p1.x - p0.x;
    float derror = std::abs(dy/float(dx));
    float error = 0;
    int y = p0.y;
    int yinc = (p1.y > p0.y ? 1: -1);

    for (int x = p0.x; x <= p1.x; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error += derror;
        if (error > .5)
        {
            y += yinc;
            error -= 1;
        }
    }
}