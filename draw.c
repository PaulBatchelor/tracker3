#include <GLFW/glfw3.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <soundpipe.h>
#include <sporth.h>
#include <pthread.h>
#include <unistd.h>
#include <runt.h>
#include "tracker3.h"

#include "bitmaps.h"

static void clear_color_rgb(uint32_t rgb)
{
    int r, g, b;
    r = (rgb & 0xff0000) >> 16;
    g = (rgb & 0x00ff00) >> 8;
    b = (rgb & 0x0000ff);
    glClearColor(r / 255.0, g / 255.0, b / 255.0, 1.0);
}

static void color_rgb(uint32_t rgb)
{
    int r, g, b;
    r = (rgb & 0xff0000) >> 16;
    g = (rgb & 0x00ff00) >> 8;
    b = (rgb & 0x0000ff);
    glColor3f(r / 255.0, g / 255.0, b / 255.0);
}

static void draw_number(unsigned char num, 
        GLfloat xo, 
        GLfloat yo, 
        GLfloat xm, 
        GLfloat ym)
{
    glBitmap(5, 5, xo, yo, xm, ym, numbers + num * 5);
}

static void draw_letter(unsigned char note, 
        GLfloat xo, 
        GLfloat yo, 
        GLfloat xm, 
        GLfloat ym)
{
    glBitmap(5, 5, xo, yo, xm, ym, notes + ((note + 2) % 7) * 5);
}

static void draw_acc(unsigned char a, 
        GLfloat xo, 
        GLfloat yo, 
        GLfloat xm, 
        GLfloat ym)
{
    glBitmap(5, 5, xo, yo, xm, ym, accid + a * 5);
}


static void draw_note(unsigned char n, int xo, int yo)
{

    int not, oct, acc;
    oct = (n / 12) - 1;
    not = n % 12;

    switch(not) {
        case 0:
            not = 0;
            acc = 0;
            break;
        case 1:
            not = 0;
            acc = 1;
            break;
        case 2:
            not = 1;
            acc = 0;
            break;
        case 3:
            not = 1;
            acc = 1;
            break;
        case 4:
            not = 2;
            acc = 0;
            break;
        case 5:
            not = 3;
            acc = 0;
            break;
        case 6:
            not = 3;
            acc = 1;
            break;
        case 7:
            not = 4;
            acc = 0;
            break;
        case 8:
            not = 4;
            acc = 1;
            break;
        case 9:
            not = 5;
            acc = 0;
            break;
        case 10:
            not = 5;
            acc = 1;
            break;
        case 11:
            not = 6;
            acc = 0;
            break;
    }

    draw_letter(not, 1, 2, 6, 0);
    draw_acc(acc, 1, 2, 6, 0);
    draw_number(oct, 1, 2, xo - 12, yo);
}

void tracker_draw(tracker3_d *track)
{
    int i, x, y, xo, yo;
    int playhead;
    clear_color_rgb(0x181818);
    color_rgb(0xF5AA9D);
    glClear(GL_COLOR_BUFFER_BIT);
    /*
    glRasterPos2i(4, 12);
    draw_number(0, 0, 0, 8, 0);
    draw_number(1, 0, 0, 8, 0);
    draw_number(2, 0, 0, 8, 0);
    draw_number(3, 0, 0, 8, 0);
    draw_number(4, 0, 0, 8, 0);
    draw_number(5, 0, 0, 8, 0);
    draw_number(6, 0, 0, 8, 0);
    draw_number(7, 0, 0, 8, 0);
    draw_number(8, 0, 0, 8, 0);
    draw_number(9, 0, 0, 8, 0);
    */


    glRasterPos2i(0, 8);

    for(i = 0; i < 65; i++) {
        glBitmap(1, 8, 0, 0, 16, 0, lines);
        glBitmap(1, 8, 0, 0, 24, 0, lines);
        glBitmap(1, 8, 0, 0, 24, 0, lines);
        glBitmap(1, 8, 0, 0, 24, 0, lines);
        glBitmap(1, 8, 0, 0, -88, -8, lines);
    }

    glRasterPos2i(0, 9);
    for(i = 0; i < 11; i++) {
        glBitmap(8, 1, 0, 0, 8, 0, lines + 8);
    }
    glRasterPos2i(0, 1);
    for(i = 0; i < 11; i++) {
        glBitmap(8, 1, 0, 0, 8, 0, lines + 8);
    }
    glRasterPos2i(0, 521);
    for(i = 0; i < 11; i++) {
        glBitmap(8, 1, 0, 0, 8, 0, lines + 8);
    }
    
    glRasterPos2i(3, 15);
    y = 0;
    for(y = 0; y < 7; y++) {
        for(x = 0; x < 10; x++) {
            if(y * 10 + x < 64) {
                draw_number(y, 0, 0, 6, 0);
                draw_number(x, 0, 0, -6, -8);
            }
        }
    }

    glRasterPos2i(20, 7);

    /* track names */ 

    /* +-- */
    glBitmap(5, 5, 0, 0, 6, 0, square + 5);
    glBitmap(5, 5, 0, 0, 6, 0, square);
    glBitmap(5, 5, 0, 0, 12, 0, square);
    
    /* -+- */
    glBitmap(5, 5, 0, 0, 6, 0, square);
    glBitmap(5, 5, 0, 0, 6, 0, square + 5);
    glBitmap(5, 5, 0, 0, 12, 0, square);
    
    /* --+ */
    glBitmap(5, 5, 0, 0, 6, 0, square);
    glBitmap(5, 5, 0, 0, 6, 0, square);
    glBitmap(5, 5, 0, 0, 12, 0, square + 5);

     
    glRasterPos2i(21, 13);
    for(i = 0; i < 64; i++) {
        for(x = 0; x < 3; x++) {
            if(x == 2) {
                xo = -48;
                yo = -8;
            } else {
                xo = 24;
                yo = 0;
            }
            if(track->data[3 * i + x] == -1) {
                glBitmap(15, 1, 0, 0, xo, yo, lines + 9);
            } else if(track->data[3 * i + x] == -2) {
                glBitmap(1, 4, 1, 1, 17, 0, lines + 8);
                glBitmap(1, 4, 1, 1, -17, 0, lines + 8);
                glBitmap(8, 1, 1, -2, 2, 0, lines + 8);
                glBitmap(8, 1, 1, -2, 8, 0, lines + 8);
                glBitmap(8, 1, 1, -2, -10, 0, lines + 8);


                glBitmap(8, 1, 1, 2, 2, 0, lines + 8);
                glBitmap(8, 1, 1, 2, 8, 0, lines + 8);
                glBitmap(8, 1, 1, 2, xo - 10, yo, lines + 8);
                /*glBitmap(8, 1, 0, 2, 8, 0, lines + 8);*/
                /* glBitmap(8, 1, 0, 2, xo - 8, yo, lines + 8); */
            } else {
                draw_note(track->data[3 * i + x], xo, yo);

            }
        }
    }
    
    glRasterPos2i(15 + 24 * track->chan, 16 + 8 * track->pos);
    glBitmap(1, 8, 0, 0, 0, 7, lines);
    glBitmap(8, 1, 0, 0, 8, 0, lines + 8);
    glBitmap(8, 1, 0, 0, 8, 0, lines + 8);
    glBitmap(8, 1, 0, 0, 8, 0, lines + 8);
    glBitmap(3, 1, 0, 0, 2, -8, lines + 8);
    glBitmap(1, 8, 0, 0, -8, 0, lines);
    glBitmap(8, 1, 0, 0, -8, 0, lines + 8);
    glBitmap(8, 1, 0, 0, -8, 0, lines + 8);
    glBitmap(8, 1, 0, 0, -2, 0, lines + 8);
    glBitmap(8, 1, 0, 0, -8, 0, lines + 8);

    if(track->is_playing) {
        if(track->playhead < 0) playhead = 0;
        else playhead = track->playhead;

        glRasterPos2i(0, 8 + playhead * 8);
        for(i = 0; i < 11; i++) {
            glBitmap(8, 1, 0, 0, 8, 0, lines + 8);
        }
        glRasterPos2i(0, 17 + playhead * 8);
        for(i = 0; i < 11; i++) {
            glBitmap(8, 1, 0, 0, 8, 0, lines + 8);
        }
    }

    glFlush();
}
