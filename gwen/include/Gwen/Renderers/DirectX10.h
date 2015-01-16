#ifndef GWEN_RENDERERS_DIRECTX10_H
#define GWEN_RENDERERS_DIRECTX10_H

#include "Gwen/Gwen.h"
#include "Gwen/BaseRender.h"

#include <d3d10.h>

#pragma comment( lib, "D3D10.lib" )
#pragma comment( lib, "D3Dx10.lib" )

namespace Gwen
{
	namespace Renderer
	{
#define SafeRelease(var) if(var) {var->Release(); var = NULL;}

		class GWEN_EXPORT DirectX10 : public Gwen::Renderer::Base
		{
		public:

			DirectX10(ID3D10Device* pDevice = NULL);
			~DirectX10();

			virtual void Init();

			virtual void Begin();
			virtual void End();
			virtual void Release();

			virtual void SetDrawColor(Gwen::Color color);

			virtual void DrawFilledRect(Gwen::Rect rect);

			virtual void LoadFont(Gwen::Font* pFont);
			virtual void FreeFont(Gwen::Font* pFont);
			virtual void RenderText(Gwen::Font* pFont, Gwen::Point pos, const Gwen::String & text);
			virtual void RenderText(Gwen::Font* pFont, Gwen::Point pos, const Gwen::UnicodeString & text);
			virtual Gwen::Point MeasureText(Gwen::Font* pFont, const Gwen::String & text);
			virtual Gwen::Point MeasureText(Gwen::Font* pFont, const Gwen::UnicodeString & text);

			void StartClip();
			void EndClip();

			void DrawTexturedRect(Gwen::Texture* pTexture, Gwen::Rect pTargetRect, float u1 = 0.0f, float v1 = 0.0f, float u2 = 1.0f, float v2 = 1.0f);
			void LoadTexture(Gwen::Texture* pTexture);
			void FreeTexture(Gwen::Texture* pTexture);
			Gwen::Color PixelColour(Gwen::Texture* pTexture, unsigned int x, unsigned int y, const Gwen::Color & col_default);

			//
			// Self Initialization
			//

			virtual bool InitializeContext(Gwen::WindowProvider* pWindow);
			virtual bool ShutdownContext(Gwen::WindowProvider* pWindow);
			virtual bool PresentContext(Gwen::WindowProvider* pWindow);
			virtual bool ResizedContext(Gwen::WindowProvider* pWindow, int w, int h);
			virtual bool BeginContext(Gwen::WindowProvider* pWindow);
			virtual bool EndContext(Gwen::WindowProvider* pWindow);

		protected:

			virtual void FillPresentParameters(Gwen::WindowProvider* pWindow, DXGI_SWAP_CHAIN_DESC & Params);

			FLOAT width, height;
			DWORD				m_Color;
			Gwen::Font::List	m_FontList;
			bool m_Valid;

			//Rendering
			ID3D10Device* m_pDevice;
			IDXGISwapChain* m_pSwapChain;
			ID3D10PixelShader* m_pPixShader;
			ID3D10PixelShader* m_pTexPixShader;
			ID3D10VertexShader* m_pVertShader;
			ID3D10BlendState* m_pBlendState;
			ID3D10InputLayout* m_pInputLayout;
			ID3D10RasterizerState* m_pRastState;
			ID3D10ShaderResourceView* m_pCurrentTexture;

			ID3D10BlendState*       m_pUILastBlendState;
			float                   m_LastBlendFactor[4];
			UINT				    m_LastBlendMask;
			UINT				    m_LastStencilRef;
			ID3D10InputLayout*      m_LastInputLayout;
			D3D10_PRIMITIVE_TOPOLOGY m_LastTopology;
			ID3D10Buffer*           m_LastBuffers[8];
			UINT				    m_LastStrides[8];
			UINT				    m_LastOffsets[8];
			ID3D10PixelShader*      m_LastPSShader;
			ID3D10VertexShader*     m_LastVSShader;
			ID3D10GeometryShader*   m_LastGSShader;
			ID3D10ShaderResourceView* m_pLastTexture[8];
			ID3D10DepthStencilState* m_LastDepthState;
			ID3D10RasterizerState*  m_pUILastRasterizerState;
			D3D10_RECT region;

			void Flush();
			void Present();
			void AddVert(int x, int y);
			void AddVert(int x, int y, float u, float v);

			struct VertexFormat
			{
				FLOAT x, y, z;
				DWORD color;
				float u, v;
			};

			template <typename T>
			class VertexBuffer
			{
			protected:
				T* data;
				ID3D10Buffer* m_vbuffer;
				DWORD maxVertices;
				DWORD numVertices;
				bool bufferResize;
				bool open;
			public:
				inline VertexBuffer(DWORD _maxVertices = 1)
					:bufferResize(true), numVertices(0), maxVertices(_maxVertices), open(false), m_vbuffer(nullptr), data(nullptr)
				{}

				inline ~VertexBuffer()
				{
					if (open)
						End();
					SafeRelease(m_vbuffer);
				}

				inline DWORD GetMaxVertices() const { return maxVertices; }

				inline DWORD GetNumVertices(void) const { return numVertices; }
				inline void SetNumVertices(DWORD value) { maxVertices = value; }

				inline ID3D10Buffer* & GetBuffer() { return m_vbuffer; }

				inline bool isBufferResizing() const { return (bufferResize || GetMaxVertices() < GetNumVertices()); }


				inline HRESULT Begin(ID3D10Device* pDevice)
				{
					HRESULT	hr;
					if (bufferResize)
					{
						SafeRelease(m_vbuffer);

						D3D10_BUFFER_DESC bufdesc = CD3D10_BUFFER_DESC(maxVertices * sizeof(T), D3D10_BIND_VERTEX_BUFFER, D3D10_USAGE_DYNAMIC, D3D10_CPU_ACCESS_WRITE);

						if (FAILED(hr = pDevice->CreateBuffer(&bufdesc, NULL, &m_vbuffer)))
							return hr;

						bufferResize = false;
					}

					if (FAILED(hr = m_vbuffer->Map(D3D10_MAP::D3D10_MAP_WRITE_DISCARD, 0, (void**)&data)))
						return hr;

					numVertices = 0;

					open = true;
					return S_OK;
				}

				inline void Add(const std::vector<T>& vertices)
				{
					auto size = vertices.size();
					if (open && GetMaxVertices() >= GetNumVertices() + size)
					{
						memcpy(data, vertices.data(), sizeof(T) * size);
						data += size;
					}
					numVertices += size;
				}

				inline void Add(const T& vertex)
				{
					if (open && GetMaxVertices() >= GetNumVertices() + 1)
						*data++ = vertex;

				}

				inline void Add(const T* vertices, int len)
				{
					if (open && GetMaxVertices() >= GetNumVertices() + len)
					{
						memcpy(data, vertices, len * sizeof(T));
						data += len;
					}
					numVertices += len;
				}

				inline HRESULT End()
				{
					open = false;

					m_vbuffer->Unmap();

					data = nullptr;

					if (bufferResize = isBufferResizing())
					{
						maxVertices = numVertices;
					}

					return S_OK;
				}

			};

			VertexBuffer<VertexFormat> m_Buffer;
		};
	}
}
#endif