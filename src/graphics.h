// Copyright Eric Wolfson 2016-2017
// See LICENSE.txt (GPLv3)

#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "globals.h"
#include "point.h"

static const std::string texture_file_names[NUM_TOTAL_TEXTURES] =
{
    "backdroptex1.png",
    "endblocktex.png",
    "wallblocktex.png",
    "guntex1.png",
    "herotex.png",
    "guardtex.png",
    "gibtex1.png",
    "gibtex2.png",
    "gladiatortex.png",
    "guntex2.png",
    "guntex3.png",
    "hazmattex.png",
    "guntex4.png",
    "flametex.png",
    "endblocktex2.png",
    "wallblocktex2.png",
    "laddertex.png",
    "bigguardtex.png",
    "executionertex.png",
    "soldiertex.png",
    "healthtex.png",
    "switchtex.png",
    "doortex.png",
    "thresholdtex.png",
    "door2tex.png",
    "threshold2tex.png",
    "wallblocktex3.png",
    "goldnuggettex.png",
    "accesscard1tex.png",
    "lockeddoor2tex.png",
    "exittex.png",
    "cannonballtex.png",
    "guntex6.png",
    "guntex5.png",
    "explosiontex.png",
    "agenttex.png",
    "championtex.png",
    "healthtex2.png",
    "healthtex3.png",
    "goblettex.png",
    "grandchampiontex.png",
    "rocketlaunchertex.png",
    "rockettex.png",
    "hazmatgodtex.png",
    "spawntex.png",
    "kingtex.png",
    "soldiertex.png",
    "laserguntex.png",
    "lasertex.png",
    "healthtex4.png",
    "permhealthtex.png",
    "permhealthtex2.png",
    "permhealthtex3.png",
    "permhealthtex4.png",
    "exppoweruptex.png",
    "exppoweruptex2.png",
    "exppoweruptex3.png",
    "advancedagenttex.png",
    "shadowtex.png",
    "menubackdrop.png",
    "laddertex2.png",
    "brickbackdrop1a.png",
    "brickbackdrop1b.png",
    "brickbackdrop1c.png",
    "brickbackdrop1d.png",
    "brickbackdrop1e.png",
    "brickbackdrop1f.png",
    "brickbackdrop1g.png",
    "brickbackdrop1h.png",
    "brickbackdrop1i.png",
    "brickbackdrop1j.png",
    "brickbackdrop1k.png",
    "brickbackdrop1l.png",
    "gibtex3.png",
    "gibtex4.png",
    "gibtex5.png",
    "gibtex6.png",
    "gibtex7.png",
    "guardtex.png",
    "humantex.png",
    "eliteguardtex.png"
};

class gfx_engine
{
    public:
        gfx_engine();
        ~gfx_engine();
        SDL_Texture* getTexture(int);
        bool initSDL();
        void freeSDL();
        void renderSprite(SDL_Texture *, point, point, point, int, int, SDL_RendererFlip, bool, SDL_Color, int);
        void drawRectangle(SDL_Color,point,point);
        void addBitmapString(SDL_Color, std::string, point);
        void addBitmapCharacter(SDL_Color, int, point);
        void clearScreen();
        void updateScreen();
        void updateCamera(point,point);
        point getCamera();
    private:
        SDL_Window* screen;
        SDL_Renderer* renderer;
        SDL_Texture* textures[NUM_TOTAL_TEXTURES];
        SDL_Texture* font_texture;
        point camera;
};

#endif
