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

Point specialFood;
bool isSpecialFoodActive = false;
int regularFoodCounter1 = 0;

// Point poisonFood;
// bool ispoisonFoodActivet = false;
// int regularFoodCounter2 = 0;

// void SpawnFood() {
//     int columns = SCREEN_WIDTH / GRID_SIZE;
//     int rows = SCREEN_HEIGHT / GRID_SIZE;
//     foodX = (rand() % columns) * GRID_SIZE;
//     foodY = (rand() % rows) * GRID_SIZE;
// }

void ResetGame(vector<Point>& snake, int& velocityX, int& velocityY, int& score, Uint32 moveDelay) {
    snake.clear();
    int startX = SCREEN_WIDTH / 2;
    int startY = SCREEN_HEIGHT / 2;

    int initialLength = 3;
    for (int i = 0; i < initialLength; i++) {
        // Each segment is placed one GRID_SIZE backward on the X-axis
        snake.push_back({startX - (i * GRID_SIZE), startY});
    }
    
    // snake.push_back({startX, startY});           
    // snake.push_back({startX - GRID_SIZE, startY});      
    // snake.push_back({startX - GRID_SIZE * 2, startY});      

    velocityX = GRID_SIZE;
    velocityY = 0;
    score = 0;
    SpawnallFood();
    moveDelay = 200;
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

    
    bool isRunning = true;
    SDL_Event event;
    
    Uint32 lastMoveTime = SDL_GetTicks();
    Uint32 initialmoveDelay = 200; 
    Uint32 moveDelay = initialmoveDelay;
    
    ResetGame(snake, velocityX, velocityY, score, moveDelay);

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
                        ResetGame(snake, velocityX, velocityY, score, moveDelay);
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

                // // BORDER COLLISION CHECK
                // for(size_t i =1; i < snake.size(); i++){
                //     if (snake[0].x < 0 || snake[0].x >= SCREEN_WIDTH || snake[0].y < 0 || snake[0].y >= SCREEN_HEIGHT) {
                //         currentState = GAME_OVER;
                //         break;
                //     }
                // }

                
                if (snake[0].x < 0){
                    snake[0].x = SCREEN_WIDTH - GRID_SIZE;
                    score--;
                    snake.pop_back();
                }else if(snake[0].x >= SCREEN_WIDTH){
                     snake[0].x = 0;
                     score--;
                     snake.pop_back();
                } 
                if (snake[0].y < 0){
                    snake[0].y = SCREEN_HEIGHT - GRID_SIZE;
                    score--;
                    snake.pop_back();
                }else if (snake[0].y >= SCREEN_HEIGHT){
                    snake[0].y = 0;
                    score--;
                    snake.pop_back();
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

                // --- FOOD COLLISION FOR MULTIPLE FOODS ---
                for(size_t i = 0; i < foods.size(); i++){
                    if(snake[0].x == foods[i].x && snake[0].y == foods[i].y){
                        snake.push_back(oldTail);
                        // snake.pop_back();
                        score += 1;
                        regularFoodCounter1 += 1; // Count regular foods eaten
                        // regularFoodCounter2 += 1;

                        int speedMilestons = score/1;
                        moveDelay = initialmoveDelay - (speedMilestons * 5);

                        if(moveDelay < 25){
                            moveDelay = 20;
                        }

                        // If 5 regular foods are eaten, spawn the special green food
                        if (regularFoodCounter1 >= 5 && !isSpecialFoodActive) {
                            int columns = SCREEN_WIDTH / GRID_SIZE;
                            int rows = SCREEN_HEIGHT / GRID_SIZE;
                            specialFood.x = (rand() % columns) * GRID_SIZE;
                            specialFood.y = (rand() % rows) * GRID_SIZE;
                            isSpecialFoodActive = true;
                            regularFoodCounter1 = 0; // Reset counter
                        }

                        // //for poison food
                        // if(regularFoodCounter2 >=7 && !ispoisonFoodActivet){
                        //     int columns = SCREEN_WIDTH / GRID_SIZE;
                        //     int rows = SCREEN_HEIGHT / GRID_SIZE;
                        //     poisonFood.x = (rand() % columns) * GRID_SIZE;
                        //     poisonFood.y = (rand() % rows) * GRID_SIZE;
                        //     ispoisonFoodActivet = true;
                        //     regularFoodCounter2 = 0; // Reset counter
                        // }

                        // Respawn the eaten regular food
                        int columns = SCREEN_WIDTH / GRID_SIZE;
                        int rows = SCREEN_HEIGHT / GRID_SIZE;
                        foods[i].x = (rand() % columns) * GRID_SIZE;
                        foods[i].y = (rand() % rows) * GRID_SIZE;
                        break; 
                    }else if(score < 0){
                        score = 0; 
                        currentState = GAME_OVER; 
                        break;
                    }
                }

                // --- SPECIAL GREEN FOOD COLLISION ---
                if (isSpecialFoodActive && snake[0].x == specialFood.x && snake[0].y == specialFood.y) {
                    snake.push_back(oldTail);
                    snake.push_back(oldTail);
                    snake.push_back(oldTail);
                    snake.push_back(oldTail);
                    snake.push_back(oldTail);
                    score += 5;                 // Big score boost!
                    isSpecialFoodActive = false; // Remove it from the board
                }

                //  // --- POISON FOOD RED COLLISION ---
                // if (ispoisonFoodActivet && snake[0].x == poisonFood.x && snake[0].y == poisonFood.y) {
                //     snake.push_back(oldTail);
                //     score -= 5;                
                //     ispoisonFoodActivet = false; 
                // }


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
            // --- DRAW SPECIAL GREEN FOOD ---
            if (isSpecialFoodActive) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Pure Green
                SDL_Rect specialRect = { specialFood.x, specialFood.y, GRID_SIZE, GRID_SIZE };
                SDL_RenderFillRect(renderer, &specialRect);
            }
            // // --- DRAW POISON RED FOOD ---
            // if (ispoisonFoodActivet) {
            //     SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Pure Green
            //     SDL_Rect poisonRect = { poisonFood.x, poisonFood.y, GRID_SIZE, GRID_SIZE };
            //     SDL_RenderFillRect(renderer, &poisonRect);
            // }

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