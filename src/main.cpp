#include <iostream>
#include "raylib.h"

// define variables 
const int screen_width = 600;
const int screen_height = 800;

double deltatime = 0.0;

double g = 400.0;

double impulse = 300; // This is the variable that makes the bird to boom or gives it a boost while in air 

int const number_of_pillar = 1; // I have created a strct array for pillars becuase in future i have plans to increase number of pillars

double starttime; // Creates a start time variable that is used for delay

double pillar_speed = 0.1;

bool is_collided = false;

int score = 0;

struct character{
    int height;
    int width;
    double velocity;
    Vector2 position;
};
character bird;

struct enemy
{
    int height;
    int width;
    int gap;
    bool state;
    Vector2 position;
};
enemy pillar[number_of_pillar];
enemy pillar_rotated[number_of_pillar]; // Pillar roatated has a seperate assest and a seperate struct 

// AABB collsion logic for player to pillar 
bool collision(enemy &pillar, enemy &pillar_rotated, character &bird)
{
    bool bottom_pillar = bird.position.x + bird.width > pillar.position.x &&
        pillar.position.x + pillar.width > bird.position.x &&
        bird.position.y + bird.height > pillar.position.y &&
        pillar.position.y + pillar.height > bird.position.y;

    bool top_pillar = bird.position.x + bird.width > pillar_rotated.position.x &&
        pillar_rotated.position.x + pillar_rotated.width > bird.position.x &&
        bird.position.y + bird.height > pillar_rotated.position.y &&
        pillar_rotated.position.y + pillar_rotated.height > bird.position.y;

    if(top_pillar || bottom_pillar){ return true; }
    else{ return false; }
}

// A simple gravity based movment using the first equation of motion 
void movement(character &bird, double &gravity, double &deltatime)
{
    if(IsKeyPressed(KEY_SPACE)){ bird.velocity = -impulse; }
    bird.velocity += g * deltatime;
    bird.position.y += bird.velocity * deltatime;
}

void boundaries(character &bird)
{
    if(bird.position.y >= screen_height - bird.height){ bird.position.y = screen_height - bird.height; }
    if(bird.position.y < 0){ bird.position.y = 0; }
}

int main()
{
    InitWindow(screen_width, screen_height, "Birdie");

    //loads in textures
    Texture2D background_image = LoadTexture("assets/background.png");
    Texture2D bird_texture = LoadTexture("assets/bird1.png");
    Texture2D pillar_image = LoadTexture("assets/pillar.png");
    Texture2D pillar_rotated_image = LoadTexture("assets/pillar_rotated.png");
    
    // assign values to bird struct 
    bird.position.x = bird_texture.width;
    bird.position.y = 64; // 64 was the most appropriate value to put there 
    bird.height = bird_texture.height;
    bird.width = bird_texture.width;
    bird.velocity = 0.0;

    starttime = GetTime();

    // assig pillars(including rotated ones) struct value 
    for(int i = 0; i < number_of_pillar; i++)
    {
        pillar[i].height = pillar_image.height;
        pillar[i].width = pillar_image.width;
        pillar[i].position.x = GetRandomValue(400, 600 - pillar[i].width);
        pillar[i].position.y = pillar[i].height - GetRandomValue(128, 256);
        pillar[i].state = false;

        pillar_rotated[i].height = pillar_image.height;
        pillar_rotated[i].width = pillar_image.width;
        pillar_rotated[i].position.x = pillar[i].position.x;
        pillar_rotated[i].position.y = GetRandomValue(128, 256) - pillar_rotated[i].height;
        pillar_rotated[i].state = false;
    }

    while(!WindowShouldClose())
    {
        deltatime = GetFrameTime();
        BeginDrawing();

        //A small delay before putting up pillar 
        if(!is_collided)
        {
            if(GetTime() - starttime >= 2)
            {
                for(int i = 0; i < number_of_pillar; i++)
                {
                    pillar[i].state = true;
                }
            }
            // Calling function and drawing the textures 
            ClearBackground(BLACK);
            DrawTexture(background_image, 0, 0, WHITE);
            movement(bird, g, deltatime);
            boundaries(bird);
            DrawTexture(bird_texture, bird.position.x, bird.position.y, WHITE);
            DrawText(TextFormat("Score: %d", score),0,0,50,WHITE);

            // Drawing , reseting , moving , reassiging after goes off screen is done in the for loop below 
            for(int i = 0; i < number_of_pillar; i++)
            {
                if(pillar[i].state)
                {
                    DrawTexture(pillar_image, pillar[i].position.x, pillar[i].position.y, WHITE);
                    DrawTexture(pillar_rotated_image, pillar_rotated[i].position.x, pillar_rotated[i].position.y, WHITE);
                    if(GetTime() - starttime >= 2.5) // Starts moving the pillar with 2.5 sec delay 
                    {
                        pillar[i].position.x -= pillar_speed;
                        pillar_rotated[i].position.x -= pillar_speed;
                    }
                }
                // in the below lines there are some magic number they are there becuase they were the most appropriate i found during testing 
                if(pillar[i].position.x - 10 < 0 && pillar_rotated[i].position.x - pillar_rotated[i].width < 0)
                {
                    pillar[i].state = false;
                    pillar_rotated[i].state = false;

                    pillar[i].width = pillar_image.width;
                    pillar[i].position.x = GetRandomValue(400, 600) - pillar[i].width;

                    pillar_rotated[i].position.x = pillar[i].position.x;
                    pillar_rotated[i].position.y = GetRandomValue(128, 256) - pillar_rotated[i].height;

                    score ++;
                }
            }

            // Used to close game loop and enter menu
            for(int i = 0; i < number_of_pillar; i++)
            {
                is_collided = collision(pillar[i], pillar_rotated[i], bird);
            }
        }

        // Draws screen after death
        if(is_collided)
        {
            ClearBackground(BLUE);
            DrawText("Game Over", 0, 0, 50, WHITE);
            DrawText(TextFormat("Score: %d", score), 0, 50, 50, WHITE);
            DrawText("Press R to restart", 0, 100, 50, WHITE);
            // reset condition
            if(IsKeyDown(KEY_R))
            {
                is_collided = false;

                score = 0;

                // assign values to bird struct
                bird.position.x = bird_texture.width;
                bird.position.y = 64; // 64 was the most appropriate value to put there
                bird.height = bird_texture.height;
                bird.width = bird_texture.width;
                bird.velocity = 0.0;
                starttime = GetTime();

                // assign pillars(including rotated ones) struct value
                for(int i = 0; i < number_of_pillar; i++)
                {
                    pillar[i].height = pillar_image.height;
                    pillar[i].width = pillar_image.width;
                    pillar[i].position.x = GetRandomValue(400, 600 - pillar[i].width);
                    pillar[i].position.y = pillar[i].height - GetRandomValue(128, 256);
                    pillar[i].state = false;

                    pillar_rotated[i].height = pillar_image.height;
                    pillar_rotated[i].width = pillar_image.width;
                    pillar_rotated[i].position.x = pillar[i].position.x;
                    pillar_rotated[i].position.y = GetRandomValue(128, 256) - pillar_rotated[i].height;
                    pillar_rotated[i].state = false;
                }
            }
        }

        EndDrawing();
    }

    // Unloads all loaded textures
    UnloadTexture(pillar_image);
    UnloadTexture(pillar_rotated_image);
    UnloadTexture(bird_texture);
    UnloadTexture(background_image);

    CloseWindow();
}