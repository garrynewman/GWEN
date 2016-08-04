#include "Gwen/Renderers/DirectX11.h"
#include "Gwen/Utility.h"
#include "Gwen/Font.h"
#include "Gwen/Texture.h"
#include "Gwen/WindowProvider.h"

#include <xnamath.h>
#include <d3dx11.h>
#include <d3dcompiler.h>

#define D3DCOLOR_ARGB(a,r,g,b) \
    ((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#define D3DCOLOR_COLORVALUE(r,g,b,a) \
    D3DCOLOR_ARGB((DWORD)((a)*255.f), (DWORD)((r)*255.f),(DWORD)((g)*255.f),(DWORD)((b)*255.f))

#pragma region Shaders
const char pixshader[] = {
	"sampler samp0 : register(s0);\n"
	"Texture2D tex2D : register(t0);\n"
	"struct PS_INPUT\n"
	"{\n"
	"	float4 pos : SV_POSITION;\n"
	"	float4 col : COLOR;\n"
	"	float2 tex : TEXCOORD;\n"
	"};\n"
	"float4 texmain( PS_INPUT input ) : SV_Target\n"
	"{\n"
	"	return tex2D.Sample(samp0, input.tex) * input.col;\n"
	"}\n"
	"float4 main( PS_INPUT input ) : SV_Target\n"
	"{\n"
	"	return input.col;\n"
	"}"
};

const char vertshader[] = {
	"struct VS_INPUT\n"
	"{\n"
	"	float4 pos : POSITION;\n"
	"	float4 col : COLOR;\n"
	"	float2 tex : TEXCOORD;\n"
	"};\n"
	"struct PS_INPUT\n"
	"{\n"
	"	float4 pos : SV_POSITION;\n"
	"	float4 col : COLOR;\n"
	"	float2 tex : TEXCOORD;\n"
	"};\n"
	"PS_INPUT main( VS_INPUT input )\n"
	"{\n"
	"	PS_INPUT output;\n"
	"	output.pos = input.pos;\n"
	"	output.col = input.col;\n"
	"	output.tex = input.tex;\n"
	"	return output;\n"
	"};\n"
};

inline HRESULT CompileShaderFromMemory(const char* szdata, SIZE_T len, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
	
	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromMemory(szdata, len, NULL, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);

	if (FAILED(hr))
	{
		if (pErrorBlob)
			pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob)
		pErrorBlob->Release();

	return S_OK;
}
#pragma endregion
namespace Gwen
{
	namespace Renderer
	{
		class FontData
		{
		public:
			FontData()
			{
				m_Texture = NULL;
				m_TexWidth = 0;
				m_TexHeight = 0;
				m_Spacing = 0.f;
			}

			~FontData()
			{
				SafeRelease(m_Texture);
			}

			XMFLOAT4 m_fTexCoords[0x60];

			UINT32   m_TexWidth;
			UINT32   m_TexHeight;
			float   m_Spacing;
			ID3D11ShaderResourceView* m_Texture;
		};

		DirectX11::DirectX11(ID3D11Device* pDevice) : m_pDevice(pDevice), m_Buffer(256)
		{
			m_Valid = false;

			m_pSwapChain = NULL;
			width = height = 0;

			m_pContext = NULL;
			m_pRastState = NULL;
			m_pPixShader = NULL;
			m_pVertShader = NULL;
			m_pBlendState = NULL;
			m_pInputLayout = NULL;
			m_pTexPixShader = NULL;
			m_pCurrentTexture = NULL;

			if (m_pDevice)
				Init();
		}

		DirectX11::~DirectX11()
		{
			Release();
		}

		void DirectX11::Init()
		{
			m_Valid = false;

			m_pDevice->GetImmediateContext(&m_pContext);

			ID3DBlob* pVSBlob = NULL;
			ID3DBlob* pPSBlob = NULL;
			ID3DBlob* pTexPSBlob = NULL;

			if (FAILED(CompileShaderFromMemory(vertshader, sizeof(vertshader), "main", "vs_4_0", &pVSBlob)))
				return;

			if (FAILED(CompileShaderFromMemory(pixshader, sizeof(pixshader), "main", "ps_4_0", &pPSBlob))
				|| FAILED(CompileShaderFromMemory(pixshader, sizeof(pixshader), "texmain", "ps_4_0", &pTexPSBlob)))
			{
				SafeRelease(pVSBlob);
				SafeRelease(pPSBlob);
				SafeRelease(pTexPSBlob);
				return;
			}

			if (FAILED(m_pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &m_pVertShader))
				|| FAILED(m_pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &m_pPixShader))
				|| FAILED(m_pDevice->CreatePixelShader(pTexPSBlob->GetBufferPointer(), pTexPSBlob->GetBufferSize(), NULL, &m_pTexPixShader)))
			{
				SafeRelease(pVSBlob);
				SafeRelease(pPSBlob);
				SafeRelease(pTexPSBlob);

				Release();
				return;
			}

			SafeRelease(pPSBlob);
			SafeRelease(pTexPSBlob);

			D3D11_INPUT_ELEMENT_DESC layout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			UINT numElements = ARRAYSIZE(layout);

			if (FAILED(m_pDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pInputLayout)))
			{
				SafeRelease(pVSBlob);

				Release();
				return;
			}

			SafeRelease(pVSBlob);

			D3D11_BLEND_DESC blenddesc;
			blenddesc.AlphaToCoverageEnable = FALSE;
			blenddesc.IndependentBlendEnable = FALSE;
			blenddesc.RenderTarget[0].BlendEnable = TRUE;
			blenddesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			blenddesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blenddesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blenddesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blenddesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
			blenddesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			blenddesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

			if (FAILED(m_pDevice->CreateBlendState(&blenddesc, &m_pBlendState)))
			{
				Release();
				return;
			}

			D3D11_RASTERIZER_DESC rastdesc = CD3D11_RASTERIZER_DESC(D3D11_FILL_SOLID, D3D11_CULL_NONE, false, 0, 0.f, 0.f, false, true, false, false);

			if (FAILED(m_pDevice->CreateRasterizerState(&rastdesc, &m_pRastState)))
			{
				Release();
				return;
			}

			D3D11_VIEWPORT vp;
			UINT numViewports = 1;
			m_pContext->RSGetViewports(&numViewports, &vp);

			width = vp.Width;
			height = vp.Height;

			region.left = 0.f;
			region.right = width;
			region.top = 0.f;
			region.bottom = height;
			m_pContext->RSSetScissorRects(1, &region);

			m_Valid = true;
		}

		void DirectX11::Release()
		{
			m_Valid = false;

			Font::List::iterator it = m_FontList.begin();

			while (it != m_FontList.end())
			{
				FreeFont(*it);
				it = m_FontList.begin();
			}

			SafeRelease(m_pRastState);
			SafeRelease(m_pPixShader);
			SafeRelease(m_pBlendState);
			SafeRelease(m_pVertShader);
			SafeRelease(m_pInputLayout);
			SafeRelease(m_pTexPixShader);
		}

		void DirectX11::Begin()
		{
			if (FAILED(m_Buffer.Begin(m_pDevice)))
				return;

			if (!m_Valid)
				Init();

			
			// Save current state
			m_pContext->OMGetBlendState(&m_pUILastBlendState, m_LastBlendFactor, &m_LastBlendMask);
			m_pContext->RSGetState(&m_pUILastRasterizerState);
			m_pContext->OMGetDepthStencilState(&m_LastDepthState, &m_LastStencilRef);
			m_pContext->IAGetInputLayout(&m_LastInputLayout);
			m_pContext->IAGetPrimitiveTopology(&m_LastTopology);
			m_pContext->IAGetVertexBuffers(0, 8, m_LastBuffers, m_LastStrides, m_LastOffsets);
			m_pContext->PSGetShaderResources(0, 8, m_pLastTexture);
			m_pContext->PSGetShader(&m_LastPSShader, NULL, 0);
			m_pContext->GSGetShader(&m_LastGSShader, NULL, 0);
			m_pContext->VSGetShader(&m_LastVSShader, NULL, 0);
			
			m_pCurrentTexture = m_pLastTexture[0];

			m_pContext->RSSetState(m_pRastState);
			m_pContext->IASetInputLayout(m_pInputLayout);
			m_pContext->OMSetBlendState(m_pBlendState, NULL, 0xFFFFFFFF);

		}

		void DirectX11::End()
		{
			m_Buffer.End();
			Present();
			
			m_pContext->OMSetBlendState(m_pUILastBlendState, m_LastBlendFactor, m_LastBlendMask);
			m_pContext->RSSetState(m_pUILastRasterizerState);
			m_pContext->OMSetDepthStencilState(m_LastDepthState, m_LastStencilRef);
			m_pContext->IASetInputLayout(m_LastInputLayout);
			m_pContext->IASetPrimitiveTopology(m_LastTopology);
			m_pContext->IASetVertexBuffers(0, 8, m_LastBuffers, m_LastStrides, m_LastOffsets);
			m_pContext->PSSetShaderResources(0, 8, m_pLastTexture);
			m_pContext->PSSetShader(m_LastPSShader, NULL, 0);
			m_pContext->GSSetShader(m_LastGSShader, NULL, 0);
			m_pContext->VSSetShader(m_LastVSShader, NULL, 0);
		}

		void DirectX11::Present()
		{
			UINT stride = sizeof(VertexFormat);
			UINT offset = 0;

			m_pContext->IASetVertexBuffers(0, 1, &m_Buffer.GetBuffer(), &stride, &offset);

			if (m_pCurrentTexture != NULL)
			{
				m_pContext->PSSetShader(m_pTexPixShader, NULL, 0);
				m_pContext->PSSetShaderResources(0, 1, &m_pCurrentTexture);
			}
			else
				m_pContext->PSSetShader(m_pPixShader, NULL, 0);

			m_pContext->VSSetShader(m_pVertShader, NULL, 0);
			m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			m_pContext->Draw(m_Buffer.GetNumVertices(), 0);
		}

		void DirectX11::Flush()
		{
			m_Buffer.End();
			Present();
			m_Buffer.Begin(m_pDevice);
		}

		void DirectX11::DrawFilledRect(Gwen::Rect rec)
		{
			if (m_pCurrentTexture != NULL)
			{
				Flush();
				m_pCurrentTexture = NULL;
			}

			Translate(rec);

			float scalex = 1 / width * 2.f;
			float scaley = 1 / height * 2.f;

			XMFLOAT4A rect(rec.x, rec.y, rec.w, rec.h);

			rect.z = (rect.x + rect.z) * scalex - 1.f;
			rect.w = 1.f - (rect.y + rect.w) * scaley;
			rect.x = rect.x * scalex - 1.f;
			rect.y = 1.f - rect.y * scaley;

			VertexFormat verts[6] =
			{
				{ rect.x, rect.w, 0.5f, m_Color, 0.f, 0.f },
				{ rect.x, rect.y, 0.5f, m_Color, 0.f, 0.f },
				{ rect.z, rect.w, 0.5f, m_Color, 0.f, 0.f },
				{ rect.z, rect.y, 0.5f, m_Color, 0.f, 0.f },
				{ rect.z, rect.w, 0.5f, m_Color, 0.f, 0.f },
				{ rect.x, rect.y, 0.5f, m_Color, 0.f, 0.f }
			};

			m_Buffer.Add(verts, ARRAYSIZE(verts));
		}

		void DirectX11::SetDrawColor(Gwen::Color color)
		{
			m_Color = D3DCOLOR_ARGB(color.a, color.r, color.g, color.b);
		}

		void DirectX11::LoadFont(Gwen::Font* font)
		{
			font->realsize = font->size * Scale();

			DWORD texWidth, texHeight;
			if (font->realsize > 60)
				texWidth = 2048;
			else if (font->realsize > 30)
				texWidth = 1024;
			else if (font->realsize > 15)
				texWidth = 512;
			else
				texWidth = 256;

			DWORD*      pBitmapBits;
			BITMAPINFO bmi;
			ZeroMemory(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));

			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = texWidth;
			bmi.bmiHeader.biHeight = -static_cast<int>(texWidth);
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biBitCount = 32;

			HDC hDC = CreateCompatibleDC(NULL);
			HBITMAP hBitmap = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (void**)&pBitmapBits, NULL, 0);
			SetMapMode(hDC, MM_TEXT);

			LOGFONTW fd;

			memset(&fd, 0, sizeof(fd));
			wcscpy_s(fd.lfFaceName, LF_FACESIZE, font->facename.c_str());
			fd.lfWidth = 0;
			
			fd.lfCharSet = DEFAULT_CHARSET;
			fd.lfHeight = font->realsize * -1.0f;
			fd.lfOutPrecision = OUT_DEFAULT_PRECIS;
			fd.lfItalic = 0;
			fd.lfWeight = font->bold ? FW_BOLD : FW_NORMAL;
#ifdef CLEARTYPE_QUALITY
			fd.lfQuality = font->realsize < 14 ? DEFAULT_QUALITY : CLEARTYPE_QUALITY;
#else
			fd.lfQuality = PROOF_QUALITY;
#endif
			fd.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
			
			HFONT hFont = CreateFontIndirectW(&fd);

			if (!hFont)
				return;

			SelectObject(hDC, hBitmap);
			SelectObject(hDC, hFont);

			SetTextColor(hDC, RGB(0xFF, 0xFF, 0xFF));
			SetBkColor(hDC, 0x00000000);
			SetTextAlign(hDC, TA_TOP);

			float x = 0, y = 0;
			wchar_t str[2] = L"x";
			SIZE sz;

			GetTextExtentPoint32W(hDC, L" ", 1, &sz);

			float spacing = sz.cx;

			std::vector<XMFLOAT4> sizes;

			for (wchar_t c = L' '; c < L'~'+1; c++)
			{
				str[0] = c;
				GetTextExtentPoint32W(hDC, str, 1, &sz);

				if (x + sz.cx > texWidth)
				{
					x = 0;
					y += sz.cy;
				}
				
				x = ceilf(x);
				y = ceilf(y);

				sizes.push_back(XMFLOAT4(x, y, x + sz.cx, y + sz.cy));

				x += sz.cx + spacing / 2;
			}

			texHeight = y + sz.cy;

			FontData* data = new FontData();

			byte c = 0;
			for each (auto& var in sizes)
			{
				str[0] = L' ' + c;

				float x = ceilf(var.x);
				float y = ceilf(var.y);

				ExtTextOutW(hDC, static_cast<int>(x), static_cast<int>(y), ETO_OPAQUE | ETO_CLIPPED, NULL, str, 1, NULL);

				data->m_fTexCoords[c++] = { x / texWidth, y / texHeight, var.z / texWidth, var.w / texHeight };
			}

			data->m_Spacing = spacing;
			data->m_TexHeight = texHeight;
			data->m_TexWidth = texWidth;
			
			DWORD len = texWidth * texHeight;

			ID3D11Texture2D* buftex;
			D3D11_TEXTURE2D_DESC texdesc = CD3D11_TEXTURE2D_DESC(DXGI_FORMAT_R8G8B8A8_UNORM, data->m_TexWidth, data->m_TexHeight, 1, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

			if (FAILED(m_pDevice->CreateTexture2D(&texdesc, nullptr, &buftex)))
			{
				delete data;

				DeleteObject(hBitmap);
				DeleteObject(hFont);
				DeleteDC(hDC);

				return;
			}

			D3D11_MAPPED_SUBRESOURCE texmap;
			if (FAILED(m_pContext->Map(buftex, 0, D3D11_MAP_WRITE_DISCARD, 0, &texmap)))
			{
				delete data;

				DeleteObject(hBitmap);
				DeleteObject(hFont);
				DeleteDC(hDC);
				SafeRelease(buftex);

				return;
			}
			
			BYTE bAlpha;
			DWORD* pDst32;
			BYTE* pDstRow = (BYTE*)texmap.pData;

			for (UINT32 y = 0; y < data->m_TexHeight; y++)
			{
				pDst32 = (DWORD*)pDstRow;
				for (UINT32 x = 0; x < data->m_TexWidth; x++)
				{
					//bAlpha = BYTE( & 0xFF);
					if (pBitmapBits[data->m_TexWidth * y + x] > 0)
					{
						DWORD col = pBitmapBits[data->m_TexWidth * y + x];
						float fcol[] = { ((BYTE*)&col)[3] / 255.f, ((BYTE*)&col)[2] / 255.f, ((BYTE*)&col)[1] / 255.f, ((BYTE*)&col)[0] / 255.f };
						fcol[3] = fcol[2] * 0.30f + fcol[1] * 0.59f + fcol[0] * 0.11f;
						*pDst32++ = D3DCOLOR_COLORVALUE(1.f, 1.f, 1.f, fcol[3]);
					}
					else
						*pDst32++ = 0;
				}
				pDstRow += texmap.RowPitch;
			}

			DeleteObject(hBitmap);
			DeleteObject(hFont);
			DeleteDC(hDC);

			m_pContext->Unmap(buftex, 0);

			if (FAILED(m_pDevice->CreateShaderResourceView(buftex, nullptr, &data->m_Texture)))
			{
				delete data;

				SafeRelease(buftex);

				return;
			}

			SafeRelease(buftex);

			font->data = data;

			m_FontList.push_back(font);
		}

		void DirectX11::FreeFont(Gwen::Font* pFont)
		{
			m_FontList.remove(pFont);

			if (!pFont->data)
				return;

			FontData* pFontData = (FontData*)pFont->data;

			delete pFontData;

			pFont->data = NULL;
		}

		void DirectX11::RenderText(Gwen::Font* pFont, Gwen::Point pos, const Gwen::String & text)
		{
			Flush();

			// If the font doesn't exist, or the font size should be changed
			if (!pFont->data || fabs(pFont->realsize - pFont->size * Scale()) > 2)
			{
				FreeFont(pFont);
				LoadFont(pFont);
			}

			FontData* pFontData = (FontData*)pFont->data;

			Translate(pos.x, pos.y);


			XMFLOAT4A loc(pos.x, pos.y, 0, 0);

			FontData* data = (FontData*)pFont->data;

			if (m_pCurrentTexture != data->m_Texture)
			{
				m_pCurrentTexture = data->m_Texture;
			}

			
			float fStartX = loc.x;
			
			for each(auto c in text)
			{
				if (c < 32 || c >= 128)
				{
					if (c == '\n')
					{
						loc.x = fStartX;
						loc.y += (data->m_fTexCoords[c - 32].w - data->m_fTexCoords[c - 32].y) * data->m_TexHeight;
					}
					else
						continue;
				}

				c -= 32;

				loc.z = loc.x + ((data->m_fTexCoords[c].z - data->m_fTexCoords[c].x) * data->m_TexWidth);
				loc.w = loc.y + ((data->m_fTexCoords[c].w - data->m_fTexCoords[c].y) * data->m_TexHeight);

				if (c != 0)
				{
					float scalex = 1 / (float)width * 2.f;
					float scaley = 1 / (float)height * 2.f;

					XMFLOAT4A rect(loc);

					rect.z = rect.z * scalex - 1.f;
					rect.w = 1.f - rect.w * scaley;
					rect.x = rect.x * scalex - 1.f;
					rect.y = 1.f - rect.y * scaley;

					VertexFormat v[6];
					v[0] = { rect.x, rect.w, 0.5f, m_Color, data->m_fTexCoords[c].x, data->m_fTexCoords[c].w };
					v[1] = { rect.x, rect.y, 0.5f, m_Color, data->m_fTexCoords[c].x, data->m_fTexCoords[c].y };
					v[2] = { rect.z, rect.w, 0.5f, m_Color, data->m_fTexCoords[c].z, data->m_fTexCoords[c].w };
					v[3] = { rect.z, rect.y, 0.5f, m_Color, data->m_fTexCoords[c].z, data->m_fTexCoords[c].y };
					v[4] = v[2];
					v[5] = v[1];

					m_Buffer.Add(v, ARRAYSIZE(v));
				}
				loc.x += (loc.z - loc.x);
			}

			Flush();
		}

		void DirectX11::RenderText(Gwen::Font* pFont, Gwen::Point pos, const Gwen::UnicodeString & text)
		{
			RenderText(pFont, pos, Utility::UnicodeToString(text));
		}

		Gwen::Point DirectX11::MeasureText(Gwen::Font* pFont, const Gwen::String & text)
		{
			// If the font doesn't exist, or the font size should be changed
			if (!pFont->data || fabs(pFont->realsize - pFont->size * Scale()) > 2)
			{
				FreeFont(pFont);
				LoadFont(pFont);
			}

			FontData* font = (FontData*)pFont->data;

			float fRowWidth = 0.0f;
			float fRowHeight = (font->m_fTexCoords[0].w - font->m_fTexCoords[0].y) * font->m_TexHeight;
			float fWidth = 0.0f;
			float fHeight = fRowHeight;

			for each(auto c in text)
			{
				if (c == '\n')
				{
					fRowWidth = 0.0f;
					fHeight += fRowHeight;
				}

				c -= 32;

				if (c < 0 || c >= 96)
					continue;


				float tx1 = font->m_fTexCoords[c].x;
				float tx2 = font->m_fTexCoords[c].z;

				fRowWidth += (tx2 - tx1)* font->m_TexWidth;

				if (fRowWidth > fWidth)
					fWidth = fRowWidth;
			}

			return Gwen::Point(fWidth, fHeight);
		}

		Gwen::Point DirectX11::MeasureText(Gwen::Font* pFont, const Gwen::UnicodeString & text)
		{
			return MeasureText(pFont, Utility::UnicodeToString(text));
		}

		void DirectX11::StartClip()
		{
			Flush();
			const Gwen::Rect & rect = ClipRegion();

			region.left = ceil(((float)rect.x) * Scale());
			region.right = ceil(((float)(rect.x + rect.w)) * Scale());
			region.top = ceil((float)rect.y * Scale());
			region.bottom = ceil(((float)(rect.y + rect.h)) * Scale());
			
			m_pContext->RSSetScissorRects(1, &region);
		}

		void DirectX11::EndClip()
		{
			Flush();

			region.left = 0.f;
			region.right = width;
			region.top = 0.f;
			region.bottom = height;
			m_pContext->RSSetScissorRects(1, &region);
		}

		void DirectX11::DrawTexturedRect(Gwen::Texture* pTexture, Gwen::Rect rec, float u1, float v1, float u2, float v2)
		{
			ID3D11ShaderResourceView* pImage = (ID3D11ShaderResourceView*)pTexture->data;

			if (!pImage)
				return DrawMissingImage(rec);

			if (m_pCurrentTexture != pImage)
			{
				Flush();
				m_pCurrentTexture = pImage;
			}

			float scalex = 1 / width * 2.f;
			float scaley = 1 / height * 2.f;

			Translate(rec);

			XMFLOAT4A rect(rec.x, rec.y, rec.w, rec.h);

			rect.z = (rect.x + rect.z) * scalex - 1.f;
			rect.w = 1.f - (rect.y + rect.w) * scaley;
			rect.x = rect.x * scalex - 1.f;
			rect.y = 1.f - rect.y * scaley;

			VertexFormat verts[6] =
			{
				{ rect.x, rect.w, 0.5f, m_Color, u1, v2 },
				{ rect.x, rect.y, 0.5f, m_Color, u1, v1 },
				{ rect.z, rect.w, 0.5f, m_Color, u2, v2 },
				{ rect.z, rect.y, 0.5f, m_Color, u2, v1 },
				{ rect.z, rect.w, 0.5f, m_Color, u2, v2 },
				{ rect.x, rect.y, 0.5f, m_Color, u1, v1 }
			};

			m_Buffer.Add(verts, ARRAYSIZE(verts));
		}

		void DirectX11::LoadTexture(Gwen::Texture* pTexture)
		{
			HRESULT hr = S_OK;
			ID3D11ShaderResourceView* pTex = NULL;
			HRESULT retHR;
			D3DX11_IMAGE_INFO imgInfo;

			retHR = D3DX11GetImageInfoFromFileW(pTexture->name.GetUnicode().c_str(), NULL, &imgInfo, NULL);
			hr = D3DX11CreateShaderResourceViewFromFileW(m_pDevice, pTexture->name.GetUnicode().c_str(), NULL, NULL, &pTex, NULL);
			
			if (hr != S_OK || retHR != S_OK)
			{
				return;
			}

			pTexture->data = pTex;
			pTexture->width = imgInfo.Width;
			pTexture->height = imgInfo.Height;
		}

		void DirectX11::FreeTexture(Gwen::Texture* pTexture)
		{
			ID3D11ShaderResourceView* pImage = (ID3D11ShaderResourceView*)pTexture->data;
			if (!pImage)
				return;

			SafeRelease(pImage);
			pTexture->data = NULL;
		}

		Gwen::Color DirectX11::PixelColour(Gwen::Texture* pTexture, unsigned int x, unsigned int y, const Gwen::Color & col_default)
		{
			ID3D11ShaderResourceView* pImage = (ID3D11ShaderResourceView*)pTexture->data;
			if (!pImage)
				return col_default;

			ID3D11Texture2D *t = NULL;
			pImage->GetResource(reinterpret_cast<ID3D11Resource**>(&t));
			
			// We have to create a staging texture to copy the texture to, because textures cannot
			// be used as shader resources and have CPU read access at the same time.

			// Furthermore, in DX11 this texture must already exist and have the same exact dimensions as the
			// source texture. So we have to create and destroy it every time, without prior knowledge of the size
			// of the incoming texture. Obviously this is really expensive.
			ID3D11Texture2D* stagingTexture = NULL;

			DXGI_SAMPLE_DESC sampleDesc = { 1, 0 };

			D3D11_TEXTURE2D_DESC tdesc;
			tdesc.Width = pTexture->width;
			tdesc.Height = pTexture->height;
			tdesc.MipLevels = 0;
			tdesc.ArraySize = 1;
			tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			tdesc.SampleDesc = sampleDesc;
			tdesc.Usage = D3D11_USAGE_STAGING;
			tdesc.BindFlags = 0;
			tdesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			tdesc.MiscFlags = 0;
			
			if (FAILED(m_pDevice->CreateTexture2D(&tdesc, NULL, &stagingTexture)))
			{
				return col_default;
			}

			// Copy the data over to the staging texture
			m_pContext->CopyResource(stagingTexture, t);

			D3D11_MAPPED_SUBRESOURCE msr;
			if (FAILED(m_pContext->Map(stagingTexture, 0, D3D11_MAP_READ, 0, &msr)))
			{
				SafeRelease(stagingTexture);
				SafeRelease(t);
				return col_default;
			}

			DWORD* pixels = (DWORD*)msr.pData;
			DWORD color = pixels[msr.RowPitch / sizeof(DWORD) * y + x];
			m_pContext->Unmap(stagingTexture, 0);

			stagingTexture->Release();
			t->Release();

			DWORD a = ((255u << 24u) & color) >> 24u;
			DWORD r = ((255u << 16u) & color) >> 16u;
			DWORD g = ((255u << 8u) & color) >> 8u;
			DWORD b = ((255u) & color);

			return Gwen::Color(r, g, b, a);
		}
#pragma region Experimental
		void DirectX11::FillPresentParameters(Gwen::WindowProvider* pWindow, DXGI_SWAP_CHAIN_DESC & Params)
		{
			HWND pHWND = (HWND)pWindow->GetWindow();
			RECT ClientRect;
			GetClientRect(pHWND, &ClientRect);
			ZeroMemory(&Params, sizeof(Params));


			UINT WIDTH = ClientRect.right - ClientRect.left;
			UINT HEIGHT = ClientRect.bottom - ClientRect.top;

			Params.BufferCount = 1;
			Params.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			Params.OutputWindow = pHWND;
			Params.SampleDesc.Count = 1;
			Params.SampleDesc.Quality = 0;
			Params.Windowed = TRUE;

			Params.BufferDesc.RefreshRate = { 1, 60 };
			Params.BufferDesc.Width = WIDTH;
			Params.BufferDesc.Height = HEIGHT;
			Params.BufferDesc.Format = DXGI_FORMAT_B8G8R8X8_UNORM;
			Params.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		}

		bool DirectX11::InitializeContext(Gwen::WindowProvider* pWindow)
		{
			HWND pHWND = (HWND)pWindow->GetWindow();
			RECT ClientRect;
			GetClientRect(pHWND, &ClientRect);
			
			UINT WIDTH = ClientRect.right - ClientRect.left;
			UINT HEIGHT = ClientRect.bottom - ClientRect.top;

			D3D_DRIVER_TYPE driverTypes[] =
			{
				D3D_DRIVER_TYPE_HARDWARE,
				D3D_DRIVER_TYPE_WARP,
				D3D_DRIVER_TYPE_REFERENCE,
			};
			UINT numDriverTypes = ARRAYSIZE(driverTypes);

			D3D_FEATURE_LEVEL featureLevels[] =
			{
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0,
				D3D_FEATURE_LEVEL_9_3,
				D3D_FEATURE_LEVEL_9_2,
				D3D_FEATURE_LEVEL_9_1
			};
			UINT numFeatureLevels = ARRAYSIZE(featureLevels);

			
			DXGI_SWAP_CHAIN_DESC Params;
			FillPresentParameters(pWindow, Params);
			HRESULT hr = S_OK;
			for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
			{
				D3D_DRIVER_TYPE driverType = driverTypes[driverTypeIndex];
				hr = D3D11CreateDeviceAndSwapChain(NULL, driverType, NULL, D3D11_CREATE_DEVICE_SINGLETHREADED, featureLevels, numFeatureLevels,
					D3D11_SDK_VERSION, &Params, &m_pSwapChain, &m_pDevice, NULL, &m_pContext);
				if (SUCCEEDED(hr))
					break;
			}
			if (FAILED(hr))
				return false;
			
			ID3D11Texture2D* pBackBuffer = NULL;
			hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			if (FAILED(hr))
			{
				ShutdownContext(pWindow);
				return false;
			}

			ID3D11RenderTargetView* pRenderTargetView;
			hr = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView);
			pBackBuffer->Release();
			if (FAILED(hr))
			{
				ShutdownContext(pWindow);
				return false;
			}

			m_pContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);
			SafeRelease(pRenderTargetView);

			D3D11_VIEWPORT vp;
			width = vp.Width = (FLOAT)WIDTH;
			height = vp.Height = (FLOAT)HEIGHT;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;

			m_pContext->RSSetViewports(1, &vp);
			
			return true;
		}

		bool DirectX11::ShutdownContext(Gwen::WindowProvider* pWindow)
		{
			ID3D11RenderTargetView* pRenderTargetView;
			m_pContext->OMGetRenderTargets(1, &pRenderTargetView, 0);
			SafeRelease(pRenderTargetView);
			SafeRelease(m_pSwapChain);
			SafeRelease(m_pContext);
			SafeRelease(m_pDevice);
			return true;
		}

		bool DirectX11::PresentContext(Gwen::WindowProvider* pWindow)
		{
			m_pSwapChain->Present(1, 0);
			return true;
		}

		bool DirectX11::ResizedContext(Gwen::WindowProvider* pWindow, int w, int h)
		{
			HWND pHWND = (HWND)pWindow->GetWindow();
			RECT ClientRect;
			GetClientRect(pHWND, &ClientRect);

			ID3D11RenderTargetView* pRenderTargetView;
			m_pContext->OMGetRenderTargets(1, &pRenderTargetView, 0);
			SafeRelease(pRenderTargetView);

			m_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

			ID3D11Texture2D* pBuffer;
			m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
				(void**)&pBuffer);

			m_pDevice->CreateRenderTargetView(pBuffer, NULL,
				&pRenderTargetView);
			// Perform error handling here!
			SafeRelease(pBuffer);

			m_pContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);
			SafeRelease(pRenderTargetView);

			UINT WIDTH = ClientRect.right - ClientRect.left;
			UINT HEIGHT = ClientRect.bottom - ClientRect.top;

			D3D11_VIEWPORT vp;
			width = vp.Width = (FLOAT)WIDTH;
			height = vp.Height = (FLOAT)HEIGHT;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;

			m_pContext->RSSetViewports(1, &vp);
			return true;
		}

		bool DirectX11::BeginContext(Gwen::WindowProvider* pWindow)
		{
			float ClearColor[4] = { 0.128f, 0.128f, 0.128, 1.0f }; // red,green,blue,alpha


			ID3D11RenderTargetView* pRenderTargetView;
			m_pContext->OMGetRenderTargets(1, &pRenderTargetView, 0);
			m_pContext->ClearRenderTargetView(pRenderTargetView, ClearColor);
			pRenderTargetView->Release();
			return true;
		}

		bool DirectX11::EndContext(Gwen::WindowProvider* pWindow)
		{
			return true;
		}

#pragma endregion
	}
}