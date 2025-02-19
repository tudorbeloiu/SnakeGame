#include <iostream>
#include <vector>
#include <SDL3/SDL.h>
#include <string>
#include <stack>
#include <map>
#include <cstdlib>
#include <ctime>

using namespace std;

constexpr int wScreenWidth = 1024;
constexpr int wScreenHeight = 768;

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRender = nullptr;
SDL_FRect snakeHead{
    (wScreenWidth-30)/2.0f,
    (wScreenHeight-30)/2.0f,
    30.0f,
    30.0f
};

SDL_Texture* apple = nullptr;
SDL_FRect appleRect;


float dx = 3.5f;
float dy = 3.5f;

float dx_nou = 0.0f;
float dy_nou = 0.0f;

vector<SDL_FRect> snakeBody;
constexpr int segmentSize = 30;
constexpr int initialSnakeLength = 20; 

int delay = 16;
int actualDelay = 0;

void generateApplePosition(){
    bool validPosition = false;
    while(!validPosition){
        appleRect.x = static_cast<float>(rand()%(wScreenWidth-segmentSize));
        appleRect.y = static_cast<float>(rand()%(wScreenHeight-segmentSize));
        appleRect.w = segmentSize;
        appleRect.h = segmentSize;

        validPosition = true;
        for(const auto& segment: snakeBody){
            if(SDL_HasRectIntersectionFloat(&segment,&appleRect)){
                validPosition = false;
                break;
            }
        }
    }

}


bool init(){
    bool success = true;
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        SDL_Log("Eroare! SDL error: %s\n", SDL_GetError());
        success = false;
    }
    else{
        gWindow = SDL_CreateWindow("Snake Game", wScreenWidth, wScreenHeight, 0);
        if (gWindow == nullptr) {
            SDL_Log("Eroare! SDL error: %s\n", SDL_GetError());
            success = false;
        }
        gRender = SDL_CreateRenderer(gWindow, 0);
        if(gRender == nullptr){
            SDL_Log("Eroare! SDL error: %s\n", SDL_GetError());
            success = false;
        }
        apple = SDL_CreateTexture(gRender,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,20,20);
        if(!apple){
            SDL_Log("Eroare la crearea marului. SDL error: %s\n",SDL_GetError());
            success = false;
        }

        SDL_SetRenderTarget(gRender,apple);
        SDL_SetRenderDrawColor(gRender,255,0,0,255);
        SDL_RenderClear(gRender);

        SDL_SetRenderTarget(gRender,nullptr);

        //initializam random pozitia marului
        srand(time(nullptr));

        snakeBody.push_back({(wScreenWidth-segmentSize)/2.0f,(wScreenHeight-segmentSize)/2.0f,segmentSize,segmentSize});
        for(int i = 1; i <= initialSnakeLength; i++){
            snakeBody.push_back({snakeBody[0].x - i * segmentSize, snakeBody[0].y, segmentSize, segmentSize});
        }
        generateApplePosition();
        
    }
    return success;
}

void close(){
    SDL_DestroyTexture(apple);
    SDL_DestroyWindow(gWindow);
    SDL_DestroyRenderer(gRender);

    gWindow = nullptr;
    gRender = nullptr;
    apple = nullptr;

    SDL_Quit();
}




int main(){
    if(init() == false){
        SDL_Log("Initializare esuata!");
        return 1;
    }
    bool quit = false;
    SDL_Event e;
    SDL_zero(e);

    while(quit == false){
        while(SDL_PollEvent(&e)){
            if(e.type == SDL_EVENT_QUIT){
                quit = true;
            }
            if(e.type == SDL_EVENT_KEY_DOWN){
                switch(e.key.key){
                    case SDLK_LEFT:
                        dx_nou = -3.5f;
                        dy_nou = 0.0f;
                        break;
                    case SDLK_RIGHT:
                        dx_nou = 3.5f;
                        dy_nou = 0.0f;
                        break;
                    case SDLK_UP:
                        dx_nou = 0.0f;
                        dy_nou = -3.5f;
                        break;
                    case SDLK_DOWN:
                        dx_nou = 0.0f;
                        dy_nou = 3.5f;
                        break;
                }
            }
        }

        SDL_FRect newHead = {snakeBody[0].x + dx_nou,snakeBody[0].y+dy_nou,segmentSize,segmentSize};
        if(newHead.x < 0 || newHead.x + newHead.w > wScreenWidth || newHead.y<0 || newHead.y + newHead.h > wScreenHeight){
            quit = true;
        }


        for(int i = snakeBody.size()-1; i>0; i--){
            snakeBody[i] = snakeBody[i-1];
        }
        snakeBody[0] = newHead;

        if(SDL_HasRectIntersectionFloat(&snakeBody[0],&appleRect)){
            SDL_FRect lastSegment = snakeBody.back();
            for(int i = 1; i <= 10; i++){
                float newX = lastSegment.x;
                float newY = lastSegment.y;

                if(dx_nou>0)
                    newX -= segmentSize;
                else if(dx_nou<0)
                    newX += segmentSize;
                else if(dy_nou>0)
                    newY -= segmentSize;
                else if(dy_nou<0)
                    newY -= segmentSize;
                snakeBody.push_back({newX,newY,segmentSize,segmentSize});

                lastSegment.x = newX;
                lastSegment.y = newY;
            }
            generateApplePosition();
        }

        SDL_SetRenderDrawColor(gRender, 0, 0, 0, 255);
        SDL_RenderClear(gRender);

        SDL_SetRenderDrawColor(gRender, 44, 255, 58, 255);
        SDL_RenderFillRect(gRender, &snakeBody[0]);

        SDL_SetRenderDrawColor(gRender, 44, 100, 58, 255);
        for (size_t i=1; i<snakeBody.size(); i++) {
            SDL_RenderFillRect(gRender, &snakeBody[i]);
        }

        SDL_RenderTexture(gRender,apple,nullptr,&appleRect);

        SDL_RenderPresent(gRender);

        SDL_Delay(16);
    }
    close();
    return 0;
}