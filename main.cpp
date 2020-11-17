#include "headers/chip8.hpp"
#include "SDL.h"
#include <iostream>
#include <cstdio>
#include <chrono>
#include <string>

bool graphicsInit();
bool audioInit();

void drawGraphics();

void close();

SDL_Window* gWindow = NULL;
SDL_Surface* gSurface = NULL;
SDL_Renderer* gRenderer;
SDL_Event gE;

const int K_SCREEN_WIDTH = 64;
const int K_SCREEN_HEIGHT = 32;

const int X_SCALE = 10;
const int Y_SCALE = 10;

float gDelay = 1000/400.0f;

bool gQuit = false;

Chip8 gEmu;

int main(int argc, char* args[])
{
    
    //Checking arguments
    if (argc == 1)
    {
        std::cout << "Missing arguments.\nUsage: \'chip8emu path_to_a_game.ch8\'\n";
        std::getchar();
        return 1;
    }

    //Calculate delay
    if (argc > 2)
    {
        try
        {
            int argument = std::stoi(std::string(args[2]));
            if (argument <= 0)
            {
                std::cout << "The number mustn't be equal or lower than 0.";
                std::getchar();
                return 1;
            }
            gDelay = 1000.0f/argument;
        }
        catch(std::invalid_argument& e)
        {
            std::cout << "No conversion found for 'operations per second' number! Please use non-decimal number without characters.";
            std::getchar();
            return 1;
        }
        catch(std::out_of_range& e)
        {
            std::cout << "The 'operations per second' number is too large or too small.";
            std::getchar();
            return 1;
        }
    }

    //SDL init
    graphicsInit();
    audioInit();

    //Chip8 init
    gEmu.init();
    gEmu.loadGame(args[1]);

    std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
    std::chrono::system_clock::time_point b = std::chrono::system_clock::now();

    //Main loop
    while (!gQuit)
    {

        a = std::chrono::system_clock::now();
        std::chrono::duration<double, std::milli> work_time = a - b;

        if (work_time.count() < gDelay)
        {
            std::chrono::duration<double, std::milli> delta_ms(gDelay - work_time.count());
            auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
            SDL_Delay(delta_ms_duration.count());
        }

        gEmu.emulateCycle();

        while( SDL_PollEvent( &gE ) != 0 )
        {
            //User requests quit
            if( gE.type == SDL_QUIT )
            {
                gQuit = true;
            }
            else if (gE.type == SDL_KEYDOWN)
            {
                switch (gE.key.keysym.sym)
                {
                    case SDLK_1:
                        gEmu.key[0x1] = 1;
                    break;
                    case SDLK_2:
                        gEmu.key[0x2] = 1;
                    break;
                    case SDLK_3:
                        gEmu.key[0x3] = 1;
                    break;
                    case SDLK_4:
                        gEmu.key[0xC] = 1;
                    break;
                    case SDLK_q:
                        gEmu.key[0x4] = 1;
                    break;
                    case SDLK_w:
                        gEmu.key[0x5] = 1;
                    break;
                    case SDLK_e:
                        gEmu.key[0x6] = 1;
                    break;
                    case SDLK_r:
                        gEmu.key[0xD] = 1;
                    break;
                    case SDLK_a:
                        gEmu.key[0x7] = 1;
                    break;
                    case SDLK_s:
                        gEmu.key[0x8] = 1;
                    break;
                    case SDLK_d:
                        gEmu.key[0x9] = 1;
                    break;
                    case SDLK_f:
                        gEmu.key[0xE] = 1;
                    break;
                    case SDLK_z:
                        gEmu.key[0xA] = 1;
                    break;
                    case SDLK_x:
                        gEmu.key[0x0] = 1;
                    break;
                    case SDLK_c:
                        gEmu.key[0xB] = 1;
                    break;
                    case SDLK_v:
                        gEmu.key[0xF] = 1;
                    break;
                }
            }
            else if (gE.type == SDL_KEYUP)
            {
                switch (gE.key.keysym.sym)
                {
                    case SDLK_1:
                        gEmu.key[0x1] = 0;
                    break;
                    case SDLK_2:
                        gEmu.key[0x2] = 0;
                    break;
                    case SDLK_3:
                        gEmu.key[0x3] = 0;
                    break;
                    case SDLK_4:
                        gEmu.key[0xC] = 0;
                    break;
                    case SDLK_q:
                        gEmu.key[0x4] = 0;
                    break;
                    case SDLK_w:
                        gEmu.key[0x5] = 0;
                    break;
                    case SDLK_e:
                        gEmu.key[0x6] = 0;
                    break;
                    case SDLK_r:
                        gEmu.key[0xD] = 0;
                    break;
                    case SDLK_a:
                        gEmu.key[0x7] = 0;
                    break;
                    case SDLK_s:
                        gEmu.key[0x8] = 0;
                    break;
                    case SDLK_d:
                        gEmu.key[0x9] = 0;
                    break;
                    case SDLK_f:
                        gEmu.key[0xE] = 0;
                    break;
                    case SDLK_z:
                        gEmu.key[0xA] = 0;
                    break;
                    case SDLK_x:
                        gEmu.key[0x0] = 0;
                    break;
                    case SDLK_c:
                        gEmu.key[0xB] = 0;
                    break;
                    case SDLK_v:
                        gEmu.key[0xF] = 0;
                    break;
                }
            }
        }

        if (gEmu.drawFlag)
        {
            drawGraphics();
        }
        b = std::chrono::system_clock::now();
        std::chrono::duration<double, std::milli> sleep_time = b - a;
    }

    close();
    return 0;
} 

bool graphicsInit()
{
    //init
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return 1;
    }
    //Creating window
    gWindow = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, K_SCREEN_WIDTH * X_SCALE, K_SCREEN_HEIGHT * Y_SCALE, SDL_WINDOW_SHOWN);
    if (gWindow == NULL)
    {
        return 1;
    }

    gRenderer =  SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

    //Getting window surface
    gSurface = SDL_GetWindowSurface(gWindow);

    return 0;
}

bool audioInit()
{
    //TODO: Init
    std::cout << "Audio still not done!\n";
    return 0;
}

void drawGraphics()
{
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);
    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            if (gEmu.gfx[y * 64 + x] == 1)
            {
                SDL_Rect r;
                r.w = X_SCALE;
                r.h = Y_SCALE;
                r.x = x * X_SCALE;
                r.y = y * Y_SCALE;
                SDL_RenderFillRect(gRenderer, &r);
            }
        }
    }
    SDL_RenderPresent(gRenderer);
}

void close()
{
    //Destroy window
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

    SDL_Quit();
}
