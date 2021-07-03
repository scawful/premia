// Texture Class Header
// @scawful

#ifndef Texture_hpp
#define Texture_hpp

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <iostream>

//Texture wrapper class
class PTexture
{
    public:
        // Initializes variables
        PTexture();

        // Deallocates memory
        ~PTexture();

        // Legacy texture loader from path
        SDL_Texture *loadTexture( SDL_Renderer *pRenderer, std::string path );

        // Loads image at specified path
        bool loadFromFile( SDL_Renderer *pRenderer, std::string path );
        
        // Creates image from font string
        bool loadFromRenderedText( SDL_Renderer *pRenderer, TTF_Font *gFont, std::string textureText, SDL_Color textColor );

        // quickly creates image from font string
        bool lazyLoadText( std::string textureText, TTF_Font *textFont );
        
        // Deallocates texture
        void free();

        // import the renderer object
        void setRenderer( SDL_Renderer *pRenderer );

        // Set color modulation
        void setColor( Uint8 red, Uint8 green, Uint8 blue );

        // Set TTF_Font Color 
        void setFontColor( SDL_Color font_color );

        // Set TTF_Font 
        void setFont( TTF_Font *font );

        // Set blending
        void setBlendMode( SDL_BlendMode blending );

        // Set alpha modulation
        void setAlpha( Uint8 alpha );
            
        // Renders texture at given point
        void render( SDL_Renderer *pRenderer, int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

        SDL_Texture* getRawTexture();

        // Gets image dimensions
        int getWidth();
        int getHeight();

        // Predefined color for the TTF_Font
        SDL_Color fontColor;
        SDL_Renderer *tRenderer;
        TTF_Font *textureFont;
    
    private:
        //The actual hardware texture
        SDL_Texture* mTexture;

        //Image dimensions
        int mWidth;
        int mHeight;
};

#endif /* Texture_hpp */
