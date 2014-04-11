#include <stdio.h>

#include "framework.h"
#include "server.h"
#include "messages.h"
#include "collision.h"

#define LOG_FILENAME "errors.log"

//#if defined(__DEBUG) || defined(_DEBUG) || !defined(NDEBUG)
//#define init_log()
//#else
#define init_log() freopen(LOG_FILENAME, "w", stderr)
//#endif

#define log_error(format, ...) fprintf(stderr, format, __VA_ARGS__)

unsigned int            timesteps = 0;
static int              quit;

static ENetAddress      address;
static ENetHost         *server;

static struct PLAYER    players[MAX_PLAYERS];
static int              players_count;
static struct BULLET    bullets[MAX_BULLETS];
static int              bullets_count;

static int
add_player(ENetPeer *peer)
{
    static int player_id = 1000;

    if(!peer)
    {
        log_error("error: %s\n", "can\'t add empty peer");
        return -1;
    }

    if(players_count >= MAX_PLAYERS)
    {
        enet_peer_disconnect(peer, 1);

        return -1;
    }

    struct PLAYER *p = &players[players_count];

    p->id = player_id++;
    p->peer = peer;
    p->color = (vec3){1, 1, 1};

    players_count++;

    return p->id;
}

static int
get_player(ENetPeer *peer)
{
    if(!peer)
    {
        log_error("error: %s\n", "can\'t find empty peer");
        return -1;
    }

    for(int i = 0; i < MAX_PLAYERS; i++)
        if(players[i].peer)
            if(players[i].peer->connectID == peer->connectID)
                return i;

    return -1;
}

static int
delete_player(ENetPeer *peer)
{
    if(!peer)
    {
        log_error("error: %s\n", "can\'t delete empty peer");
        return -1;
    }

    int p = get_player(peer);

    if(p == -1)
        return -1;

    if(players_count > 1)
        memcpy(&players[p], &players[players_count - 1], sizeof(struct PLAYER));

    memset(&players[players_count - 1], 0, sizeof(struct PLAYER));
    players_count--;

    return 0;
}

static void
add_bullet(int pid, vec2 position, vec2 velocity)
{
    struct BULLET *b = &bullets[bullets_count];

    b->current.position = position;
    b->current.velocity = velocity;
    b->current.acceleration = 0.5f;

    b->previous = b->current;

    b->alive = 1;
    b->used = 1;

    bullets_count++;
    bullets_count =  bullets_count % MAX_BULLETS;
}

static void
print_players_list()
{
    for(int i = 0; i < MAX_PLAYERS; i++)
    {
        if(players[i].peer)
            printf("Player %2d: ID=%4d connect ID=%d\n", i, players[i].id, players[i].peer->connectID);
    }

    printf("\n");
}


static void
server_send_player_id(ENetPeer *peer, int id)
{
    if(!peer)
        return;

    void *data = malloc(sizeof(struct MESSAGE) + sizeof(int));

    struct MESSAGE *msg = data;
    msg->type = MSG_PLAYER_ID;
    msg->size = sizeof(int);

    int *value = data + sizeof(struct MESSAGE);
    *value = id;

    ENetPacket *packet = enet_packet_create(data, sizeof(struct MESSAGE) + sizeof(int), ENET_PACKET_FLAG_RELIABLE);

    if(packet)
    {
        packet->userData = 0;

        enet_peer_send(peer, 0, packet);
    }

    free(data);
}

static void
server_send_player_join(ENetPeer *peer, int id)
{
    if(!peer)
        return;

    void *data = malloc(sizeof(struct MESSAGE) + sizeof(int));

    struct MESSAGE *msg = data;
    msg->type = MSG_PLAYER_JOIN;
    msg->size = sizeof(int);

    int *value = data + sizeof(struct MESSAGE);
    *value = id;

    ENetPacket *packet = enet_packet_create(data, sizeof(struct MESSAGE) + sizeof(int), ENET_PACKET_FLAG_RELIABLE);

    if(packet)
    {
        packet->userData = 0;

        //enet_peer_send(peer, 0, packet);

        enet_host_broadcast(server, 0, packet);
    }

    free(data);
}

static void
server_send_players_join()
{
    void *data = malloc(sizeof(struct MESSAGE) + sizeof(int));

    for(int i = 0; i < players_count; i++)
    {
        struct MESSAGE *msg = data;
        msg->type = MSG_PLAYER_JOIN;
        msg->size = sizeof(int);

        int *value = data + sizeof(struct MESSAGE);
        *value = players[i].id;

        ENetPacket *packet = enet_packet_create(data, sizeof(struct MESSAGE) + sizeof(int), ENET_PACKET_FLAG_RELIABLE);

        if(packet)
        {
            packet->userData = 0;

            //enet_peer_send(peer, 0, packet);

            enet_host_broadcast(server, 0, packet);
        }
    }

    free(data);
}

static void
server_send_player_leave(ENetPeer *peer)
{
    int p = get_player(peer);

    if(p == -1)
        return;

    void *data = malloc(sizeof(struct MESSAGE) + sizeof(int));

    struct MESSAGE *msg = data;
    msg->type = MSG_PLAYER_LEAVE;
    msg->size = sizeof(int);

    int *value = data + sizeof(struct MESSAGE);
    *value = players[p].id;

    ENetPacket *packet = enet_packet_create(data, sizeof(struct MESSAGE) + sizeof(int), ENET_PACKET_FLAG_RELIABLE);

    if(packet)
    {
        packet->userData = 0;

        enet_host_broadcast(server, 0, packet);
    }

    free(data);
}

static void
server_send_snaphot()
{
    if(players_count < 1)
        return;

    const size_t size = sizeof(struct MESSAGE) + sizeof(unsigned int) + sizeof(struct PLAYER_SNAP) * players_count;

    void *data = malloc(size);

    struct MESSAGE *msg = data;
    msg->type = MSG_SNAPSHOT;
    msg->size = sizeof(unsigned int) + sizeof(struct PLAYER_SNAP) * players_count;

    unsigned int *step = data + sizeof(struct MESSAGE);
    *step = timesteps;

    struct PLAYER_SNAP *snapshot = data + sizeof(struct MESSAGE) + sizeof(unsigned int);

    for(int i = 0; i < players_count; i++)
    {
        snapshot[i].id = players[i].id;
        snapshot[i].current = players[i].current;
        snapshot[i].previous = players[i].previous;
        snapshot[i].input = players[i].input;
        snapshot[i].weapon_delay = players[i].weapon_delay;
    }

    ENetPacket *packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);

    if(packet)
    {
        packet->userData = 0;
        enet_host_broadcast(server, 0, packet);
    }

    free(data);
}

static void
server_receive_player_input(ENetPeer *peer, struct PLAYER_INPUT *input)
{
    //printf("player input %d\n", peer->connectID);

    int i = get_player(peer);

    if(i == -1)
        return;

    struct PLAYER *p = &players[i];

    p->input = *input;
}

static int
server_init()
{
    if (enet_initialize () != 0)
    {
        log_error("error: %s\n", "initializing ENet");
        return -1;
    }

    atexit (enet_deinitialize);

    address.host = ENET_HOST_ANY;
    address.port = SERVER_PORT;

    server = enet_host_create(&address, 32, 2, 0, 0);

    if (server == NULL)
    {
        log_error("error: %s\n", "create an ENet server host");
        return -1;
    }

    return 0;
}

static void
server_cleanup()
{
    enet_host_destroy(server);
}

static void
server_process_message(ENetPeer *peer, void *data, size_t size)
{
    if(data == NULL)
            return;

    struct MESSAGE *msg = data;

    if(msg->type == MSG_PLAYER_INPUT)
        server_receive_player_input(peer, data + sizeof(struct MESSAGE));
}

static void
server_process(void *nothing)
{
    ENetEvent event;
    int status;

    char host_name[40] = {};
    int pid = -1;

    while (1)
    {
        status = enet_host_service (server, &event, 10);

        if(status > 0)
        {
            switch(event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
                enet_address_get_host_ip(&event.peer->address, host_name, sizeof(host_name));
                printf("%s connected\n", host_name);

                if((pid = (add_player(event.peer))) != -1)
                {
                   server_send_player_id(event.peer, pid); // send to client
                   server_send_players_join(); // send to all
                }

                print_players_list();
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                server_process_message(event.peer, event.packet->data, event.packet->dataLength);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:

                server_send_player_leave(event.peer);

                delete_player(event.peer);

                enet_address_get_host_ip(&event.peer->address, host_name, sizeof(host_name));
                printf("%s disconnected\n", host_name);

                event.peer->data = NULL;
                enet_packet_destroy(event.packet);
                break;
            }
        }
    }
}

static void
init_players()
{
    for(int i = 0; i < MAX_PLAYERS; i++)
    {
        players[i].current.position.x = 0.f;
        players[i].current.position.y = 0.f;
        players[i].current.velocity.x = 0.f;
        players[i].current.velocity.y = 0.f;
        players[i].current.acceleration = 0.05f;

        players[i].previous = players[i].current;

        players[i].color = (vec3){1.f, 1.f, 1.f};
    }
}

static void
update_players()
{
    for(int i = 0; i < players_count; i++)
    {
        players[i].aabb = make_aabb(players[i].current.position, (vec2){PLAYER_SIZE, PLAYER_SIZE});

        players[i].hit = 0;

        for(int j = 0; j < MAX_BULLETS; j++)
            if(bullets[j].used)
            if(is_inside_aabb(players[i].aabb, bullets[j].current.position))
            {
                players[i].hit = 1;

                bullets[j].alive = 0;
            }

        if(players[i].input.shoot && (players[i].weapon_delay == 0))
        {
            vec2 pos = players[i].current.position;

            vec2 r = (vec2){players[i].input.target_x, players[i].input.target_y};

            vec2 dir;
            vec2_subv(dir, r, pos);
            vec2_normalize(dir);

            vec2 offset = (vec2){PLAYER_SIZE * dir.x * 1.2, PLAYER_SIZE * dir.y * 1.2};
            vec2 v = (vec2){BULLET_SPEED * dir.x, BULLET_SPEED * dir.y};
            vec2 p = (vec2){offset.x + players[i].current.position.x, offset.y + players[i].current.position.y};

            add_bullet(players[i].id, p, v);
        }

        players[i].weapon_delay--;

        if(players[i].weapon_delay < 0)
            players[i].weapon_delay = BULLET_DELAY;
    }
}

static void
update_physics(unsigned int t, float dt, struct PLAYER_INPUT *input)
{
    timesteps = t;

    for(int i = 0; i < players_count; i++)
    {
        if(players[i].input.forward)
            add_impulse(&players[i].current, 0, PLAYER_SPEED);

        if(players[i].input.back)
            add_impulse(&players[i].current, 0, -PLAYER_SPEED);

        if(players[i].input.left)
            add_impulse(&players[i].current, -PLAYER_SPEED, 0);

        if(players[i].input.right)
            add_impulse(&players[i].current, PLAYER_SPEED, 0);

    }

    for(int i = 0; i < players_count; i++)
    {
        players[i].previous = players[i].current;

        integrate(&players[i].input, &players[i].current, dt);
    }

    for(int i = 0; i < MAX_BULLETS; i++)
    {
        bullets[i].previous = bullets[i].current;

        integrate(NULL, &bullets[i].current, dt);
    }
}

extern void
onQuit()
{
    quit = 1;
}

extern void
onKeyPress(unsigned char key)
{
}

extern void
onKeyRelease(unsigned char key)
{
}

void
onMousePress(int x, int y, unsigned char buttons)
{
}

void
onMouseRelease(int x, int y, unsigned char buttons)
{
}

void
onMouseMove(int x, int y, unsigned char buttons)
{
}

static void
render_scene(float interpolation)
{
    for(int i = 0; i < MAX_BULLETS; i++)
    {
        if(bullets[i].alive)
        {
            struct STATE state;

            interpolate(&bullets[i].previous, &bullets[i].current, interpolation, &state);

            glPointSize(4);
            glBegin(GL_POINTS);
            glColor3f(1, 0, 0);
            glVertex3f(state.position.x, state.position.y, 0);
            glEnd();
        }
    }

    for(int i = 0; i < players_count; i++)
    {
        struct STATE state;

        interpolate(&players[i].previous, &players[i].current, interpolation, &state);

        glBegin(GL_QUADS);

        if(players[i].hit == 1)
            glColor3f(1, 0, 0);
        else
            glColor3f(players[i].color.x, players[i].color.y, players[i].color.z);

        glVertex2f(state.position.x - PLAYER_SIZE, state.position.y - PLAYER_SIZE);
        glVertex2f(state.position.x + PLAYER_SIZE, state.position.y - PLAYER_SIZE);
        glVertex2f(state.position.x + PLAYER_SIZE, state.position.y + PLAYER_SIZE);
        glVertex2f(state.position.x - PLAYER_SIZE, state.position.y + PLAYER_SIZE);
        glEnd();

        glBegin(GL_LINES);
        glColor3f(1, 1, 1);
        glVertex3f(players[i].input.target_x - 0.1f, players[i].input.target_y - 0.1f, 0);
        glVertex3f(players[i].input.target_x + 0.1f, players[i].input.target_y + 0.1f, 0);

        glVertex3f(players[i].input.target_x + 0.1f, players[i].input.target_y - 0.1f, 0);
        glVertex3f(players[i].input.target_x - 0.1f, players[i].input.target_y + 0.1f, 0);
        glEnd();
    }
}

extern int
main(int argc, char *argv[])
{
    init_log();
    srand(time(0));

    if(server_init() == -1)
        return EXIT_FAILURE;

    const int width = 800;
    const int height = 600;

    if (!openDisplay("server", width, height, 0, 1))
        return 1;

    glClearColor(0.3f, 0.3f, 0.3f, 1);

    printf("Start server...\n");

    createThread(server_process, NULL);

    init_players();

    float t = 0.0f;
    float dt = TIMESTEP;

    float currentTime = 0.0f;
    float accumulator = 0.0f;

    while(!quit)
    {
        const float newTime = time();
        float deltaTime = newTime - currentTime;
        currentTime = newTime;

        if (deltaTime <= 0.0f)
            continue;

        accumulator += deltaTime;

        while (accumulator >= dt)
        {
            accumulator -= dt;

            update_physics(timesteps, dt, NULL);
            update_players();

            server_send_snaphot();

            t += dt;
            timesteps++;
        }

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, 4.0/3.0, 1, 10);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0,0,10, 0,0,0, 0,1,0);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render_scene(accumulator / dt);

        updateDisplay();
    }

    server_cleanup();

    return 0;
}
