#include "history.h"
#include "circularbuffer.h"
#include "client.h"

CIRCULAR_BUFFER *moves;

extern void
init_player_history()
{
    moves = new_circular_buffer(MOVES_HISTORY_SIZE, sizeof(struct PLAYER_MOVE));
}

extern void
cleanup_player_history()
{
    delete_circular_buffer(moves);
}

extern void
add_move(struct PLAYER_MOVE *move)
{
    write_circular_buffer(moves, move);
}

extern void
correction_player(unsigned int t, struct STATE *state, struct PLAYER_INPUT *input)
{
    struct PLAYER_MOVE move = {};

    if(!is_empty_circular_buffer(moves))
        read_circular_buffer(moves, &move);

    while((move.time < t) && (!is_empty_circular_buffer(moves)))
        read_circular_buffer(moves, &move);

    if(is_empty_circular_buffer(moves))
        return;

    unsigned int saved_time;
    struct PLAYER_INPUT saved_input;

    replaying = 1;

    while (!is_empty_circular_buffer(moves))
    {
        saved_time = timesteps;
        saved_input = player_input;

        read_circular_buffer(moves, &move);

        while(timesteps < move.time)
            update(t, 0.01f, input); // TODO: fix dt
    }

    replaying = 0;

    update(saved_time, 0.01f, &saved_input); // TODO: fix dt
}
