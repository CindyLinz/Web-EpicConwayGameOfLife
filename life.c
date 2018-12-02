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
        for(int j=1; j<=Width; ++j){
            Buffer[0][k] = random() % 2;
            ++k;
        }
        k += 2;
    }
}

void set_wrap(bool wrap){
    Wrap = wrap;
}

void resize(int width, int height){
    if( width!=Width || height!=Height ){
        int size = (width+2)*(height+2);
        Buffer[0] = realloc(Buffer[0], size);
        Buffer[1] = realloc(Buffer[1], size);
        Width = width;
        Height = height;

        if( window )
            SDL_DestroyWindow(window);
        window = SDL_CreateWindow("", 0, 0, width, height, 0);
    }
    random_init();
}

void step(){
    if( Wrap ){
        for(int j=1; j<=Width; ++j){
            Buffer[0][0*(Width+2)+j] = Buffer[0][Height*(Width+2)+j];
            Buffer[0][(Height+1)*(Width+2)+j] = Buffer[0][1*(Width+2)+j];
        }
        for(int i=1; i<=Height; ++i){
            Buffer[0][i*(Width+2)+0] = Buffer[0][i*(Width+2)+Height];
            Buffer[0][i*(Width+2)+(Height+1)] = Buffer[0][i*(Width+2)+1];
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
    step();
}

int main(){
    SDL_Init(SDL_INIT_VIDEO);
    resize(200, 200);
    render();
    //window = SDL_CreateWindow("", 0, 0, 100, 100, 0);

    emscripten_set_main_loop(main_loop, 0, 0);
    return 0;
}
