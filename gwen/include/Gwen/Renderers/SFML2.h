/*
	GWEN
	Copyright (c) 2011 Facepunch Studios
	See license in Gwen.h
*/

#ifndef SFML2_HPP
#define SFML2_HPP

#include "Gwen/Gwen.h"
#include "Gwen/BaseRender.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace Gwen
{
	namespace Renderer
	{
		class SFML2: public Gwen::Renderer::Base
		{
		public:

			SFML2( sf::RenderTarget& target );
			virtual ~SFML2();

			inline void EnsurePrimitiveType(sf::PrimitiveType type)
			{
				if (myBuffer.getPrimitiveType() != type)
				{
					Flush();
					myBuffer.setPrimitiveType(type);
				}
			}

			inline void EnsureTexture(const sf::Texture *texture)
			{
				if (myRenderStates.texture != texture)
				{
					Flush();
					myRenderStates.texture = texture;
				}
			}

			inline void AddVert( int x, int y, float u = 0.0f , float v = 0.0f )
			{
				if (myRenderStates.texture)
				{
					u *= myRenderStates.texture->getSize().x;
					v *= myRenderStates.texture->getSize().y;
				}
				myBuffer.append(sf::Vertex(sf::Vector2f(x, y), myColor, sf::Vector2f(u, v)));
			}

			inline void Flush()
			{
				if (myBuffer.getVertexCount() > 0)
				{
					myTarget.draw(myBuffer, myRenderStates);
					myBuffer.clear();
				}
			}

			virtual void Begin();
			virtual void End();

			virtual void StartClip();
			virtual void EndClip();

			virtual void SetDrawColor(Gwen::Color color);
			virtual void DrawPixel( int x, int y );
			virtual void DrawLinedRect( Gwen::Rect rect );
			virtual void DrawFilledRect( Gwen::Rect rect );
			virtual void DrawShavedCornerRect(Gwen::Rect rect, bool bSlight = false);
			virtual void DrawTexturedRect( Gwen::Texture* pTexture, Gwen::Rect rect, float u1, float v1, float u2, float v2 );

			virtual void RenderText( Gwen::Font* pFont, Gwen::Point pos, const Gwen::UnicodeString& text );
			virtual Gwen::Point MeasureText( Gwen::Font* pFont, const Gwen::UnicodeString& text );
			virtual void LoadFont( Gwen::Font* font );
			virtual void FreeFont( Gwen::Font* pFont );

			virtual void LoadTexture( Gwen::Texture* pTexture );
			virtual void FreeTexture( Gwen::Texture* pTexture );
			virtual Gwen::Color PixelColour( Gwen::Texture* pTexture, unsigned int x, unsigned int y, const Gwen::Color& col_default );

		protected:
			sf::RenderTarget&	myTarget;
			sf::Color	myColor;
			sf::VertexArray myBuffer;
			sf::RenderStates myRenderStates;
			sf::View myOriginalView;
			int myHeight;
		};
	}
}

#endif // SFML2_HPP
