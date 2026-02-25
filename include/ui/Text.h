#ifndef UI_TEXT_H
#define UI_TEXT_H
#include <SDL2/SDL.h>
#include <string>
struct _TTF_Font;
typedef _TTF_Font TTF_Font;

struct TextSystem {
    TTF_Font* font=nullptr;
    bool ready=false;
};

bool initText(TextSystem& ts,const char* fontPath, int fontSize);
void destroyText(TextSystem& ts);

void drawText(SDL_Renderer* ren, TextSystem& ts,int x, int y, const std::string& text);
void drawTextColored(SDL_Renderer* ren, TextSystem& ts,int x, int y, const std::string& text, SDL_Color color);

#endif