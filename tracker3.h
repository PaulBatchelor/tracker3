#ifndef SEQ16_H
#define SEQ16_H
typedef struct tracker3_d {
    int run;
    pthread_t thread;
    GLFWwindow *window;
    char data[64 * 3];
    int please_draw;
    int chan;
    int pos;
    int octave;
    int playhead;
    char is_playing;
    SPFLOAT *notes;
    SPFLOAT *gates;
    const char *filename;
} tracker3_d;

void tracker_init(tracker3_d *track,
        const char *filename,
        SPFLOAT *notes,
        SPFLOAT *gates);

/* top-level draw function */
void tracker_draw(tracker3_d *track);
void tracker_update(tracker3_d *track);

/* movement */
void tracker_move_left(tracker3_d *track);
void tracker_move_right(tracker3_d *track);
void tracker_move_up(tracker3_d *track);
void tracker_move_down(tracker3_d *track);

/* note manipulation */
void tracker_play_note(tracker3_d *track, int off);
void tracker_play_noteoff(tracker3_d *track);
void tracker_remove_note(tracker3_d *track);
void tracker_insert_noteoff(tracker3_d *track);

/* playback */

void tracker_step(tracker3_d *track);
void tracker_play(tracker3_d *track);

void tracker_start(tracker3_d *seq);
void tracker_stop(tracker3_d *seq);
#endif
