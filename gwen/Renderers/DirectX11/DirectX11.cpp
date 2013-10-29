
#include "Gwen/Renderers/DirectX11.h"
#include "Gwen/Utility.h"
#include "Gwen/Font.h"
#include "Gwen/Texture.h"
#include "Gwen/WindowProvider.h"

#include <d3dcompiler.h>

#include <memory>
#include <exception>
#include <type_traits>
#include <cassert>

#include "WICTextureLoader.h"

#define D3DCOLOR_ARGB(a, r, g, b) ((uint32_t)((((a) &0xff) <<24)|(((b) &0xff) <<16)|(((g) &0xff) <<8)|((r) &0xff)))
#define SAFE_RELEASE(x) {if(x) x->Release(); x=0; }
#define RED   0x1
#define GREEN 0x2
#define BLUE  0x4
#define ALPHA 0x8
#define ALL (RED | GREEN | BLUE | ALPHA)

const std::string GwenUIVertexShaderSrc = std::string(" \n\
struct VSStruct                                      \n\
{                                                    \n\
    float2 position : POSITION;                      \n\
    float4 color : COLOR;                            \n\
    float2 uv : TEXCOORD;                            \n\
};                                                   \n\
                                                     \n\
struct PSstruct                                      \n\
{                                                    \n\
    float4 position : SV_Position;                   \n\
    float4 color    : COLOR;                         \n\
    float2 uv       : TexCoord;                      \n\
};                                                   \n\
                                                     \n\
PSstruct VS( VSStruct In)                            \n\
{                                                    \n\
    PSstruct Out;                                    \n\
    Out.position = float4(In.position.x, In.position.y, 1, 1);        \n\
    Out.color = In.color;                            \n\
    Out.uv = In.uv;                                  \n\
    return Out;                                      \n\
}");

const std::string GwenUIPixelShaderColSrc = std::string(" \n\
struct PSstruct                                           \n\
{                                                         \n\
    float4 position : SV_Position;                        \n\
    float4 color    : COLOR;                              \n\
    float2 uv       : TexCoord;                           \n\
};                                                        \n\
                                                          \n\
float4 PSCol(in PSstruct In ) : SV_Target0                \n\
{                                                         \n\
    return In.color;                                      \n\
}");

const std::string GwenUIPixelShaderTexSrc = std::string(" \n\
struct PSstruct                                           \n\
{                                                         \n\
    float4 position : SV_Position;                        \n\
    float4 color    : COLOR;                              \n\
    float2 uv       : TexCoord;                           \n\
};                                                        \n\
                                                          \n\
SamplerState LinearFilter                                 \n\
{                                                         \n\
    AddressU = CLAMP;                                     \n\
    AddressV = CLAMP;                                     \n\
    AddressW = CLAMP;                                     \n\
    Filter = MIN_MAG_MIP_LINEAR;                          \n\
};                                                        \n\
                                                          \n\
Texture2D UITexture;                                      \n\
                                                          \n\
float4 PSTex(in PSstruct In ) : SV_Target0                \n\
{                                                         \n\
    return UITexture.Sample( LinearFilter, In.uv );       \n\
}");

namespace DirectX
{
    inline HANDLE safe_handle( HANDLE h ) { return (h == INVALID_HANDLE_VALUE) ? 0 : h; }

    inline void AssertIfFailed(HRESULT hr)
    {
        assert(SUCCEEDED(hr));
    }

    struct handle_closer { void operator()(HANDLE h) { if (h) CloseHandle(h); } };
    typedef public std::unique_ptr<void, handle_closer> ScopedHandle;

    // Helper for reading binary data, either from the filesystem a memory buffer.
    class BinaryReader
    {
    public:

        explicit BinaryReader(_In_z_ wchar_t const* fileName)
        {
            size_t dataSize;

            AssertIfFailed(ReadEntireFile(fileName, mOwnedData, &dataSize));

            mPos = mOwnedData.get();
            mEnd = mOwnedData.get() + dataSize;
        }

        BinaryReader(_In_reads_bytes_(dataSize) uint8_t const* dataBlob, size_t dataSize)
        {
            mPos = dataBlob;
            mEnd = dataBlob + dataSize;
        }

        
        // Reads a single value.
        template<typename T> T const& Read()
        {
            return *ReadArray<T>(1);
        }


        // Reads an array of values.
        template<typename T> T const* ReadArray(size_t elementCount)
        {
            static_assert(std::is_pod<T>::value, "Can only read plain-old-data types");

            uint8_t const* newPos = mPos + sizeof(T) * elementCount;

            if (newPos > mEnd)
                throw std::exception("End of file");

            auto result = reinterpret_cast<T const*>(mPos);

            mPos = newPos;

            return result;
        }


        // Lower level helper reads directly from the filesystem into memory.
        static HRESULT ReadEntireFile(_In_z_ wchar_t const* fileName, _Inout_ std::unique_ptr<uint8_t[]>& data, _Out_ size_t* dataSize)
        {
            //ScopedHandle hFile(safe_handle(CreateFile2(fileName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr)));

            ScopedHandle hFile(safe_handle(CreateFile(
                fileName,
                GENERIC_READ,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL)));

            if (!hFile) return HRESULT_FROM_WIN32(GetLastError());

            // Get the file size.
            LARGE_INTEGER fileSize = { 0 };

            FILE_STANDARD_INFO fileInfo;

            if (!GetFileInformationByHandleEx(hFile.get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
            {
                return HRESULT_FROM_WIN32(GetLastError());
            }

            fileSize = fileInfo.EndOfFile;

            // File is too big for 32-bit allocation, so reject read.
            if (fileSize.HighPart > 0) return E_FAIL;

            // Create enough space for the file data.
            data.reset(new uint8_t[fileSize.LowPart]);

            if (!data) return E_OUTOFMEMORY;

            // Read the data in.
            DWORD bytesRead = 0;

            if (!ReadFile(hFile.get(), data.get(), fileSize.LowPart, &bytesRead, nullptr))
            {
                return HRESULT_FROM_WIN32(GetLastError());
            }

            if (bytesRead < fileSize.LowPart)
                return E_FAIL;

            *dataSize = bytesRead;
    
            return S_OK;
        }


    private:
        // The data currently being read.
        uint8_t const* mPos;
        uint8_t const* mEnd;

        std::unique_ptr<uint8_t[]> mOwnedData;


        // Prevent copying.
        BinaryReader(BinaryReader const&);
        BinaryReader& operator= (BinaryReader const&);
    };

    ID3D10Blob* CompileShaderToBytecode(const std::string& source, const std::string& name, const std::string& entryPoint, const std::string& target)
    {
        ID3D10Blob* errorMessage = NULL;
        ID3D10Blob* bytecodeBuffer = NULL;
        HRESULT hr = D3DCompile2(
            source.c_str(),
            source.size(),
            name.c_str(),
            NULL,
            NULL,
            entryPoint.c_str(),
            target.c_str(),
            D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_WARNINGS_ARE_ERRORS,
            0,
            0,
            NULL,
            0,
            &bytecodeBuffer,
            &errorMessage);

        if(errorMessage)
        {
            printf("Error when compiling shader source: %s\n", errorMessage->GetBufferPointer());
            errorMessage->Release();
        }

        AssertIfFailed(hr);

        return bytecodeBuffer;
    }
    ID3D11VertexShader* CompileVertexShaderFromMemory(ID3D11Device* device, const std::string& source, const std::string& name, const std::string& entryPoint, ID3D10Blob*& outByteCode)
    {
        ID3D10Blob* bytecodeBuffer = CompileShaderToBytecode(source, name, entryPoint, "vs_5_0");
        ID3D11VertexShader* vShader = NULL;
        AssertIfFailed(device->CreateVertexShader(bytecodeBuffer->GetBufferPointer(),bytecodeBuffer->GetBufferSize(),NULL,&vShader));
        outByteCode = bytecodeBuffer;
        return vShader;
    }
    ID3D11PixelShader* CompilePixelShaderFromMemory(ID3D11Device* device, const std::string& source, const std::string& name, const std::string& entryPoint)
    {
        ID3D10Blob* bytecodeBuffer = CompileShaderToBytecode(source, name, entryPoint, "ps_5_0");
        ID3D11PixelShader* pShader = NULL;
        AssertIfFailed(device->CreatePixelShader(bytecodeBuffer->GetBufferPointer(),bytecodeBuffer->GetBufferSize(),NULL,&pShader));
        bytecodeBuffer->Release();
        return pShader;
    }
}

namespace Gwen
{
    namespace Renderer
    {
        DirectX11::DirectX11( ID3D11Device* device, ID3D11DeviceContext* dc, float width, float height)
        {
            m_device = device;
            m_dc = dc;
            m_width = width;
            m_height = height;
            m_iVertNum = 0;
            m_pInputLayout = NULL;
            m_pUIVertexShader = NULL;
            m_pUIPixelShaderCol = NULL;
            m_pUIPixelShaderTex = NULL;

            m_pUITexture = NULL;
            m_pUITexSRV = NULL;
            m_pUIVertexBuffer = NULL;
            m_pUIVertexBufferSRV = NULL;
            m_pUIBlendState = NULL;

            m_pUIRasterizerStateNormal = NULL;
            m_pUIRasterizerStateScissor = NULL;

            // Initialize the COM library so we can load textures with WIC.
            DirectX::AssertIfFailed(CoInitializeEx(NULL,COINIT_MULTITHREADED));

            // Compile and create shader objects
            ID3D10Blob* vsBytecode = NULL;
            m_pUIVertexShader = DirectX::CompileVertexShaderFromMemory(m_device,GwenUIVertexShaderSrc, "UI Vertex Shader", "VS", vsBytecode);
            m_pUIPixelShaderCol = DirectX::CompilePixelShaderFromMemory(m_device,GwenUIPixelShaderColSrc, "UI Pixel Shader Color", "PSCol");
            m_pUIPixelShaderTex = DirectX::CompilePixelShaderFromMemory(m_device,GwenUIPixelShaderTexSrc, "UI Pixel Shader Texture", "PSTex");

            const D3D11_INPUT_ELEMENT_DESC layout[] =
            {
                { "POSITION",   0, DXGI_FORMAT_R32G32_FLOAT,    0,  0,                              D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "COLOR",      0, DXGI_FORMAT_R8G8B8A8_UNORM,  0,  8,                              D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,    0,  12,                             D3D11_INPUT_PER_VERTEX_DATA, 0 }
            };

            size_t numElements = sizeof( layout ) / sizeof( layout[0] );

            void* bufferPointer = vsBytecode->GetBufferPointer();
            size_t bufferSize = vsBytecode->GetBufferSize();

            DirectX::AssertIfFailed(m_device->CreateInputLayout(layout,numElements,bufferPointer,bufferSize,&m_pInputLayout));

            vsBytecode->Release();
            
            // Create an empty, dynamic vertex buffer
            D3D11_BUFFER_DESC vbdesc;
            vbdesc.ByteWidth = MaxVerts * sizeof( VertexFormat );
            vbdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            vbdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            vbdesc.Usage = D3D11_USAGE_DYNAMIC;
            vbdesc.MiscFlags = 0;
            DirectX::AssertIfFailed(m_device->CreateBuffer( &vbdesc, 0, &m_pUIVertexBuffer));
        
            // Create a rast state to cull none and fill solid. Also create the scissor rast state
            D3D11_RASTERIZER_DESC descras;
            descras.CullMode = (D3D11_CULL_MODE) D3D11_CULL_NONE;
            descras.FillMode = (D3D11_FILL_MODE) D3D11_FILL_SOLID;
            descras.FrontCounterClockwise = FALSE;
            descras.DepthBias = 0;
            descras.DepthBiasClamp = 0.0f;
            descras.SlopeScaledDepthBias = 0.0f;
            descras.AntialiasedLineEnable = FALSE;
            descras.DepthClipEnable = FALSE;
            descras.MultisampleEnable = false;
            descras.ScissorEnable = false;
            DirectX::AssertIfFailed(m_device->CreateRasterizerState(&descras, &m_pUIRasterizerStateNormal));
            descras.ScissorEnable = true;
            DirectX::AssertIfFailed(m_device->CreateRasterizerState(&descras, &m_pUIRasterizerStateScissor));

            D3D11_BLEND_DESC blenddesc;
            blenddesc.AlphaToCoverageEnable = false;
            blenddesc.IndependentBlendEnable = false;
            memset(&blenddesc.RenderTarget, 0, sizeof(blenddesc.RenderTarget));
            blenddesc.RenderTarget[0].BlendEnable = true;
            blenddesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            blenddesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            blenddesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            blenddesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
            blenddesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            blenddesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
            blenddesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            DirectX::AssertIfFailed(m_device->CreateBlendState(&blenddesc, &m_pUIBlendState));

            D3D11_DEPTH_STENCIL_DESC depthdesc;
            memset(&depthdesc, 0, sizeof(depthdesc));
            depthdesc.DepthEnable = false;
            depthdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
            depthdesc.DepthFunc = D3D11_COMPARISON_NEVER;
            depthdesc.StencilEnable = false;
            DirectX::AssertIfFailed(m_device->CreateDepthStencilState(&depthdesc, &m_pUIDepthState));
        }

        DirectX11::~DirectX11()
        {
            SAFE_RELEASE(m_pInputLayout);
            SAFE_RELEASE(m_pUIVertexShader);
            SAFE_RELEASE(m_pUIPixelShaderCol);
            SAFE_RELEASE(m_pUIPixelShaderTex);
            SAFE_RELEASE(m_pUIVertexBuffer);
            SAFE_RELEASE(m_pUIBlendState);
            SAFE_RELEASE(m_pUIDepthState);
            SAFE_RELEASE(m_pUIRasterizerStateScissor);
            SAFE_RELEASE(m_pUIRasterizerStateNormal);
        }

        void DirectX11::Begin()
        {
            // Save current state
            m_dc->OMGetBlendState(&m_pUILastBlendState, m_LastBlendFactor, &m_LastBlendMask);
            m_dc->RSGetState(&m_pUILastRasterizerState);
            m_dc->OMGetDepthStencilState(&m_LastDepthState, &m_LastStencilRef);
            m_dc->IAGetInputLayout(&m_LastInputLayout);
            m_dc->IAGetPrimitiveTopology(&m_LastTopology);
            m_dc->IAGetVertexBuffers(0, 8, m_LastBuffers, m_LastStrides, m_LastOffsets);
            m_dc->PSGetShader(&m_LastPSShader, NULL, 0);
            m_dc->GSGetShader(&m_LastGSShader, NULL, 0);
            m_dc->VSGetShader(&m_LastVSShader, NULL, 0);

            // Set required states
            float factor[4] = {0, 0, 0, 0};
            m_dc->OMSetBlendState(m_pUIBlendState, factor, ~0);
            m_dc->OMSetDepthStencilState(m_pUIDepthState, 0);
            m_dc->RSSetState( m_pUIRasterizerStateNormal);

            // Set initial shaders
            m_amTextured = false;
            m_dc->VSSetShader(m_pUIVertexShader, NULL, 0);
            m_dc->PSSetShader(m_pUIPixelShaderCol, NULL, 0);
        }

        void DirectX11::End()
        {
            Flush();

            // Restore previous state
            m_dc->OMSetBlendState(m_pUILastBlendState, m_LastBlendFactor, m_LastBlendMask);
            m_dc->RSSetState(m_pUILastRasterizerState);
            m_dc->OMSetDepthStencilState(m_LastDepthState, m_LastStencilRef);
            m_dc->IASetInputLayout(m_LastInputLayout);
            m_dc->IASetPrimitiveTopology(m_LastTopology);
            m_dc->IASetVertexBuffers(0, 8, m_LastBuffers, m_LastStrides, m_LastOffsets);
            m_dc->PSSetShader(m_LastPSShader, NULL, 0);
            m_dc->GSSetShader(m_LastGSShader, NULL, 0);
            m_dc->VSSetShader(m_LastVSShader, NULL, 0);
        }

        void DirectX11::Flush()
        {
            if ( m_iVertNum > 0 )
            {
                // Update the vertex buffer
                D3D11_MAPPED_SUBRESOURCE msr;
                DirectX::AssertIfFailed(m_dc->Map(m_pUIVertexBuffer,0,D3D11_MAP_WRITE_DISCARD,0,&msr));
                memcpy(msr.pData, m_pVerts , m_iVertNum*sizeof(VertexFormat));
                m_dc->Unmap(m_pUIVertexBuffer,0);

                // Set topology to triangle list
                m_dc->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                m_dc->IASetInputLayout(m_pInputLayout);
                UINT stride [] = { sizeof(VertexFormat) };
                UINT offset [] = { 0 };
                // Set the vertex buffer
                m_dc->IASetVertexBuffers(0, 1, &m_pUIVertexBuffer, stride, offset);

                // Draw
                m_dc->Draw(m_iVertNum, 0);
                m_iVertNum = 0;
            }
        }

        void DirectX11::AddVert( int x, int y )
        {
            if ( m_iVertNum >= MaxVerts - 1 )
            {
                Flush();
            }

            m_pVerts[ m_iVertNum ].x = (2.0f*(float)x/m_width) -1.0f;
            m_pVerts[ m_iVertNum ].y = (-2.0f*(float)y/m_height) +1.0f;
            m_pVerts[ m_iVertNum ].color = m_color;
            ++m_iVertNum;
        }

        void DirectX11::AddVert( int x, int y, float u, float v )
        {
            if ( m_iVertNum >= MaxVerts - 1 )
            {
                Flush();
            }

            m_pVerts[ m_iVertNum ].x = (2.0f*(float)x/m_width) -1.0f;
            m_pVerts[ m_iVertNum ].y = (-2.0f*(float)y/m_height) +1.0f;
            m_pVerts[ m_iVertNum ].u = u;
            m_pVerts[ m_iVertNum ].v = v;
            m_pVerts[ m_iVertNum ].color = m_color;
            m_iVertNum++;
        }

        void DirectX11::DrawFilledRect( Gwen::Rect rect )
        {
            if ( m_amTextured || m_currentTexture != NULL )
            {
                Flush();
                ID3D11ShaderResourceView* null = NULL;
                m_dc->PSSetShaderResources(0,1,&null);
                m_currentTexture = NULL;
                m_dc->PSSetShader(m_pUIPixelShaderCol,NULL,0);
                m_amTextured = false;
            }

            Translate( rect );

            AddVert( rect.x, rect.y );
            AddVert( rect.x+rect.w, rect.y );
            AddVert( rect.x, rect.y + rect.h );

            AddVert( rect.x+rect.w, rect.y );
            AddVert( rect.x+rect.w, rect.y+rect.h );
            AddVert( rect.x, rect.y + rect.h );
        }

        void DirectX11::SetDrawColor( Gwen::Color color )
        {
            m_color = D3DCOLOR_ARGB( color.a, color.r, color.g, color.b );
        }

        void DirectX11::LoadFont( Gwen::Font* font )
        {
            FontData* data = new FontData();

            m_fontList.push_back( font );
            // Scale the font according to canvas
            font->realsize = font->size * Scale();

            // Load the font data from the file
            Gwen::UnicodeString fileName = font->facename + L".sfnt";

            DirectX::BinaryReader bReader(fileName.c_str());

            // Validate the header.
            for (char const* magic = "DXTKfont"; *magic; magic++)
            {
                if (bReader.Read<uint8_t>() != *magic)
                {
                    assert("This file is not a DirectXTK SpriteFont file!" && 0);
                }
            }

            // Read the glyph data.
            auto glyphCount = bReader.Read<uint32_t>();
            auto glyphData = bReader.ReadArray<FontData::Glyph>(glyphCount);

            data->charset.assign(glyphData, glyphData + glyphCount);

            // Read font properties.
            data->linespacing = bReader.Read<float>();

            m_defaultCharacter = ((wchar_t)bReader.Read<uint32_t>());

            // Read the texture data.
            auto textureWidth = bReader.Read<uint32_t>();
            auto textureHeight = bReader.Read<uint32_t>();
            auto textureFormat = bReader.Read<DXGI_FORMAT>();
            auto textureStride = bReader.Read<uint32_t>();
            auto textureRows = bReader.Read<uint32_t>();
            auto textureData = bReader.ReadArray<uint8_t>(textureStride * textureRows);

            // Create the D3D texture.
            CD3D11_TEXTURE2D_DESC textureDesc(
                textureFormat, 
                textureWidth, 
                textureHeight, 
                1, 
                1, 
                D3D11_BIND_SHADER_RESOURCE, 
                D3D11_USAGE_IMMUTABLE);

            CD3D11_SHADER_RESOURCE_VIEW_DESC viewDesc(
                D3D11_SRV_DIMENSION_TEXTURE2D,
                textureFormat);

            D3D11_SUBRESOURCE_DATA initData = { textureData, textureStride };

            ID3D11Texture2D* texture;
            ID3D11ShaderResourceView* srv;

            DirectX::AssertIfFailed(m_device->CreateTexture2D(&textureDesc, &initData, &texture));
            DirectX::AssertIfFailed(m_device->CreateShaderResourceView(texture, &viewDesc, &srv));

            data->texture = texture;
            data->srv = srv;

            font->data = (void*)data;
        }

        void DirectX11::FreeFont( Gwen::Font* pFont )
        {
            m_fontList.remove( pFont );

            if ( !pFont->data ) { return; }

            FontData* pFontData = ( FontData* ) pFont->data;

            SAFE_RELEASE(pFontData->texture);
            SAFE_RELEASE(pFontData->srv);

            delete pFontData;
            pFont->data = NULL;
        }

        void DirectX11::RenderText( Gwen::Font* pFont, Gwen::Point pos, const Gwen::UnicodeString & text )
        {
            Flush();

            // If the font doesn't exist, or the font size should be changed
            if ( !pFont->data || fabs( pFont->realsize - pFont->size * Scale() ) > 2 )
            {
                FreeFont( pFont );
                LoadFont( pFont );
            }

            FontData* pFontData = ( FontData* ) pFont->data;
            Translate( pos.x, pos.y );
            RECT ClipRect = { pos.x, pos.y, 0, 0 };
            //pFontData->pFont->DrawTextW( NULL, text.c_str(), -1, &ClipRect, DT_LEFT | DT_TOP | DT_NOCLIP | DT_SINGLELINE, m_Color );
        }

        Gwen::Point DirectX11::MeasureText( Gwen::Font* pFont, const Gwen::UnicodeString & text )
        {
            // If the font doesn't exist, or the font size should be changed
            if ( !pFont->data || fabs( pFont->realsize - pFont->size * Scale() ) > 2 )
            {
                FreeFont( pFont );
                LoadFont( pFont );
            }

            //FontData* pFontData = ( FontData* ) pFont->data;
            //Gwen::Point size;

            //if ( text.empty() )
            //{
            //	RECT rct = {0, 0, 0, 0};
            //	pFontData->pFont->DrawTextW( NULL, L"W", -1, &rct, DT_CALCRECT, 0 );
            //	return Gwen::Point( 0, rct.bottom );
            //}

            RECT rct = {0, 0, 0, 0};
            //pFontData->pFont->DrawTextW( NULL, text.c_str(), -1, &rct, DT_CALCRECT | DT_LEFT | DT_TOP | DT_SINGLELINE, 0 );

            //for ( int i = text.length() - 1; i >= 0 && text[i] == L' '; i-- )
            //{
            //	rct.right += pFontData->iSpaceWidth;
            //}

            return Gwen::Point( rct.right / Scale(), rct.bottom / Scale() );
        }

        void DirectX11::StartClip()
        {
            Flush();

            m_dc->RSSetState( m_pUIRasterizerStateScissor );

            const Gwen::Rect& rect = ClipRegion();

            D3D11_RECT r;

            r.left = ceil( ((float)rect.x) * Scale() );
            r.right = ceil(((float)(rect.x + rect.w)) * Scale()) + 1;
            r.top = ceil( (float)rect.y * Scale() );
            r.bottom = ceil( ((float)(rect.y + rect.h)) * Scale() ) + 1;

            m_dc->RSSetScissorRects(1, &r );
        }

        void DirectX11::EndClip()
        {
            Flush();
            m_dc->RSSetState(m_pUIRasterizerStateNormal);
        }

        void DirectX11::DrawTexturedRect( Gwen::Texture* pTexture, Gwen::Rect rect, float u1, float v1, float u2, float v2 )
        {
            ID3D11ShaderResourceView* pImage = (ID3D11ShaderResourceView*) pTexture->data;

            // Missing image, not loaded properly?
            if ( !pImage )
            {
                return DrawMissingImage( rect );
            }

            Translate( rect );

            if(!m_amTextured)
            {
                Flush();
                m_dc->PSSetShader(m_pUIPixelShaderTex, NULL, 0);
                m_amTextured = true;
            }

            if(m_currentTexture != pImage)
            {
                m_dc->PSSetShaderResources(0,1,&pImage);
                m_currentTexture = pImage;
            }

            AddVert( rect.x, rect.y,            u1, v1 );
            AddVert( rect.x+rect.w, rect.y,     u2, v1 );
            AddVert( rect.x, rect.y + rect.h,   u1, v2 );

            AddVert( rect.x+rect.w, rect.y,     u2, v1 );
            AddVert( rect.x+rect.w, rect.y+rect.h, u2, v2 );
            AddVert( rect.x, rect.y + rect.h, u1, v2 );
        }

        void DirectX11::LoadTexture( Gwen::Texture* pTexture )
        {
            ID3D11Resource* texture = NULL;
            ID3D11ShaderResourceView* srv = NULL;

            uint32_t width = 0;
            uint32_t height = 0;

            HRESULT hr = CreateWICTextureFromFile(
                m_device,
                m_dc,
                pTexture->name.GetUnicode().c_str(),
                &texture,
                &srv,
                width,
                height);

            if(FAILED(hr))
            {
                pTexture->failed = true;
            }
            else
            {
                pTexture->data = srv;
                pTexture->width = width;
                pTexture->height = height;
            }
        }

        void DirectX11::FreeTexture( Gwen::Texture* pTexture )
        {
            ID3D11ShaderResourceView* pImage = (ID3D11ShaderResourceView*) pTexture->data;
            if ( !pImage ) return;
            
            ID3D11Resource* resource = NULL;
            
            pImage->GetResource(&resource);

            if(resource)
            {
                resource->Release();
            }

            pImage->Release();
            pTexture->data = NULL;
            return;
        }

        Gwen::Color DirectX11::PixelColour( Gwen::Texture* pTexture, unsigned int x, unsigned int y, const Gwen::Color & col_default )
        {
            ID3D11ShaderResourceView* pImage = (ID3D11ShaderResourceView*) pTexture->data;
            if ( !pImage ) return col_default;

            ID3D11Texture2D *t;
            pImage->GetResource(reinterpret_cast<ID3D11Resource**>(&t));

            // We have to create a staging texture to copy the texture to, because textures cannot
            // be used as shader resources and have CPU read access at the same time.

            // Furthermore, in DX11 this texture must already exist and have the same exact dimensions as the
            // source texture. So we have to create and destroy it every time, without prior knowledge of the size
            // of the incoming texture. Obviously this is really expensive.
            ID3D11Texture2D* stagingTexture = NULL;

            DXGI_SAMPLE_DESC sampleDesc = {1,0};

            D3D11_TEXTURE2D_DESC tdesc;
            tdesc.Width= pTexture->width;
            tdesc.Height = pTexture->height;
            tdesc.MipLevels = 0;
            tdesc.ArraySize = 1;
            tdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            tdesc.SampleDesc = sampleDesc;
            tdesc.Usage = D3D11_USAGE_STAGING;
            tdesc.BindFlags = 0;
            tdesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            tdesc.MiscFlags = 0;

            DirectX::AssertIfFailed(m_device->CreateTexture2D(&tdesc,NULL,&stagingTexture));

            // Copy the data over to the staging texture
            m_dc->CopyResource(stagingTexture, t);

            D3D11_MAPPED_SUBRESOURCE msr;
            DirectX::AssertIfFailed(m_dc->Map(stagingTexture, 0, D3D11_MAP_READ, 0,  &msr));

            uint32_t* pixels = (uint32_t*)msr.pData;
            uint32_t color = pixels[msr.RowPitch / sizeof(uint32_t) * y + x];
            m_dc->Unmap(stagingTexture,0);

            stagingTexture->Release();
            t->Release();

            uint8_t a = ((255u << 24u) & color) >> 24u;
            uint8_t r = ((255u << 16u) & color) >> 16u;
            uint8_t g = ((255u << 8u) & color) >> 8u;
            uint8_t b = ((255u) & color);

            return Gwen::Color(r, g, b, a);
        }

        void DirectX11::Release()
        {
            Font::List::iterator it = m_fontList.begin();

            while ( it != m_fontList.end() )
            {
                FreeFont( *it );
                it = m_fontList.begin();
            }
        }
    }
}