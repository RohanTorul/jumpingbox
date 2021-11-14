#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <math.h>
#define RESX  800
#define RESY  600
#define FPS_LIMIT 16



struct Vecc
{
    int x;
    int y;
};


struct Player
{
    SDL_Rect body;
    float verticalVelocity;
    int jumpcount;
};


struct  Triangle
{
    struct Vecc top_vertex;
    struct Vecc left_vertex;
    struct Vecc right_vertex;
    int slant_length;
};


struct Player p;
struct Triangle t;
int scored = 0;
int IsRunning = 1;
int floorY =  (RESY/2) -5;
int TriangleVx = 10;
int Score = 0;
long framecount = 1;
SDL_Color textcolor = {255, 255, 255};
SDL_Window* window;
SDL_Renderer* renderer;
TTF_Font * font;

void limit_fps(unsigned int limit)
{
    unsigned int ticks = SDL_GetTicks();
    if(limit < ticks)
        return;
    else if(limit> ticks + FPS_LIMIT)
        SDL_Delay(FPS_LIMIT);
    else
        SDL_Delay(limit - ticks);

}

void ExitOnError()
{
    if( renderer != NULL)
    {
        SDL_DestroyRenderer(renderer);
        printf("Renderer Destroyed");
    }
    if( window != NULL)
    {
        SDL_DestroyWindow(window);
        printf("Window Destroyed");
    }
    TTF_Quit();
    SDL_Quit();
    printf("SDL2 Quit");
}

int Initialise_stuff()
{
    TTF_Init();

    font = TTF_OpenFont("arial.ttf", 25);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {

        printf("[DEBUG: SDL2 Failed to initialise]");
        return (-1);

    }


    window = SDL_CreateWindow("test-Jumpey",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              RESX,
                              RESY,
                              0 );
    if (window == NULL)
    {
        printf("[DEBUG: Window failed to Initialise]");
        return -1;
    }

    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    if (renderer == NULL)
    {
        printf("[DEBUG: Renderer failed to Initialise]");
        return -1;
    }

    t.top_vertex.x = RESX/2;
    t.top_vertex.y = RESY/2;
    t.slant_length = 50;

    p.body.h = 50;
    p.body.w = 50;
    p.body.x = RESX/4;
    p.body.y = (RESY/2) -5;
    p.verticalVelocity =  0;
    return 0;

}

void DrawText(char* message , int x, int  y)
{

    int texW = 0;
    int texH = 0;

    SDL_Color color = { 255, 255, 255 };
    SDL_Surface * surface = TTF_RenderText_Solid(font, message, color);
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

     SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = { x, y, texW, texH };


    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

struct Triangle Draw_Triangle(struct Triangle t)
{
    int isdrawsuccess = 1;
    int height = (int)(t.slant_length * sin((M_PI/3)));
    int half_base = (int)(t.slant_length * cos((M_PI/3)));
    t.left_vertex.y = t.top_vertex.y + height;
    t.left_vertex.x = t.top_vertex.x - half_base;
    t.right_vertex.y = t.left_vertex.y;
    t.right_vertex.x = t.top_vertex.x + half_base;
    //TODO: Function to change Render Colour.
    if (SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255) != 0)
    {
        printf("[DEBUG: Error when changing Renderer draw Color ...]");
        ExitOnError();
    }

    if (SDL_RenderDrawLine(renderer, t.top_vertex.x, t.top_vertex.y, t.left_vertex.x, t.left_vertex.y) != 0)
    {
        isdrawsuccess = 0;
    }
    if (SDL_RenderDrawLine(renderer, t.top_vertex.x, t.top_vertex.y, t.right_vertex.x, t.right_vertex.y))
    {
        isdrawsuccess = 0;
    }
    if(SDL_RenderDrawLine(renderer, t.right_vertex.x, t.right_vertex.y, t.left_vertex.x, t.left_vertex.y))
    {
        isdrawsuccess = 0;
    }

    if(isdrawsuccess != 1)
    {
        printf("[DEBUG: Error Drawing a line ...]");
        ExitOnError();
    } //else{printf("[DEBUG: ALL 3 Lines drawn] \n");}

    return t;
}

void Draw_Player()
{
    if(SDL_SetRenderDrawColor(renderer, 0, 200, 200, 225) != 0)
    {
        printf("[DEBUG: Error When Changing Renderer's color]");
        ExitOnError();
    }
    if(SDL_RenderDrawRect(renderer, &p.body) != 0)
    {
        printf("[DEBUG: Error When Drawing Player]");
    }
}

int handleInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            printf("[DEBUG: Safely Exiting ...]");
            IsRunning = 0;
        }
        switch(event.type)
        {
        case SDL_KEYDOWN:
            printf("a key has been pressed \n");
            if (event.key.keysym.sym == SDLK_SPACE)
            {
                printf("that  key is space \n");
                if(p.jumpcount <= 1)
                {
                    p.verticalVelocity = 20;
                    p.jumpcount ++;
                }

            }
        }



    }



}

int processLogic()
{
    if(framecount % 100 == 0)
    {
        TriangleVx ++;
    }

    t.top_vertex.x  -= TriangleVx;
    if( t.top_vertex.x <= 0)
    {
        t.top_vertex.x = RESX;
    }


    p.body.y -= p.verticalVelocity;
    if (p.body.y >= floorY)
    {
        p.jumpcount = 0;
        p.body.y = floorY;
        p.verticalVelocity = 0;
        scored = 0;
        if ((p.body.x >  t.left_vertex.x) && (p.body.x < t.right_vertex.x) )
        {
            Score = 0;
        }
    }

    p.verticalVelocity -= 3;

    if((p.body.y + p.body.h < (t.top_vertex.y)) && (p.body.x >  t.left_vertex.x) && (p.body.x < t.right_vertex.x) & scored == 0 )
    {
        Score++;
        scored = 1;
    }

    framecount ++;
}

int drawOnScreen()
{
    char scoretext[16];
    sprintf(scoretext, "%d", Score);

    if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0) != 0)
    {
        printf("[DEBUG: Error when changing Renderer draw Color ...]");
        ExitOnError();
    }

    SDL_RenderClear(renderer);
    DrawText(scoretext, RESX/2, RESY/4);
    t = Draw_Triangle(t);
    Draw_Player();
    SDL_RenderPresent(renderer);
    //SDL_RenderClear(renderer);

}

int main(int argc, char** argv)
{

    unsigned int frame_limit = 0;
    int feedback = 0;
    feedback = Initialise_stuff();
    if (feedback < 0 )
    {
        printf("[DEBUG: Exiting on Error...]");
        ExitOnError();
    }

    frame_limit =  SDL_GetTicks() + FPS_LIMIT;
    while(IsRunning == 1)
    {
        limit_fps(frame_limit);
        handleInput();
        processLogic();
        drawOnScreen();
        frame_limit =  SDL_GetTicks() + FPS_LIMIT;
    }

    //closing...
    ExitOnError();
    return 0;

}
