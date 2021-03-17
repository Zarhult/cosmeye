#include <SDL2/SDL_render.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>

/* Define window size */
#define W 1920
#define H 1080
/* Map dimensions */
#define MAP_W 10
#define MAP_H 10

/* loads txt mapfile into array representation of the world */
static void load_data(bool arr[MAP_W][MAP_H]) {
    char* const mapfile = "src/map-clear.txt";
    FILE* fp = fopen(mapfile, "r");
    if (!fp) {
        perror(mapfile);
        exit(EXIT_FAILURE);
    }

    char buf[256];
    size_t const line_len = MAP_W;
    size_t line_num = 0;
    while(fgets(buf, 256, fp)) {
        /* ignore blank lines and comments starting with # */
        if (buf[0] == '#' || isspace(buf[0])) continue;

        for (size_t i = 0; i < line_len; ++i) {
            arr[line_num][i] = buf[i] == '1' ? true : false;
        }
        ++line_num;
    }

    fclose(fp);
}

struct xy {
    int x;
    int y;
};

struct line { 
    struct xy pt1;
    struct xy pt2;
};

static void draw_line(SDL_Renderer* renderer, struct line ln) {
    int delta_x = ln.pt2.x - ln.pt1.x;
    int x_increment = (delta_x > 0) ? 1 : -1;
    double slope = ((double)ln.pt2.y - (double)ln.pt1.y) / (double)delta_x;
    double y = ln.pt1.y;

    for (int x = ln.pt1.x; (ln.pt2.x - x) != 0; x += x_increment) {
        y += slope;
        SDL_RenderDrawPoint(renderer, x, (int)y);
    }
}

struct entity {
    struct xy pos;
    struct xy size; // size of x by y pixels
    int speed; // pixels of movement per frame
    double angle; // view angle in radians
    Uint8 color[4]; // rgba
};

static void move_entity(struct entity* ent, int delta_x, int delta_y) {
    ent->pos.x += delta_x;
    ent->pos.y += delta_y;
}

static void draw_entity(SDL_Renderer* renderer, struct entity ent) {
    SDL_SetRenderDrawColor(renderer,
                           ent.color[0], ent.color[1],
                           ent.color[2], ent.color[3]);

    /* TODO: draw angled square/rectangle by drawing the lines of the shape one by one */

}

int main(void) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    bool map[MAP_W][MAP_H];
    load_data(map);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Failed to initialize SDL2: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    window = SDL_CreateWindow("cosmeye",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!window) {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    if (!renderer) {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    /* set up blank black screen */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_ShowCursor(SDL_DISABLE);

    struct entity player = {{0, 0}, {20, 20}, 1, 0.0, {255, 255, 255, 255}};
    move_entity(&player, W / 2 - player.size.x / 2,
                H / 2 - player.size.y / 2);
    SDL_Event event;
    bool game_over = false;
    while (!game_over) {
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                /* close button clicked */
            case SDL_QUIT:
                game_over = true;
                break;
            }
        }

        Uint8 const* keystates = SDL_GetKeyboardState(NULL);
        if (keystates[SDL_SCANCODE_ESCAPE] || keystates[SDL_SCANCODE_Q]) {
            game_over = true;
        }
        if (keystates[SDL_SCANCODE_W]) {
            move_entity(&player, player.speed * sin(player.angle),
                        -(player.speed * cos(player.angle)));
        }
        if (keystates[SDL_SCANCODE_A]) {
            player.angle -= 0.005;
        }
        if (keystates[SDL_SCANCODE_S]) {
            move_entity(&player, -(player.speed * cos(player.angle)),
                        player.speed * sin(player.angle));
        }
        if (keystates[SDL_SCANCODE_D]) {
            player.angle += 0.005;
        }

        /* wipe screen for re-rendering */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        /* make necessary renders */
        //draw_entity(renderer, player);
        /* TEST */
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        draw_line(renderer, (struct line){{3.0, 5.0}, {100.0, 200.0}});

        /* present the frame */
        SDL_RenderPresent(renderer);

        /* 60 fps */
        SDL_Delay(1.0 / 60.0);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
