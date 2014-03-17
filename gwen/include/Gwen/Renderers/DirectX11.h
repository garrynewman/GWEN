/*
    GWEN
    Copyright (c) 2012 Facepunch Studios
    See license in Gwen.h
*/

#ifndef GWEN_RENDERERS_DIRECTX11_H
#define GWEN_RENDERERS_DIRECTX11_H

#include <cstdint>

#include "Gwen/Gwen.h"
#include "Gwen/BaseRender.h"
#include "Gwen/Texture.h"

#include <d3d11.h>
#include <dxgi.h>

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "dxgi.lib" )

namespace Gwen
{
    namespace Renderer
    {

        class GWEN_EXPORT DirectX11 : public Gwen::Renderer::Base
        {
            public:

                DirectX11( ID3D11Device* device, ID3D11DeviceContext* dc, float width, float height);
                ~DirectX11();

                virtual void Begin();
                virtual void End();
                virtual void Release();

                virtual void SetDrawColor( Gwen::Color color );

                virtual void DrawFilledRect( Gwen::Rect rect );

                virtual void LoadFont( Gwen::Font* pFont );
                virtual void FreeFont( Gwen::Font* pFont );
                virtual void RenderText( Gwen::Font* pFont, Gwen::Point pos, const Gwen::UnicodeString & text );
                virtual Gwen::Point MeasureText( Gwen::Font* pFont, const Gwen::UnicodeString & text );

                void StartClip();
                void EndClip();

                void DrawTexturedRect( Gwen::Texture* pTexture, Gwen::Rect pTargetRect, float u1 = 0.0f, float v1 = 0.0f, float u2 = 1.0f, float v2 = 1.0f );
                void LoadTexture( Gwen::Texture* pTexture );
                void FreeTexture( Gwen::Texture* pTexture );
                Gwen::Color PixelColour( Gwen::Texture* pTexture, unsigned int x, unsigned int y, const Gwen::Color & col_default );

                // Should be called every time the window is resized.
                void SetWidthHeight(float width, float height) {m_width = width; m_height = height;}

            protected:

                DirectX11();
                DirectX11(const DirectX11& other);

                void*                       m_currentTexture;

                ID3D11Device*               m_device;
                ID3D11DeviceContext*        m_dc;

                ID3D11InputLayout*      m_pInputLayout;

                ID3D11VertexShader*     m_pUIVertexShader;
                ID3D11PixelShader*      m_pUIPixelShaderCol;
                ID3D11PixelShader*      m_pUIPixelShaderTex;

                ID3D11Texture2D*        m_pUITexture;
                ID3D11ShaderResourceView* m_pUITexSRV;
                
                ID3D11Buffer*           m_pUIVertexBuffer;
                ID3D11ShaderResourceView* m_pUIVertexBufferSRV;
                ID3D11BlendState*       m_pUILastBlendState;
                ID3D11BlendState*       m_pUIBlendState;
                ID3D11DepthStencilState*m_pUIDepthState;
                ID3D11RasterizerState*  m_pUIRasterizerStateScissor;
                ID3D11RasterizerState*  m_pUIRasterizerStateNormal;

                // Tells us whether we are currently using the textured pixel shader or not.
                bool m_amTextured;

                // State data from before Begin() was called that needs to be restored
                // when End() is called.
                float                   m_LastBlendFactor[4];
                uint32_t                m_LastBlendMask;
                uint32_t                m_LastStencilRef;
                ID3D11InputLayout*      m_LastInputLayout;
                D3D11_PRIMITIVE_TOPOLOGY m_LastTopology;
                ID3D11Buffer*           m_LastBuffers[8];
                uint32_t                m_LastStrides[8];
                uint32_t                m_LastOffsets[8];
                ID3D11PixelShader*      m_LastPSShader;
                ID3D11VertexShader*     m_LastVSShader;
                ID3D11GeometryShader*   m_LastGSShader;
                ID3D11DepthStencilState* m_LastDepthState;
                ID3D11RasterizerState*  m_pUILastRasterizerState;

                uint32_t                m_color;
                Gwen::Font::List        m_fontList;
                Gwen::Texture::List     m_TextureList;
                wchar_t                 m_defaultCharacter;

                float                   m_width;
                float                   m_height;



                void Flush();
                void AddVert( int x, int y );
                void AddVert( int x, int y, float u, float v );

            protected:

                struct VertexFormat
                {
                    float x, y;
                    uint32_t color;
                    float u, v;
                };

                struct FontData
                {
                    // Describes a single character glyph.
                    struct Glyph
                    {
                        uint32_t Character;
                        RECT Subrect;
                        float XOffset;
                        float YOffset;
                        float XAdvance;
                    };

                    std::vector<Glyph> charset;
                    ID3D11Texture2D* texture;
                    ID3D11ShaderResourceView* srv;
                    uint32_t spaceAdvance;
                    float linespacing;
                };

                static const int        MaxVerts = 1024;
                VertexFormat            m_pVerts[MaxVerts];
                int                     m_iVertNum;
        };

    }
}
#endif // GWEN_RENDERERS_DIRECTX11_H
