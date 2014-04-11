#pragma once

enum MESSAGE_TYPE
{
    MSG_PLAYER_ID,
    MSG_PLAYER_JOIN,
    MSG_PLAYER_LEAVE,
    MSG_PLAYER_INPUT,
    MSG_SNAPSHOT
};

struct MESSAGE
{
    enum MESSAGE_TYPE   type;
    int                 size;
};
