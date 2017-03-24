#include <GLFW/glfw3.h>
#include <pthread.h>
#include <stdlib.h>
#include <soundpipe.h>
#include <sporth.h>
#include <runt.h>
#include "tracker3.h"

static int sporth_tracker3(plumber_data *pd, sporth_stack *stack, void **ud)
{
    tracker3_d *seq;
    SPFLOAT in;
    char *gates, *notes;
    char *filename;
    sp_ftbl *ft_gates;
    sp_ftbl *ft_notes;
    switch(pd->mode) {
        case PLUMBER_CREATE:
            seq = malloc(sizeof(tracker3_d));
            *ud = seq;
            filename = sporth_stack_pop_string(stack);
            gates = sporth_stack_pop_string(stack);
            notes = sporth_stack_pop_string(stack);
            sporth_stack_pop_float(stack);

            sp_ftbl_create(pd->sp, &ft_gates, 3);
            plumber_ftmap_add(pd, gates, ft_gates);
            
            sp_ftbl_create(pd->sp, &ft_notes, 3);
            plumber_ftmap_add(pd, notes, ft_notes);

            tracker_init(seq, filename, ft_notes->tbl, ft_gates->tbl);

            break;

        case PLUMBER_INIT:
            seq = *ud;
            sporth_stack_pop_string(stack);
            sporth_stack_pop_string(stack);
            sporth_stack_pop_string(stack);
            sporth_stack_pop_float(stack);
            tracker_start(seq);
            break;

        case PLUMBER_COMPUTE:
            seq = *ud;
            in = sporth_stack_pop_float(stack);
            if(in != 0) {
                tracker_step(seq);
            }
            break;
        case PLUMBER_DESTROY:
            seq = *ud;
            tracker_stop(seq);
            free(seq);
            break;
    }
    return PLUMBER_OK;
}

plumber_dyn_func sporth_return_ugen()
{
    return sporth_tracker3;
}
