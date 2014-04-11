#pragma once

struct PLAYER_MOVE;
struct STATE;
struct PLAYER_INPUT;

void init_player_history();
void cleanup_player_history();

void add_move(struct PLAYER_MOVE *move);
void correction_player(unsigned int t, struct STATE *state, struct PLAYER_INPUT *input);

