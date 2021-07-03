// Texture Class
// @scawful

#include "Texture.hpp"

PTexture::PTexture()
{
    //Initialize
    tRenderer = NULL;
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

PTexture::~PTexture()
{
    //Deallocate
    free();
}

SDL_Texture* PTexture::loadTexture( SDL_Renderer *pRenderer, std::string path )
{
    //The final texture
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
    {
        SDL_Log( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( pRenderer, loadedSurface );
        if( newTexture == NULL )
        {
            SDL_Log( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }

    return newTexture;
}

bool PTexture::loadFromFile( SDL_Renderer *pRenderer, std::string path )
{
    //Get rid of preexisting texture
    free();

    //The final texture
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
    {
        SDL_Log( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }
    else
    {
        //Color key image
        SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0x15, 0xFF, 0 ) );

        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( pRenderer, loadedSurface );
        if( newTexture == NULL )
        {
            SDL_Log( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }
        else
        {
            //Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }

    //Return success
    mTexture = newTexture;
    return mTexture != NULL;
}

bool PTexture::loadFromRenderedText( SDL_Renderer *pRenderer, TTF_Font *gFont, std::string textureText, SDL_Color textColor )
{
    //Get rid of preexisting texture
    free();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
    if ( textSurface == NULL )
    {
        SDL_Log( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        this->mTexture = SDL_CreateTextureFromSurface( pRenderer, textSurface );
        if ( mTexture == NULL )
        {
            SDL_Log( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        }
        else
        {
            //Get image dimensions
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        //Get rid of old surface
        SDL_FreeSurface( textSurface );
    }
    
    //Return success
    return mTexture != NULL;
}

bool PTexture::lazyLoadText( std::string textureText, TTF_Font *textFont )
{
    free();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid( textFont, textureText.c_str(), fontColor );
    if ( textSurface == NULL )
    {
        SDL_Log( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        this->mTexture = SDL_CreateTextureFromSurface( tRenderer, textSurface );
        if ( mTexture == NULL )
        {
            SDL_Log( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        }
        else
        {
            //Get image dimensions
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        //Get rid of old surface
        SDL_FreeSurface( textSurface );
    }
    
    //Return success
    return mTexture != NULL;
}

void PTexture::free()
{
    //Free texture if it exists
    if( mTexture != NULL )
    {
        SDL_DestroyTexture( mTexture );
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void PTexture::setRenderer( SDL_Renderer *pRenderer )
{
    this->tRenderer = pRenderer;
}

void PTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
    //Modulate texture rgb
    SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void PTexture::setFontColor( SDL_Color font_color )
{
    this->fontColor = font_color;
}

void PTexture::setFont( TTF_Font *font )
{
    this->textureFont = font;
}

void PTexture::setBlendMode( SDL_BlendMode blending )
{
    //Set blending function
    SDL_SetTextureBlendMode( mTexture, blending );
}
        
void PTexture::setAlpha( Uint8 alpha )
{
    //Modulate texture alpha
    SDL_SetTextureAlphaMod( mTexture, alpha );
}

void PTexture::render( SDL_Renderer *pRenderer, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, mWidth, mHeight };

    //Set clip rendering dimensions
    if( clip != NULL )
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    //Render to screen
    SDL_RenderCopyEx( pRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

SDL_Texture* PTexture::getRawTexture()
{
    return mTexture;
}

int PTexture::getWidth()
{
    return mWidth;
}

int PTexture::getHeight()
{
    return mHeight;
}
