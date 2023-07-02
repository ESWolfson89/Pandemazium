// Copyright Eric Wolfson 2016-2017
// See LICENSE.txt (GPLv3)

#include "graphics.h"

/*
 * Set default values for SDL2 variables
 */
gfx_engine::gfx_engine()
{
    camera = point(0.0,0.0);
    screen = NULL;
    renderer = NULL;
    font_texture = NULL;
    for (int i = 0; i < NUM_TOTAL_TEXTURES; ++i)
    {
        textures[i] = NULL;
    }
}

/*
 * Free graphics memory 
 */
gfx_engine::~gfx_engine()
{
    std::cout << "IN GFX DESTR\n";
    freeSDL();
}

/*
 * Initialize SDL2
 */
bool gfx_engine::initSDL()
{
    // initialize all of SDL2's utilities/mechanisms etc...
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
        return false;

    // initialize SDL_Window instance screen
    screen = SDL_CreateWindow("Pandemazium", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              (int)WINDOW_WIDTH, (int)WINDOW_HEIGHT, SDL_WINDOW_SHOWN );

    if(screen == NULL)
        return false;

    // initialize the renderer
    renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);

    if(renderer == NULL)
        return false;

    font_texture = IMG_LoadTexture(renderer,".\\Debug\\textures\\mainfont16x16.png");

    if (font_texture == NULL)
    {
        std::cout << "Failed to load font file\n";
        return false;
    }
    // initialize all entity textures
    for (int i = 0; i < NUM_TOTAL_TEXTURES; ++i)
    {
        textures[i] = IMG_LoadTexture(renderer,&(".\\Debug\\textures\\" + texture_file_names[i])[0]);
        if (textures[i] == NULL)
        {
            std::cout << "Failed to load " + texture_file_names[i][0] << "\n";
            return false;
        }
    }
    return true;
}


/*
 * free/destroy graphics ptr objects
 */
void gfx_engine::freeSDL()
{
    for (int i = 0; i < NUM_TOTAL_TEXTURES; ++i)
    {
        if (textures[i] != NULL)
        {
            SDL_DestroyTexture(textures[i]);
            textures[i] = NULL;
        }
    }

    if (font_texture != NULL)
    {
        SDL_DestroyTexture(font_texture);
        font_texture = NULL;
    }

    if (renderer != NULL)
    {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }

    if (screen != NULL)
    {
        SDL_DestroyWindow(screen);
        screen = NULL;
    }

    SDL_Quit();
}

/*
 * Render given subarea (row, frame, textureArea) of texture object in memory and place it at a specific
 * location (loc) on the window. Place it on the screen taking up a specific area on the window (possible stretched)
 */
void gfx_engine::renderSprite(SDL_Texture *texture, point loc, point textureArea, point area, int frame, int row, SDL_RendererFlip orientation, bool parallax, SDL_Color col_multval, int scale)
{
    SDL_Rect rect;
    SDL_Rect crop;

    crop.x = (int)textureArea.x() * frame;
    crop.y = (int)textureArea.y() * row;
    crop.w = (int)textureArea.x();
    crop.h = (int)textureArea.y();

    rect.x = (int)loc.x() * 2;
    rect.y = (int)loc.y() * 2;
    rect.w = (int)area.x() * scale;
    rect.h = (int)area.y() * scale;

    if (parallax == false)
    {
        rect.x -= camera.x();
        rect.y -= camera.y();
    }

    // don't render anything not on screen (optimize)
    // if (rect.x + rect.w < 0 || rect.y + rect.h < 0 ||
    //    (double)rect.x > MAP_WIDTH - ((double)WINDOW_WIDTH/scale) || (double)rect.y > MAP_HEIGHT - ((double)WINDOW_HEIGHT/scale))
    //     return;
    if (col_multval.r != 0 || col_multval.g != 0 || col_multval.b != 0)
        SDL_SetTextureColorMod(texture,col_multval.r, col_multval.g, col_multval.b);

    SDL_RenderCopyEx(renderer,texture,&crop,&rect,0.0,NULL,orientation);
    SDL_SetTextureColorMod(texture,255,255,255);
}

/*
 * print a string (sval) to the screen one bitmap character at a time
 * at location (x,y) with color col.
 */
void gfx_engine::addBitmapString(SDL_Color col, std::string string_val, point loc)
{
    int ascii_val = 0;
    for (int i = 0; i < (int)string_val.size(); ++i)
    {
        ascii_val = (int)string_val[i];
        if (ascii_val >= 0 && ascii_val <= 255)
            addBitmapCharacter(col, ascii_val, point(loc.x()+(i*FONT_CHAR_WIDTH),loc.y()));
    }
}

/*
 * render portion of font_bitmap to screen with color col
 */
void gfx_engine::addBitmapCharacter(SDL_Color col, int ascii_val, point loc)
{
    SDL_Rect rect;
    SDL_Rect crop;

    // crop out bitmap tile given ascii_val, TILEWID, and TILEHGT
    crop.x = (ascii_val % FONT_COLUMNS) * FONT_CHAR_WIDTH;
    crop.y = (int)(ascii_val / FONT_ROWS) * FONT_CHAR_HEIGHT;
    crop.w = FONT_CHAR_WIDTH;
    crop.h = FONT_CHAR_HEIGHT;

    rect.x = (int)loc.x();
    rect.y = (int)loc.y();
    rect.w = FONT_CHAR_WIDTH;
    rect.h = FONT_CHAR_HEIGHT;

    //Render foreground character to screen (via parsing the bitmap font)

    SDL_SetTextureColorMod(font_texture,col.r,col.g,col.b);

    SDL_RenderCopy(renderer,font_texture,&crop,&rect);
}

void gfx_engine::drawRectangle(SDL_Color c, point loc, point area)
{
    SDL_Rect rect = {(int)loc.x()*2 - (int)camera.x(),(int)loc.y()*2 - (int)camera.y(),(int)area.x()*2,(int)area.y()*2};
    SDL_SetRenderDrawColor(renderer,c.r,c.g,c.b,0);
    SDL_RenderFillRect(renderer,&rect);
}

/*
 * Clear sceen to black
 */
void gfx_engine::clearScreen()
{
    SDL_RenderClear(renderer);
}

/*
 * move camera determining what portion of the level (play area) should be rendered
 */
void gfx_engine::updateCamera(point center, point current_level_size)
{
    camera = addPoints(center,point(-1.0*(double)WINDOW_WIDTH/(4.0),-1.0*(double)WINDOW_HEIGHT/(4.0)));

    if(camera.x() < 0.0)
       camera.setx(0.0);
    if(camera.y() < 0.0)
       camera.sety(0.0);
    if(camera.x() > MAP_WIDTH - ((double)WINDOW_WIDTH/2.0))
       camera.setx(MAP_WIDTH - ((double)WINDOW_WIDTH/2.0));
    if(camera.y() > MAP_HEIGHT - ((double)WINDOW_HEIGHT/2.0))
       camera.sety(MAP_HEIGHT - ((double)WINDOW_HEIGHT/2.0));

    camera = multPoints(camera, point(2.0,2.0));
}

/*
 * Return point representing location of play area to be displayed
 */
point gfx_engine::getCamera()
{
    return camera;
}

/*
 * Update display
 */
void gfx_engine::updateScreen()
{
    SDL_RenderPresent(renderer);
}

/*
 * Get texture object from list of texture files
 */
SDL_Texture* gfx_engine::getTexture(int i)
{
    return textures[i];
}
