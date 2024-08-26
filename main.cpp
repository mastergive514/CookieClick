#include <raylib.h>
#include <math.h>
#include <vector>
#include <string>
#include <iostream>

// Structure to hold click effect information
struct ClickEffect {
    int x, y;
    int timer;
};

// Initialize variables
int score = 0;
bool cookieClicked = false;
bool shopOpen = false;
bool confirmClose = false;
std::vector<ClickEffect> clickEffects;
int clickMultiplier = 1; // Default click multiplier

// Function to play background music (using raylib's built-in functionality)
void PlayBackgroundMusic(Music& music) {
    PlayMusicStream(music);
}

// Function to play click sound effect (using raylib's built-in functionality)
void PlayClickSound(Sound& clickSound) {
    PlaySound(clickSound);
}

// Function to set click multiplier
void SetMultiplier(int multiplier) {
    clickMultiplier = multiplier;
}

int main() {
    InitWindow(800, 600, "Cookie Clicker");

    // Load textures
    Texture2D cookieTexture = LoadTexture("cookie.png");
    Texture2D smallCookieTexture = LoadTexture("small_cookie.png");

    // Resize textures
    int targetWidth = 100;
    int targetHeight = 100;
    Image cookieImage = LoadImage("cookie.png");
    ImageResize(&cookieImage, targetWidth, targetHeight);
    cookieTexture = LoadTextureFromImage(cookieImage);
    UnloadImage(cookieImage);

    Image smallCookieImage = LoadImage("small_cookie.png");
    int smallCookieWidth = 30;
    int smallCookieHeight = 30;
    ImageResize(&smallCookieImage, smallCookieWidth, smallCookieHeight);
    smallCookieTexture = LoadTextureFromImage(smallCookieImage);
    UnloadImage(smallCookieImage);

    // Shop parameters
    struct ShopItem {
        std::string name;
        int cost;
        void (*effect)();
    };
    std::vector<ShopItem> items = {
        {"AUTOCLICK X2", 10, []() { SetMultiplier(2); }},
        {"AUTOCLICK X4", 50, []() { SetMultiplier(4); }},
        {"AUTOCLICK X8", 100, []() { SetMultiplier(8); }},
        {"AUTOCLICK X20", 300, []() { SetMultiplier(20); }},
        // Add more items here
    };

    ShopItem* selectedItem = nullptr;

    int shopButtonX = 650;
    int shopButtonY = 10;
    int shopButtonWidth = 120;
    int shopButtonHeight = 40;

    // Load sounds
    Sound clickSound = LoadSound("click_sound.wav");
    Music music = LoadMusicStream("background_music.wav");
    PlayBackgroundMusic(music);

    // Initialize animation variables
    int moveSpeed = 10;
    int animationTimer = 0;
    int maxAnimationTime = 20;
    bool animationActive = false;
    float scaleFactor = 1.0f;
    float scaleSpeed = 0.1f;
    float maxScale = 1.2f;

    int cookieWidth = cookieTexture.width;
    int cookieHeight = cookieTexture.height;
    int cookieX = (GetScreenWidth() - cookieWidth) / 2;
    int cookieY = (GetScreenHeight() - cookieHeight) / 2;

    while (!WindowShouldClose()) {
        Vector2 mousePosition = GetMousePosition();
        int mouseX = mousePosition.x;
        int mouseY = mousePosition.y;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (shopOpen) {
                for (size_t i = 0; i < items.size(); ++i) {
                    int itemX = 30;
                    int itemY = 100 + i * 60;
                    int itemWidth = 240;
                    int itemHeight = 50;
                    if ((itemX <= mouseX && mouseX <= itemX + itemWidth) && (itemY <= mouseY && mouseY <= itemY + itemHeight)) {
                        if (score >= items[i].cost) {
                            score -= items[i].cost;
                            selectedItem = &items[i];
                            selectedItem->effect(); // Apply item effects
                        }
                    }
                }

                if (confirmClose) {
                    int yesButtonX = 340;
                    int yesButtonY = 300;
                    int noButtonX = 420;
                    int noButtonY = 300;
                    if ((yesButtonX <= mouseX && mouseX <= yesButtonX + 80) && (yesButtonY <= mouseY && mouseY <= yesButtonY + 40)) {
                        shopOpen = false;
                        confirmClose = false;
                    }
                    else if ((noButtonX <= mouseX && mouseX <= noButtonX + 80) && (noButtonY <= mouseY && mouseY <= noButtonY + 40)) {
                        confirmClose = false;
                    }
                }
            } else {
                if ((shopButtonX <= mouseX && mouseX <= shopButtonX + shopButtonWidth) && (shopButtonY <= mouseY && mouseY <= shopButtonY + shopButtonHeight)) {
                    shopOpen = !shopOpen;
                }
            }

            if ((cookieX <= mouseX && mouseX <= cookieX + cookieWidth) && (cookieY <= mouseY && mouseY <= cookieY + cookieHeight)) {
                score += clickMultiplier;
                animationActive = true;
                cookieClicked = true;
                PlayClickSound(clickSound);
                for (int i = 0; i < 5; ++i) {
                    clickEffects.push_back({ mouseX, mouseY, 0 });
                }
            }
        }

        if (animationActive) {
            animationTimer++;
            if (animationTimer > maxAnimationTime) {
                animationTimer = 0;
                animationActive = false;
            }
            float verticalOffset = moveSpeed * sin(animationTimer * (PI / maxAnimationTime));
        } else {
            float verticalOffset = 0;
        }

        if (cookieClicked) {
            if (scaleFactor < maxScale) {
                scaleFactor += scaleSpeed;
            } else {
                cookieClicked = false;
            }
            if (scaleFactor > maxScale) {
                scaleFactor = maxScale;
            }
        } else {
            if (scaleFactor > 1.0f) {
                scaleFactor -= scaleSpeed;
            }
        }

        for (auto it = clickEffects.begin(); it != clickEffects.end();) {
            it->timer++;
            if (it->timer > 20) {
                it = clickEffects.erase(it);
            } else {
                it->y--;
                ++it;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTextureEx(cookieTexture, { (float)cookieX, (float)(cookieY + verticalOffset) }, 0.0f, scaleFactor, WHITE);

        for (const auto& effect : clickEffects) {
            DrawTexture(smallCookieTexture, effect.x, effect.y, WHITE);
        }

        DrawText(TextFormat("Score: %d", score), 10, 10, 40, DARKGRAY);

        if (shopOpen) {
            DrawRectangle(30, 50, 240, 400, GRAY);
            DrawText("Shop", 50, 60, 30, BLACK);
            for (size_t i = 0; i < items.size(); ++i) {
                int itemX = 30;
                int itemY = 100 + i * 60;
                DrawRectangle(itemX, itemY, 240, 50, LIGHTGRAY);
                DrawText(TextFormat("%s - %d cookies", items[i].name.c_str(), items[i].cost), itemX + 10, itemY + 10, 20, BLACK);
            }
            DrawRectangle(700, 50, 80, 40, LIGHTGRAY);
            DrawText("Close", 710, 60, 20, BLACK);

            if (confirmClose) {
                DrawRectangle(300, 250, 200, 100, GRAY);
                DrawText("Are you sure?", 340, 270, 20, BLACK);
                DrawRectangle(340, 320, 80, 40, LIGHTGRAY);
                DrawText("Yes", 355, 330, 20, BLACK);
                DrawRectangle(420, 320, 80, 40, LIGHTGRAY);
                DrawText("No", 435, 330, 20, BLACK);
            }
        } else {
            DrawRectangle(shopButtonX, shopButtonY, shopButtonWidth, shopButtonHeight, LIGHTGRAY);
            DrawText("SHOP", shopButtonX + 20, shopButtonY + 10, 20, BLACK);
        }

        EndDrawing();
    }

    UnloadTexture(cookieTexture);
    UnloadTexture(smallCookieTexture);
    UnloadSound(clickSound);
    UnloadMusicStream(music);
    CloseWindow();

    return 0;
}
