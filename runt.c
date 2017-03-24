#define _GNU_SOURCE
#include <GLFW/glfw3.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <soundpipe.h>
#include <sporth.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <runt.h>

#include "tracker3.h"

static void tracker_define(runt_vm *vm,
    runt_ptr p,
    const char *word,
    runt_uint size,
    runt_proc proc)
{
    runt_uint word_id;
    word_id = runt_word_define(vm, word, size, proc);
    runt_word_bind_ptr(vm, word_id, p);
}

static runt_int rproc_insert(runt_vm *vm, runt_ptr p)
{
    runt_int rc;
    runt_stacklet *s;
    runt_int chan;
    runt_int pos;
    runt_int note;
    runt_int n;
    tracker3_d *track;
  
    track = runt_to_cptr(p);

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    chan = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    pos = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    note = s->f;

    n = pos * 3 + chan;

    track->data[n] = note;


    return RUNT_OK;
}

static runt_int rproc_pos(runt_vm *vm, runt_ptr p)
{
    runt_int rc;
    runt_stacklet *s;
    runt_int pos;
    tracker3_d *track;

    track = runt_to_cptr(p);
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    pos = s->f;

    track->pos = pos % 64;


    return RUNT_OK;
}

static runt_int rproc_chan(runt_vm *vm, runt_ptr p)
{
    runt_int rc;
    runt_stacklet *s;
    runt_int chan;
    tracker3_d *track;

    track = runt_to_cptr(p);
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    chan = s->f;

    track->chan = chan % 3;


    return RUNT_OK;
}

static runt_int rproc_note(runt_vm *vm, runt_ptr p)
{
    runt_int rc;
    runt_stacklet *s;
    runt_int note;
    runt_int off;
    runt_int n;
    tracker3_d *track;

    track = runt_to_cptr(p);

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    off = s->f;

    track->pos = (track->pos + off) % 64;
    n = track->pos * 3 + track->chan;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    note = s->f;

    track->data[n] = note;

    return RUNT_OK;
}

static runt_int runt_load_tracker3(runt_vm *vm, tracker3_d *track)
{
    runt_ptr p;
    
    p = runt_mk_cptr(vm, track);

    tracker_define(vm, p, "insert", 6, rproc_insert);
    tracker_define(vm, p, "pos", 3, rproc_pos);
    tracker_define(vm, p, "chan", 4, rproc_chan);
    tracker_define(vm, p, "note", 4, rproc_note);
    return RUNT_OK;
}

void tracker_runt_init(tracker3_d *track)
{
    track->buf = malloc(RUNT_MEGABYTE);
    track->cells = malloc(sizeof(runt_cell) * 512);

    runt_init(&track->vm);

    runt_memory_pool_set(&track->vm, track->buf, RUNT_MEGABYTE);
    runt_cell_pool_set(&track->vm, track->cells, 512);
    runt_cell_pool_init(&track->vm);
    runt_load_stdlib(&track->vm);
    runt_load_tracker3(&track->vm, track);
    runt_set_state(&track->vm, RUNT_MODE_INTERACTIVE, RUNT_ON);
}

void tracker_runt_free(tracker3_d *track)
{
    free(track->buf);
    free(track->cells);
}

static void parse(runt_vm *vm, char *str)
{
    const char *code = str;
    runt_pmark_set(vm);
    runt_compile(vm, code);
    runt_pmark_free(vm);
}

void tracker_runt_load(tracker3_d *track)
{
    FILE *fp;
    char *line = NULL;
    ssize_t read;
    size_t len = 0;
    fp = fopen(track->filename, "r");
    if(fp == NULL) return;

    while ((read = getline(&line, &len, fp)) != -1) {
        parse(&track->vm, line);
    }

    free(line);
    fclose(fp);
}

void tracker_runt_save(tracker3_d *track)
{
    int chan;
    int pos;
    FILE *fp;
    char note;
    int n;
    int off;

    fp = fopen(track->filename, "w");

    if(fp == NULL) return;

    for(chan = 0; chan < 3; chan++) {
        off = 0;
        fprintf(fp, "# Channel %d\n", chan);
        fprintf(fp, "0 pos\n");
        fprintf(fp, "%d chan\n", chan);
        for(pos = 0; pos < 64; pos++) {
            n = pos * 3 + chan;
            note = track->data[n];

            if(note != -1) {
                fprintf(fp, "%d %d note\n", note, off);
                off = 0;
            }

            off++;

        }

        fprintf(fp, "\n");
    }

    fprintf(fp, "0 pos 0 chan\n");
    runt_print(&track->vm, "Wrote to file %s\n", track->filename);
    fclose(fp);
}
