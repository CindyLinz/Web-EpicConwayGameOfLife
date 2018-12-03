#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

int Width=0, Height=0;
char *Buffer[2] = {NULL, NULL};
bool Wrap = false;
SDL_Window * window = NULL;
bool playing = false;

void init(){
    Buffer[0] = malloc(1002*1002);
    Buffer[1] = malloc(1002*1002);
    SDL_Init(SDL_INIT_VIDEO);
}

void render(){
    SDL_Surface * screen = SDL_GetWindowSurface(window);
    SDL_LockSurface(screen);
    int32_t * p = (int32_t*) screen->pixels;
    int k = Width+3;
    for(int i=1; i<=Height; ++i){
        for(int j=1; j<=Width; ++j){
            if( Buffer[0][k] )
                *p = -1;
            else
                *p = 0;
            ++p;
            ++k;
        }
        k += 2;
    }
    SDL_UnlockSurface(screen);
    SDL_UpdateWindowSurface(window);
}

void random_init(){
    int k=Width+3;
    for(int i=1; i<=Height; ++i){
        for(int j=1; j<=Width; ++j)
            Buffer[0][k++] = !(random() % 3);
        k += 2;
    }
}

void set_wrap(bool wrap){
    Wrap = wrap;
}

static inline int min(int a, int b){
    return a<b ? a : b;
}

void resize(int width, int height){
    //printf("w=%d, h=%d, W=%d, H=%d\n", width, height, Width, Height);
    if( width!=Width || height!=Height ){
        //int size = (width+2)*(height+2);
        //Buffer[0] = realloc(Buffer[0], size);
        //Buffer[1] = realloc(Buffer[1], size);

        int k = width+3;
        for(int i=1; i<=min(height,Height); ++i){
            int K = i*(Width+2) + 1;
            for(int j=1; j<=min(width,Width); ++j)
                Buffer[1][k++] = Buffer[0][K++];
            for(int j=Width+1; j<=width; ++j)
                Buffer[1][k++] = 0;
            k += 2;
        }
        k = (Height+1)*(width+2) + 1;
        for(int i=Height+1; i<=height; ++i){
            for(int j=1; j<=width; ++j)
                Buffer[1][k++] = 0;
            k += 2;
        }
        char * t = Buffer[0];
        Buffer[0] = Buffer[1];
        Buffer[1] = t;

        Width = width;
        Height = height;

        if( window )
            SDL_DestroyWindow(window);
        window = SDL_CreateWindow("", 0, 0, width, height, 0);
    }
    //random_init();
}

void step(){
    if( Wrap ){
        for(int j=1; j<=Width; ++j){
            Buffer[0][0*(Width+2)+j] = Buffer[0][Height*(Width+2)+j];
            Buffer[0][(Height+1)*(Width+2)+j] = Buffer[0][1*(Width+2)+j];
        }
        for(int i=1; i<=Height; ++i){
            Buffer[0][i*(Width+2)+0] = Buffer[0][i*(Width+2)+Width];
            Buffer[0][i*(Width+2)+(Width+1)] = Buffer[0][i*(Width+2)+1];
        }
    }
    else{
        for(int j=1; j<=Width; ++j){
            Buffer[0][0*(Width+2)+j] = 0;
            Buffer[0][(Height+1)*(Width+2)+j] = 0;
        }
        for(int i=1; i<=Height; ++i){
            Buffer[0][i*(Width+2)+0] = 0;
            Buffer[0][i*(Width+2)+(Height+1)] = 0;
        }
    }
    Buffer[0][0] = Buffer[0][Width+1] = Buffer[0][(Height+1)*(Width+2)] = Buffer[0][(Height+1)*(Width+2)+Width+1] = 0;

    int k = Width+3;
    for(int i=1; i<=Height; ++i){
        for(int j=1; j<=Width; ++j){
            char c =
                Buffer[0][k-(Width+2)-1] +
                Buffer[0][k-(Width+2)] +
                Buffer[0][k-(Width+2)+1] +
                Buffer[0][k-1] +
                Buffer[0][k+1] +
                Buffer[0][k+(Width+2)-1] +
                Buffer[0][k+(Width+2)] +
                Buffer[0][k+(Width+2)+1];
            switch(c){
                case 2:
                    Buffer[1][k] = Buffer[0][k];
                    break;
                case 3:
                    Buffer[1][k] = 1;
                    break;
                default:
                    Buffer[1][k] = 0;
            }
            ++k;
        }
        k += 2;
    }

    char *t = Buffer[0];
    Buffer[0] = Buffer[1];
    Buffer[1] = t;

    render();
}

void main_loop(){
    if( playing )
        step();

    SDL_Event event;
    while( SDL_PollEvent(&event) ){
        if( event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT ){
            int k = (event.button.y+1)*(Width+2) + event.button.x + 1;
            Buffer[0][k] = !Buffer[0][k];
            render();
        }
    }
}

void play(){
    playing = true;
}

void stop(){
    playing = false;
    render();
}

int main(){
    random_init();
    render();
    playing = true;
    emscripten_set_main_loop(main_loop, 0, 0);
    return 0;
}
