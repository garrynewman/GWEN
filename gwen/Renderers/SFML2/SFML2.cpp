
#include "Gwen/Gwen.h"
#include "Gwen/BaseRender.h"
#include "Gwen/Font.h"
#include "Gwen/Texture.h"
#include "Gwen/Renderers/SFML2.h"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

#include <GL/gl.h>

#include <cmath>

struct TextureData
{
	TextureData(sf::Image* img): texture(NULL), image(img) { }
	TextureData(sf::Texture* text): texture(text), image(NULL) { }
	~TextureData()
	{
		if (texture != NULL) delete texture;
		if (image != NULL) delete image;
	}

	sf::Texture *texture;
	sf::Image *image;
};

Gwen::Renderer::SFML2::SFML2( sf::RenderTarget& target ):
	myTarget(target), myColor(), myBuffer(), myRenderStates(sf::RenderStates::Default), myHeight(myTarget.getSize().y)
{
	myBuffer.setPrimitiveType(sf::Triangles);
	myRenderStates.blendMode = sf::BlendAlpha;
}

Gwen::Renderer::SFML2::~SFML2() {}

void Gwen::Renderer::SFML2::Begin()
{
	myOriginalView = myTarget.getView();
	sf::FloatRect vrect;
	vrect.left = 0;	vrect.top = 0;
	vrect.width = myTarget.getSize().x;	vrect.height = myHeight = myTarget.getSize().y;
	sf::FloatRect vprect;
	vprect.left = 0; vprect.top = 0;
	vprect.width = 1.0f; vprect.height = 1.0f;
	sf::View view(vrect);
	view.setViewport(vprect);
	myTarget.setView(view);
}

void Gwen::Renderer::SFML2::End()
{
	myTarget.setView(myOriginalView);
}

void Gwen::Renderer::SFML2::StartClip()
{
	Flush();

	Gwen::Rect rect = ClipRegion();
	float x = rect.x-1, y = rect.y, w = rect.w+1, h = rect.h+1;

	// OpenGL's coords are from the bottom left
	// so we need to translate them here.
	y = myHeight - (y + h);

	float scale = Scale();

	glEnable(GL_SCISSOR_TEST);
	glScissor(x * scale, y * scale, w * scale, h * scale);
}

void Gwen::Renderer::SFML2::EndClip()
{
	Flush();
	glDisable(GL_SCISSOR_TEST);
}

void Gwen::Renderer::SFML2::SetDrawColor(Gwen::Color color)
{
	myColor = sf::Color(color.r, color.g, color.b, color.a);
}

void Gwen::Renderer::SFML2::DrawPixel( int x, int y )
{
	EnsurePrimitiveType(sf::Points);
	EnsureTexture(NULL);
	Translate(x, y);
	AddVert(x, y+1);
}

void Gwen::Renderer::SFML2::DrawLinedRect(Gwen::Rect rect)
{
	EnsurePrimitiveType(sf::Lines);
	EnsureTexture(NULL);

	Translate (rect);

	//   (x,y) ---------- (x+w, y)
	//         |        |
	// (x,y+h) ---------- (x+w,y+h)

	AddVert( rect.x, rect.y);
	AddVert( rect.x+rect.w, rect.y	);

	AddVert( rect.x+rect.w, rect.y	);
	AddVert( rect.x+rect.w, rect.y+rect.h);

	AddVert( rect.x+rect.w, rect.y+rect.h);
	AddVert( rect.x, rect.y + rect.h );

	AddVert( rect.x, rect.y + rect.h );
	AddVert( rect.x, rect.y);
}

void Gwen::Renderer::SFML2::DrawFilledRect(Gwen::Rect rect)
{

	EnsurePrimitiveType(sf::Triangles);
	EnsureTexture(NULL);

	Translate( rect );

	AddVert( rect.x, rect.y );
	AddVert( rect.x+rect.w, rect.y );
	AddVert( rect.x, rect.y + rect.h );

	AddVert( rect.x+rect.w, rect.y );
	AddVert( rect.x+rect.w, rect.y+rect.h );
	AddVert( rect.x, rect.y + rect.h );
}

void Gwen::Renderer::SFML2::DrawShavedCornerRect(Gwen::Rect rect, bool bSlight)
{
	//TODO: Implement this
	Base::DrawShavedCornerRect(rect, bSlight);
}

void Gwen::Renderer::SFML2::DrawTexturedRect(Gwen::Texture* pTexture, Gwen::Rect rect, float u1, float v1, float u2, float v2)
{
	TextureData* data = reinterpret_cast<TextureData*>( pTexture->data );

	// Missing image, not loaded properly?
	if ( !data )
		return DrawMissingImage( rect );

	const sf::Texture* tex = data->texture;

	EnsurePrimitiveType(sf::Triangles);
	EnsureTexture(tex);

	Translate( rect );

	AddVert( rect.x, rect.y, u1, v1 );
	AddVert( rect.x+rect.w, rect.y,	u2, v1 );
	AddVert( rect.x, rect.y + rect.h,	u1, v2 );

	AddVert( rect.x+rect.w, rect.y, u2, v1 );
	AddVert( rect.x+rect.w, rect.y+rect.h, u2, v2 );
	AddVert( rect.x, rect.y + rect.h, u1, v2 );
}

void Gwen::Renderer::SFML2::LoadFont(Gwen::Font* font)
{
	font->realsize = font->size * Scale();

	sf::Font* pFont = new sf::Font();

	if ( !pFont->loadFromFile( Utility::UnicodeToString( font->facename ) ) )
	{
		// Ideally here we should be setting the font to a system default font here.
		delete pFont;
		pFont = NULL; // SFML 2 doesn't have a default font anymore
	}

	font->data = pFont;
}

void Gwen::Renderer::SFML2::FreeFont(Gwen::Font* pFont)
{
	if ( !pFont->data ) return;
	sf::Font* font = reinterpret_cast<sf::Font*>(pFont->data);
	delete font;
	pFont->data = NULL;
}

void Gwen::Renderer::SFML2::RenderText(Gwen::Font* pFont, Gwen::Point pos, const Gwen::UnicodeString& text)
{
	Flush();

	Translate( pos.x, pos.y );

	// If the font doesn't exist, or the font size should be changed
	if ( !pFont->data || fabs( pFont->realsize - pFont->size * Scale() ) > 2 )
	{
		FreeFont( pFont );
		LoadFont( pFont );
	}

	const sf::Font* pSFFont = reinterpret_cast<sf::Font*>(pFont->data);

	sf::Text sfStr;
	sfStr.setString( text );
	sfStr.setFont( *pSFFont );
	sfStr.move( pos.x, pos.y );
	sfStr.setCharacterSize( pFont->realsize );
	sfStr.setColor( myColor );
	myTarget.draw( sfStr );
}

Gwen::Point Gwen::Renderer::SFML2::MeasureText(Gwen::Font* pFont, const Gwen::UnicodeString& text)
{
	// If the font doesn't exist, or the font size should be changed
	if ( !pFont->data || fabs( pFont->realsize - pFont->size * Scale() ) > 2 )
	{
		FreeFont( pFont );
		LoadFont( pFont );
	}

	const sf::Font* pSFFont = reinterpret_cast<sf::Font*>(pFont->data);

	sf::Text sfStr;
	sfStr.setString( text );
	sfStr.setFont( *pSFFont );
	sfStr.setScale(Scale(), Scale());
	sfStr.setCharacterSize( pFont->realsize );
	sf::FloatRect sz = sfStr.getLocalBounds();
	return Gwen::Point( sz.left + sz.width, sz.top + sz.height );
}

void Gwen::Renderer::SFML2::LoadTexture(Gwen::Texture* pTexture)
{
	if ( !pTexture ) return;
	if ( pTexture->data ) FreeTexture( pTexture );

	sf::Texture* tex = new sf::Texture();
	tex->setSmooth( true );
	if ( !tex->loadFromFile( pTexture->name.Get() ) )
	{
		delete( tex );
		pTexture->failed = true;
		return;
	}

	pTexture->height = tex->getSize().x;
	pTexture->width = tex->getSize().y;
	pTexture->data = new TextureData(tex);
}

void Gwen::Renderer::SFML2::FreeTexture(Gwen::Texture* pTexture)
{
	TextureData* data = reinterpret_cast<TextureData*>( pTexture->data );

	if ( data )
		delete data;

	pTexture->data = NULL;
}

Gwen::Color Gwen::Renderer::SFML2::PixelColour(Gwen::Texture* pTexture, unsigned int x, unsigned int y, const Gwen::Color& col_default)
{
	TextureData* data = static_cast<TextureData*>( pTexture->data );

	if ( !data->texture && !data->image ) return col_default;
	if ( !data->image )
	{
		sf::Image copy = data->texture->copyToImage();
		data->image = new sf::Image(copy);
	}

	sf::Color col = data->image->getPixel( x, y );
	return Gwen::Color( col.r, col.g, col.b, col.a );
}


