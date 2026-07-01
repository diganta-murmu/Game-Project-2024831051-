#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <cstdlib> 
#include <ctime>   
#include <string> 

using namespace std;

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;

const int GRID_SIZE = 20;
const int BODY_SIZE = 15;

struct Point {
    int x, y;
};

enum GameState { PLAYING, GAME_OVER };
GameState currentState = PLAYING;

//int foodX, foodY;
vector<Point>foods;
const int MAX_FOODS = 1;

void SpawnallFood(){
    foods.clear();
    int columns = SCREEN_WIDTH / GRID_SIZE;
    int rows = SCREEN_HEIGHT / GRID_SIZE;
    for(int i=0; i<MAX_FOODS; i++){
        int fx = (rand() % columns) * GRID_SIZE;
        int fy = (rand() % rows) * GRID_SIZE;
        foods.push_back({fx, fy});
    }
}

// void SpawnFood() {
//     int columns = SCREEN_WIDTH / GRID_SIZE;
//     int rows = SCREEN_HEIGHT / GRID_SIZE;
//     foodX = (rand() % columns) * GRID_SIZE;
//     foodY = (rand() % rows) * GRID_SIZE;
// }

void ResetGame(vector<Point>& snake, int& velocityX, int& velocityY, int& score) {
    snake.clear();
    int startX = SCREEN_WIDTH / 2;
    int startY = SCREEN_HEIGHT / 2;
    
    snake.push_back({startX, startY});           
    snake.push_back({startX - GRID_SIZE, startY});      
    snake.push_back({startX - GRID_SIZE * 2, startY});      

    velocityX = GRID_SIZE;
    velocityY = 0;
    score = 0;
    SpawnallFood();
    currentState = PLAYING; 
}

void RenderTextHelper(SDL_Renderer* renderer, TTF_Font* font, const string& text, int x, int y, SDL_Color color, bool center) {
    if (!font) return;
    
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) return;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = { x, y, surface->w, surface->h };
    
    if (center) {
        rect.x = x - surface->w / 2;
        rect.y = y - surface->h / 2;
    }
    
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int main(int argc, char* argv[]) {
    srand(time(0));

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load two different sizes of the font
    TTF_Font* mainFont = TTF_OpenFont("arial.ttf", 26);  
    TTF_Font* titleFont = TTF_OpenFont("arial.ttf", 52); 
    
    // Fallback system paths if arial.ttf is missing from your working folder
    if (!mainFont)  mainFont  = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 26);
    if (!titleFont) titleFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 52);

    int velocityX, velocityY, score;
    vector<Point> snake;

    ResetGame(snake, velocityX, velocityY, score);

    bool isRunning = true;
    SDL_Event event;

    Uint32 lastMoveTime = SDL_GetTicks();
    const Uint32 moveDelay = 70; 

    while (isRunning) {
        // 1. INPUT PROCESSING
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
            else if (event.type == SDL_KEYDOWN) {
                if (currentState == PLAYING) {
                    switch (event.key.keysym.sym) {
                        case SDLK_UP:    if (velocityY == 0) { velocityX = 0; velocityY = -GRID_SIZE; } break;
                        case SDLK_DOWN:  if (velocityY == 0) { velocityX = 0; velocityY = GRID_SIZE;  } break;
                        case SDLK_LEFT:  if (velocityX == 0) { velocityX = -GRID_SIZE; velocityY = 0; } break;
                        case SDLK_RIGHT: if (velocityX == 0) { velocityX = GRID_SIZE;  velocityY = 0; } break;
                    }
                } 
                else if (currentState == GAME_OVER) {
                    if (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN) {
                        ResetGame(snake, velocityX, velocityY, score);
                    }
                }
            }
        }

        // 2. POSITION CALCULATIONS
        if (currentState == PLAYING) {
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - lastMoveTime > moveDelay) {

                Point oldTail = snake.back();

                for (size_t i = snake.size() - 1; i > 0; i--) {
                    snake[i] = snake[i - 1];
                }

                snake[0].x += velocityX;
                snake[0].y += velocityY;

                // Screen Wrap-around
                if (snake[0].x < 0){
                    snake[0].x = SCREEN_WIDTH - GRID_SIZE;
                }else if(snake[0].x >= SCREEN_WIDTH){
                     snake[0].x = 0;
                } 
                if (snake[0].y < 0){
                    snake[0].y = SCREEN_HEIGHT - GRID_SIZE;
                }else if (snake[0].y >= SCREEN_HEIGHT){
                    snake[0].y = 0;
                }    

                // BORDER COLLISION CHECK
                for(size_t i =1; i < snake.size(); i++){
                    if (snake[0].x < 0 || snake[0].x >= SCREEN_WIDTH || snake[0].y < 0 || snake[0].y >= SCREEN_HEIGHT) {
                        currentState = GAME_OVER;
                        break;
                    }
                }

                // Self-Collision Check
                for (size_t i = 1; i < snake.size(); i++) {
                    if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
                        currentState = GAME_OVER; 
                        break;
                    }
                }

                // // Food Collision
                // if (snake[0].x == foodX && snake[0].y == foodY) {
                //     snake.push_back(oldTail);
                //     SpawnFood();
                //     score += 1; 
                // }

                //FOOD COLLISION FOR MULTIPLE FOODS
                for(size_t i=0; i<foods.size(); i++){
                    if(snake[0].x == foods[i].x && snake[0].y == foods[i].y){
                        snake.push_back(oldTail);
                        //SpawnallFood();
                        score += 1;

                        int columns = SCREEN_WIDTH / GRID_SIZE;
                        int rows = SCREEN_HEIGHT / GRID_SIZE;

                        foods[i].x = (rand() % columns) * GRID_SIZE;
                        foods[i].y = (rand() % rows) * GRID_SIZE;
                    }
                }

                lastMoveTime = currentTime;
            }
        }

        // 3. GRAPHICS RENDERING
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderClear(renderer);

        if (currentState == PLAYING) {
            // Draw Food
            // SDL_Rect foodRect = { foodX, foodY, GRID_SIZE, GRID_SIZE };
            // SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); 
            // SDL_RenderFillRect(renderer, &foodRect);
            // Draw All Active Foods
            SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); // Yellow/Gold
            for (const auto& item : foods) {
                SDL_Rect foodRect = { item.x, item.y, GRID_SIZE, GRID_SIZE };
                SDL_RenderFillRect(renderer, &foodRect);
            }

            // Draw Snake
            for (size_t i = 0; i < snake.size(); i++) {
                if (i == 0) {
                    SDL_Rect headRect = { snake[i].x, snake[i].y, GRID_SIZE, GRID_SIZE };
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White Head
                    SDL_RenderFillRect(renderer, &headRect);
                } else {
                    int offset = (GRID_SIZE - BODY_SIZE) / 2;
                    SDL_Rect bodyRect = { snake[i].x + offset, snake[i].y + offset, BODY_SIZE, BODY_SIZE };
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green Body
                    SDL_RenderFillRect(renderer, &bodyRect);
                }
            }

            // Draw HUD Score (Uses regular mainFont)
            RenderTextHelper(renderer, mainFont, "Score: " + to_string(score), 15, 15, {255, 255, 255, 255}, false);
        } 
        else if (currentState == GAME_OVER) {
            SDL_Color red   = { 255, 50, 50, 255 };
            SDL_Color white = { 255, 255, 255, 255 };

            // 1. GAME OVER Title -> Uses titleFont (Size 52)
            RenderTextHelper(renderer, titleFont, "GAME OVER", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 80, red, true);
            
            // 2. Score Summary -> Uses mainFont (Size 26)
            RenderTextHelper(renderer, mainFont, "Your Score: " + to_string(score), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, white, true);
            
            // 3. Restart Prompts -> Uses mainFont (Size 26)
            RenderTextHelper(renderer, mainFont, "Press SPACE or ENTER to Restart", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 80, white, true);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(10); 
    }

    if (mainFont)  TTF_CloseFont(mainFont);
    if (titleFont) TTF_CloseFont(titleFont);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}