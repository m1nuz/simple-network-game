#include <enet/enet.h>
#include <stdio.h>

#include "framework.h"
#include "physics.h"
#include "client.h"
#include "messages.h"
#include "history.h"
#include "queue.h"

struct MOUSE
{
    ivec2           cursor;
    unsigned char   buttons;
};

struct UPDATE
{
    unsigned int        step;
    float               dt;
    struct PLAYER_INPUT input;
};

static ENetAddress          address;
static ENetHost             *client;
static ENetPeer             *peer;

unsigned int                timesteps;
int                         replaying;
static int                  quit = 0;
static int                  reconciliation = 0; // TODO: fix bugs

static int                  player_id = -1;
struct PLAYER_INPUT         player_input;
struct MOUSE                mouse;

static struct PLAYER        players[MAX_PLAYERS];
static int                  players_count;
static struct BULLET        bullets[MAX_BULLETS];
static int                  bullets_count;
static QUEUE                *queue;

static void
reset_players()
{
    memset(players, 0, sizeof(players));
    players_count = 0;
}

static inline void
init_player(struct PLAYER *p)
{
    if(!p)
        return;

    memset(p, 0, sizeof(struct PLAYER));

    p->current.position.x = 0.f;
    p->current.position.y = 0.f;
    p->current.velocity.x = 0.f;
    p->current.velocity.y = 0.f;
    p->current.acceleration = 0.05f;

    p->previous = p->current;

    p->aabb = make_aabb(p->current.position, (vec2){PLAYER_SIZE, PLAYER_SIZE});

    p->color = (vec3){1.f, 1.f, 1.f};
}

static int
get_player(int id)
{
    for(int i = 0; i < MAX_PLAYERS; i++)
            if(players[i].id == id)
                return i;

    return -1;
}

static void
init_players()
{
    for(int i = 0; i < MAX_PLAYERS; i++)
        init_player(&players[i]);
}

static int
add_player(int id)
{
    if(get_player(id) != -1)
        return 0;

    printf("add player %d\n", id);
    players[players_count].id = id;

    players_count++;

    return 1;
}

static void
delete_player(int id)
{
    if(player_id == id)
        return;

    int p = get_player(id);

    if(p == -1)
        return -1;

    if(players_count > 1)
        memcpy(&players[p], &players[players_count - 1], sizeof(struct PLAYER));

    memset(&players[players_count - 1], 0, sizeof(struct PLAYER));
    players_count--;
}

static void
add_bullet(int pid, vec2 position, vec2 velocity)
{
    struct BULLET *b = &bullets[bullets_count];

    b->current.position = position;
    b->current.velocity = velocity;
    b->current.acceleration = 0.0f;

    b->previous = b->current;

    b->alive = 1;
    b->used = 1;

    bullets_count++;
    bullets_count =  bullets_count % MAX_BULLETS;
}

static vec2
mouse_point(int mouse_x, int mouse_y, int width, int height)
{
    double modelview[16] = IDENTITY_MAT4 , projection[16] = IDENTITY_MAT4;
    int viewport[4];
    float z = 0;
    double objx, objy, objz;

    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetIntegerv(GL_VIEWPORT, viewport);

    glReadPixels(mouse_x, viewport[3] - mouse_y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

    gluUnProject(mouse_x, viewport[3] - mouse_y, z, modelview, projection, viewport, &objx, &objy, &objz);

    return (vec2){objx, objy};
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

extern void
update_physics(unsigned int t, float dt, struct PLAYER_INPUT *input)
{
    int p = get_player(player_id);

    if(p != -1)
        players[p].input = *input;

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

        if((players[i].id == player_id) && !replaying)
        {
            struct PLAYER_MOVE move;
            move.input = players[i].input;
            move.state = players[i].current;
            move.time = timesteps;

            add_move(&move);
        }
    }

    for(int i = 0; i < MAX_BULLETS; i++)
    {
        bullets[i].previous = bullets[i].current;

        integrate(NULL, &bullets[i].current, dt);
    }
}

static void
client_recieve_snapshot(struct PLAYER_SNAP *snap_players, int count, unsigned int step)
{
    if(players_count != count) // TODO : find lost players!
    {
    }

    int p = -1;

    for(int i = 0; i < players_count; i++)
    {
        if(snap_players[i].id == players[i].id)
        {
            players[i].input = snap_players[i].input;
            players[i].current = snap_players[i].current;
            players[i].previous = snap_players[i].previous;
            players[i].weapon_delay = snap_players[i].weapon_delay;
        }

        if(players[i].id == player_id)
            p = i;
    }

    if(reconciliation && (p != -1))
        correction_player(step, &players[p].current, &players[p].input);

    //printf("t = %d\n", timesteps - step);
    timesteps = step;
}

static void
client_send_player_input()
{
    void *data = malloc(sizeof(struct MESSAGE) + sizeof(struct PLAYER_INPUT));

    struct MESSAGE *msg = data;
    msg->type = MSG_PLAYER_INPUT;
    msg->size = sizeof(struct PLAYER_INPUT);

    struct PLAYER_INPUT *input = data + sizeof(struct MESSAGE);

    *input = player_input;

    if(peer)
    {
        ENetPacket *packet = enet_packet_create(data, sizeof(struct MESSAGE) + sizeof(struct PLAYER_INPUT), ENET_PACKET_FLAG_RELIABLE);

        if(packet)
        {
            packet->userData = 0;

            enet_peer_send(peer, 0, packet);
        }
    }

    free(data);
}

extern int
client_init(const char *srv_address)
{
    if (enet_initialize () != 0)
    {
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        return -1;
    }

    atexit(enet_deinitialize);

    client = enet_host_create(NULL, 1, 2, 57600/8, 14400/8);

    if (client == NULL)
    {
        fprintf(stderr, "An error occured while trying to create an ENet server host\n");
        return -1;
    }

    if(srv_address)
        enet_address_set_host(&address, srv_address);
    else
        enet_address_set_host(&address, "localhost");

    address.port = SERVER_PORT;

    peer = enet_host_connect(client, &address, 2, 0);

    if (peer == NULL)
    {
        fprintf(stderr, "No available peers for initializing an ENet connection");
        return -1;
    }

    return 0;
}

extern int
client_cleanup()
{
    enet_peer_disconnect(peer, 1);
    enet_host_destroy(client);

    client = NULL;
    peer = NULL;

    return 0;
}

static void
client_process_message(void *data, size_t size)
{
    if(data == NULL)
        return;

    struct MESSAGE *msg = data;

    if(msg->type == MSG_PLAYER_ID)
    {
        int *id = data + sizeof(struct MESSAGE);
        printf("player id\n");
        player_id = *id;

        const char s[40];

        sprintf(s, "client id=%d", *id);

        setTitle(s);

        return;
    }

    if(msg->type == MSG_PLAYER_JOIN)
    {
        int *id = data + sizeof(struct MESSAGE);

        if(add_player(*id))
            printf("player join %d\n", *id);

        return;
    }

    if(msg->type == MSG_PLAYER_LEAVE)
    {
        int *id = data + sizeof(struct MESSAGE);

        printf("player leave %d\n", *id);

        delete_player(*id);

        return;
    }

    if(msg->type == MSG_SNAPSHOT)
    {

        unsigned int *step = data + sizeof(struct MESSAGE);
        int sz = msg->size - sizeof(unsigned int) - sizeof(struct MESSAGE);

        //printf("snapsho %d\n", timesteps - *step);

        client_recieve_snapshot(data + sizeof(struct MESSAGE) + sizeof(unsigned int), sz / sizeof(struct PLAYER_SNAP), step);
    }
}

extern int
client_process(void *nothing)
{
    ENetEvent event;
    int status;

    char host_name[40] = {};

    while(client)
    {
        status = enet_host_service (client, &event, 10);

        if(status > 0)
        {
            if(status > 0)
            {
                switch(event.type)
                {
                case ENET_EVENT_TYPE_CONNECT:
                    enet_address_get_host_ip(&event.peer->address, host_name, sizeof(host_name));
                    printf("%s connected\n", host_name);
                    break;

                case ENET_EVENT_TYPE_RECEIVE:
                    client_process_message(event.packet->data, event.packet->dataLength);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    enet_address_get_host_ip(&event.peer->address, host_name, sizeof(host_name));
                    printf("%s disconnected\n", host_name);

                    event.peer->data = NULL;
                    enet_packet_destroy(event.packet);
                    break;
                }
            }
        }        
    }

    return 0;
}

extern void
onQuit()
{
    quit = 1;
}

extern void
onKeyPress(unsigned char key)
{
    if(key == VK_ESCAPE)
        onQuit();

    if(key == 0x57) // W
    {
        player_input.forward = true;
    }

    if(key == 0x53) // S
    {
        player_input.back = true;
    }

    if(key == 0x41) // A
    {
        player_input.left = true;
    }

    if(key == 0x44) // D
    {
        player_input.right = true;
    }

    if(key == VK_SPACE)
    {
        player_input.shoot = true;
    }
}

extern void
onKeyRelease(unsigned char key)
{
    if(key == VK_ESCAPE)
        onQuit();

    if(key == 0x57) // W
    {
        player_input.forward = false;
    }

    if(key == 0x53) // S
    {
        player_input.back = false;
    }

    if(key == 0x41) // A
    {
        player_input.left = false;
    }

    if(key == 0x44) // D
    {
        player_input.right = false;
    }

    if(key == VK_SPACE)
    {
        player_input.shoot = false;
    }

    if(key == VK_F1)
    {
        if(reconciliation == 1)
            reconciliation = 0;
        else
            reconciliation = 1;

        const char s[40];

        sprintf(s, "client id=%d reconciliation=%d", player_id, reconciliation);

        setTitle(s);
    }
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
    mouse.cursor.x = x;
    mouse.cursor.y = y;
    mouse.buttons = buttons;

    vec2 r = mouse_point(x, y, DISPLAY_WIDTH, DISPLAY_HEIGHT);

    player_input.target_x = r.x;
    player_input.target_y = r.y;
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
    }
}



static void
glEnable2D(int width, int height)
{
    glPushAttrib(GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(0, width, height, 0, 0, 1);
}

static void
glDisable2D()
{
    glPopAttrib();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

#define GL_BGR 0x80E0
#define GL_BGRA 0x80E1

#include "resources/cursor_image.h"

GLuint cursor_map;

static void
init_cursor()
{
    glGenTextures(1, &cursor_map);
    glBindTexture(GL_TEXTURE_2D, cursor_map);

    glTexImage2D(GL_TEXTURE_2D, 0, cursor_image_internal_format, cursor_image_width, cursor_image_height, 0, cursor_image_format, GL_UNSIGNED_BYTE, cursor_image_data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}

static void
draw_cursor(int x, int y, int size)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE); // no alpha

    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, cursor_map);

    glBegin(GL_QUADS);

    glColor3f(0.6f, 0.0f, 0.8f);

    glTexCoord2f(0, 1);
    glVertex2i(x - size, y - size);

    glTexCoord2f(1, 1);
    glVertex2i(x + size, y - size);

    glTexCoord2f(1, 0);
    glVertex2i(x + size, y + size);

    glTexCoord2f(0, 0);
    glVertex2i(x - size, y + size);

    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_TEXTURE_2D);
}

static void
free_cursor()
{
    glDeleteTextures(1, &cursor_map);
}

void
update(unsigned int t, float dt, struct PLAYER_INPUT *input)
{
    if(queue_size(queue) > 1000)
        return;

    struct UPDATE upd;
    upd.step = t;
    upd.dt = dt;
    upd.input = *input;

    push_queue(queue, &upd);
}

extern int
main(int argc, char *argv[])
{
    char *srv_address = NULL;

    if(argc > 1)
        srv_address = argv[1];

    const int width = DISPLAY_WIDTH;
    const int height = DISPLAY_HEIGHT;

    if (!openDisplay("client", width, height, 0, 1))
        return 1;

    client_init(srv_address);
    init_player_history();

    createThread(client_process, NULL);
    queue = new_queue(QLOCKFREE, sizeof(struct UPDATE));

    glClearColor(0.3f, 0.3f, 0.3f, 1);

    init_cursor();
    init_players();

    float t = 0.0f;
    float dt = TIMESTEP;

    float currentTime = 0.0f;
    float accumulator = 0.0f;

    while (!quit)
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

            struct UPDATE upd;
            upd.step = timesteps;
            upd.dt = dt;
            upd.input = player_input;

            push_queue(queue, &upd);

            while (pop_queue(queue, &upd))
            {
                update_physics(upd.step, dt, &upd.input);
                update_players();
            }

            client_send_player_input();

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

        glEnable2D(width, height);

        if(player_id != -1)
            draw_cursor(mouse.cursor.x, mouse.cursor.y, 16);

        glDisable2D();

        updateDisplay();
    }

    delete_queue(queue);

    free_cursor();

    closeDisplay();

    cleanup_player_history();
    client_cleanup();

    return 0;
}
