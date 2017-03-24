#include <GLFW/glfw3.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <soundpipe.h>
#include <sporth.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "tracker3.h"

void init(void)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glClearColor(0.0, 0.0, 0.0, 0.0);
}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void errorcb(int error, const char* desc)
{
	printf("GLFW error %d: %s\n", error, desc);
}

static void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    tracker3_d *track = glfwGetWindowUserPointer(window);
    if(action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch(key) {
            case GLFW_KEY_J:
                tracker_move_down(track);
                break;
            case GLFW_KEY_K:
                tracker_move_up(track);
                break;
            case GLFW_KEY_H:
                tracker_move_left(track);
                break;
            case GLFW_KEY_L:
                tracker_move_right(track);
                break;
            /* notes */
            case GLFW_KEY_Q:
                tracker_play_note(track, 0);
                break;
            case GLFW_KEY_2:
                tracker_play_note(track, 1);
                break;
            case GLFW_KEY_W:
                tracker_play_note(track, 2);
                break;
            case GLFW_KEY_3:
                tracker_play_note(track, 3);
                break;
            case GLFW_KEY_E:
                tracker_play_note(track, 4);
                break;
            case GLFW_KEY_R:
                tracker_play_note(track, 5);
                break;
            case GLFW_KEY_5:
                tracker_play_note(track, 6);
                break;
            case GLFW_KEY_T:
                tracker_play_note(track, 7);
                break;
            case GLFW_KEY_6:
                tracker_play_note(track, 8);
                break;
            case GLFW_KEY_Y:
                tracker_play_note(track, 9);
                break;
            case GLFW_KEY_7:
                tracker_play_note(track, 10);
                break;
            case GLFW_KEY_U:
                tracker_play_note(track, 11);
                break;
            case GLFW_KEY_I:
                tracker_play_note(track, 12);
                break;
            case GLFW_KEY_9:
                tracker_play_note(track, 13);
                break;
            case GLFW_KEY_O:
                tracker_play_note(track, 14);
                break;
            case GLFW_KEY_0:
                tracker_play_note(track, 15);
                break;
            case GLFW_KEY_P:
                tracker_play_note(track, 16);
                break;
            
            case GLFW_KEY_X:
                tracker_remove_note(track);
                break;

            case GLFW_KEY_TAB:
                tracker_insert_noteoff(track);
                break;
            
            case GLFW_KEY_SPACE:
                tracker_play(track);
                break;
        }
    } else if(action == GLFW_RELEASE) {
        switch(key) {
            case GLFW_KEY_Q:
            case GLFW_KEY_2:
            case GLFW_KEY_W:
            case GLFW_KEY_3:
            case GLFW_KEY_E:
            case GLFW_KEY_R:
            case GLFW_KEY_5:
            case GLFW_KEY_T:
            case GLFW_KEY_6:
            case GLFW_KEY_Y:
            case GLFW_KEY_7:
            case GLFW_KEY_U:
            case GLFW_KEY_I:
            case GLFW_KEY_9:
            case GLFW_KEY_O:
            case GLFW_KEY_0:
            case GLFW_KEY_P:
                tracker_play_noteoff(track);    
                break;

        }
    }
    
}

static void * run_loop(void *ud)
{
    tracker3_d *track = (tracker3_d *)ud;

    int w, h;

	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		pthread_exit(NULL);
	}

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    track->window = glfwCreateWindow(89, 521, "tracker3", NULL, NULL);

	if (!track->window) {
		glfwTerminate();
		pthread_exit(NULL);
	}

	glfwSetKeyCallback(track->window, key);
	glfwSetErrorCallback(errorcb);
	glfwMakeContextCurrent(track->window);

	glfwSwapInterval(0);
	glfwSetTime(0);
    init();
    glfwSetWindowUserPointer(track->window, track);
    while(track->run) {
        glfwPollEvents();
		glfwGetFramebufferSize(track->window, &w, &h);

        glViewport(0, 0, (GLsizei) w, (GLsizei) h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, w, h, 0, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);

        if(track->please_draw) {
            track->please_draw = 0;
            tracker_draw(track);
            glfwSwapBuffers(track->window);
        }
        usleep(8000);
    }


	glfwTerminate();
    pthread_exit(0);
    return NULL;
}

void tracker_init(tracker3_d *track,
        const char *filename,
        SPFLOAT *notes,
        SPFLOAT *gates)
{
    track->run = 1;
    memset(track->data, -1, sizeof(char) * 64 * 3);
    track->please_draw = 1;
    track->pos = 0;
    track->chan = 0;
    track->octave = 5;
    track->is_playing = 0;
    track->playhead = 0;

    track->notes = notes;
    track->gates = gates;
    track->filename = filename;
}

void tracker_start(tracker3_d *seq)
{
    pthread_create(&seq->thread, NULL, run_loop, seq);
}

void tracker_stop(tracker3_d *seq)
{
    seq->run = 0;
    pthread_join(seq->thread, NULL);
}

void tracker_update(tracker3_d *track)
{
    track->please_draw = 1;
}

void tracker_move_down(tracker3_d *track)
{
    track->pos++;

    if(track->pos >= 64) {
        track->pos = 0;
    }

    tracker_update(track);
}

void tracker_move_up(tracker3_d *track)
{
    track->pos--;

    if(track->pos < 0) {
        track->pos = 63;
    }

    tracker_update(track);
}

void tracker_move_left(tracker3_d *track)
{
    track->chan--;
    if(track->chan < 0) {
        track->chan = 2;
    }

    tracker_update(track);
}

void tracker_move_right(tracker3_d *track)
{
    track->chan++;
    if(track->chan >= 3) {
        track->chan = 0;
    }

    tracker_update(track);
}
                
void tracker_play_note(tracker3_d *track, int off)
{
    int n;
    char note;

    n = track->pos * 3 + track->chan;
    note = track->octave * 12 + off;

    track->gates[track->chan] = 1.0;
    track->notes[track->chan] = note;
    if(!track->is_playing) {
        track->data[n] = note;
    }
    
    tracker_update(track);
}

void tracker_play_noteoff(tracker3_d *track)
{
    printf("noteoff!\n");
    track->gates[track->chan] = 0;
}

void tracker_remove_note(tracker3_d *track)
{
    int n;
    n = track->pos * 3 + track->chan;

    track->data[n] = -1;
    tracker_update(track);
}

void tracker_insert_noteoff(tracker3_d *track)
{
    int n;
    n = track->pos * 3 + track->chan;
    track->data[n] = -2;
    tracker_update(track);
}

void tracker_step(tracker3_d *track)
{
    int n;
    char note;
    int chan;
    tracker_update(track);
    if(track->is_playing) {

        track->playhead = (track->playhead + 1) % 64;
        for(chan = 0; chan < 3; chan ++) {
            n = track->playhead * 3 + chan;
            note = track->data[n];
            if(note == -2) {
                track->gates[chan] = 0;
            } else if(note == -1) {
            } else {
                track->gates[chan] = 1;
                track->notes[chan] = note;
            }
        }

    }

}

void tracker_play(tracker3_d *track)
{
    int i;
    track->playhead = -1;
    for(i = 0; i < 3; i++) track->gates[i] = 0;
    if(track->is_playing == 0) {
        track->is_playing = 1;
    } else {
        track->is_playing = 0;
    }

    tracker_update(track);
}
