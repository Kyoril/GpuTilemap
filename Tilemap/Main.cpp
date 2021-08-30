#include <cassert>
#include <cstdint>
#include <thread>
#include <memory>

#include "GraphicsDevice.h"
#include "GraphicsContext.h"
#include "PngLoader.h"
#include "ComUtils.h"

#include <Windows.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

// Common resources
static std::unique_ptr<GraphicsDevice> s_device;
static RenderWindow* s_renderTarget = nullptr;

// Tilemap specific resources
static VertexShader* s_tilemapVs = nullptr;
static PixelShader* s_tilemapPs = nullptr;
static ConstantBuffer* s_vsPerObject = nullptr;
static ConstantBuffer* s_vsPerFrame = nullptr;
static ConstantBuffer* s_psPerObject = nullptr;
static VertexBuffer* s_tilemapVBuf = nullptr;
static IndexBuffer* s_tilemapIBuf = nullptr;
static Texture* s_tilemap = nullptr;
static Texture* s_tileset = nullptr;

// Tilemap specific globals
static float scale = 2.0f;

// Common globals
static volatile bool s_terminate = false;

/// @brief Represents a single vertex which is sent to the gpu.
struct VERTEX
{
    float X, Y, Z;    // vertex position
	float u, v;
	uint32_t id;
};


DirectX::XMMATRIX s_matrices[3];
struct PSPerObject
{
	float inverseTilesetTextureSize[2];
	float inverseTilemapTextureSize[2];
	float tileSize;

	float padding[3]{};
} s_psPerObjectData{};
struct VSPerFrame
{
	float viewOffset[2];
	float viewportSize[2];
	float inverseTileTextureSize[2];
	float inverseTileSize;
		
	float padding{};
} s_vsPerFrameData{};

/// @brief Loads resources required for this specific sample.
void LoadResources()
{
#include "VS_Tilemap.h"
	s_tilemapVs = s_device->CreateVertexShader(vs_tilemap_code, ARRAYSIZE(vs_tilemap_code));
	
#include "PS_Tilemap.h"
	s_tilemapPs = s_device->CreatePixelShader(ps_tilemap_code, ARRAYSIZE(ps_tilemap_code));
	
	const float w = static_cast<float>(s_renderTarget->GetWidth());
	const float h = static_cast<float>(s_renderTarget->GetHeight());

	// create a triangle out of vertices
    const VERTEX OurVertices[] =
    {
        { 0.0f,	0.0f,	5.0f,		0.0f, 0.0f,	0 },
        { w,	0.0f,	5.0f,		1.0f, 0.0f, 1 },
        { 0.0f,	h,		5.0f,		0.0f, 1.0f, 2 },
        { w,	h,		5.0f,		1.0f, 1.0f, 3 }
    };
	s_tilemapVBuf = s_device->CreateVertexBuffer(ARRAYSIZE(OurVertices), sizeof(VERTEX), OurVertices);

	const uint16_t Indices[] = {
		0, 1, 2,
		1, 3, 2
	};
	s_tilemapIBuf = s_device->CreateIndexBuffer(ARRAYSIZE(Indices), IndexSize::Index16, Indices);

	s_matrices[0] = DirectX::XMMatrixIdentity();
	s_matrices[1] = DirectX::XMMatrixIdentity();
	s_matrices[2] = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, s_renderTarget->GetWidth(), s_renderTarget->GetHeight(), 0.0f, 0.001f, 100.0f);
	s_vsPerObject = s_device->CreateConstantBuffer(sizeof(DirectX::XMMATRIX) * 3, s_matrices);
	s_psPerObjectData = {
		1.0f / 128.0f, 1.0f / 128.0f,
		1.0f / 42.0f, 1.0f / 34.0f,
		16.0f
	};
	s_psPerObject = s_device->CreateConstantBuffer(sizeof(s_psPerObjectData), &s_psPerObjectData);
	s_vsPerFrameData = {
		0.0f, 0.0f,
		s_renderTarget->GetWidth() / scale, s_renderTarget->GetHeight() / scale,
		1.0f / 42.0f, 1.0f / 34.0f,
		1.0f / 16.0f
	};
	s_vsPerFrame = s_device->CreateConstantBuffer(sizeof(s_vsPerFrameData), &s_vsPerFrameData);

	s_tilemap = PngLoader::CreateTexture("./spelunky0.png", *s_device);
	s_tileset = PngLoader::CreateTexture("./spelunky-tiles.png", *s_device);
}

/// @brief Runs the main loop, processing operating system messages.
/// @return The exit code to return to the os.
int32_t DoMessageLoop()
{
	MSG msg {};
	while(GetMessage(&msg, nullptr, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessageW(&msg);
	}

	return static_cast<int32_t>(msg.wParam);
}

volatile bool m_resized = false;

/// @brief This thread renders the game.
void RenderThread()
{
	uint64_t totalTime = 0;
	uint64_t frames = 0;

	GraphicsContext& context = s_device->GetImmediateContext();
	while(!s_terminate)
	{
		if (m_resized)
		{
			s_matrices[2] = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, s_renderTarget->GetWidth(), s_renderTarget->GetHeight(), 0.0f, 0.001f, 100.0f);
			context.UpdateConstantBuffer(*s_vsPerObject, &s_matrices);

			s_vsPerFrameData.viewportSize[0] = s_renderTarget->GetWidth() / scale;
			s_vsPerFrameData.viewportSize[1] = s_renderTarget->GetHeight() / scale;
			context.UpdateConstantBuffer(*s_vsPerFrame, &s_vsPerFrameData);

			m_resized = false;
		}

		context.Reset();

		context.SetViewport(0, 0, s_renderTarget->GetWidth(), s_renderTarget->GetHeight(), 0.0f, 1.0f);
		context.SetRenderTarget(*s_renderTarget);
		context.ClearRenderTarget(*s_renderTarget);

		context.SetBlendMode(BlendMode::Alpha);
		context.SetFillMode(FillMode::Solid);
		context.SetFaceCullMode(FaceCullMode::Back);

		// Set geometry shaders and buffers
		context.SetTopology(Topology::TriangleList);
		context.SetVertexBuffer(*s_tilemapVBuf);
		context.SetIndexBuffer(*s_tilemapIBuf);

		// Prepare shaders and parameters
		context.SetVertexShader(*s_tilemapVs);
		context.SetPixelShader(*s_tilemapPs);

		// Parameters
		ConstantBuffer* vsBuffers[] = { s_vsPerObject, s_vsPerFrame };
		context.SetConstantBuffer(ShaderStage::VertexShader, 2, vsBuffers);
		context.SetConstantBuffer(ShaderStage::PixelShader, 1, &s_psPerObject);		

		Texture* textures[] = { s_tileset, s_tilemap };
		context.SetTextures(ShaderStage::PixelShader, 2, textures);

		// Draw the quad
		context.DrawIndexed(6);

		// Swap buffers
		s_renderTarget->Present();
	}
}

// Terminate the application if the main window is closed
class MainWindowListener final : public RenderWindowListener
{
public:
	~MainWindowListener() override = default;

public:
	void OnRenderWindowClosed(RenderWindow& window) override
	{
		PostQuitMessage(0);
	}
	
	void OnResize(RenderWindow& window) override
	{
		m_resized = true;
	}
};

/// @brief Procedural entry point of the game on Windows machines.
/// @return The exit code to return to the operating system.
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int nShowCmd)
{
	MainWindowListener listener;
	s_device = std::make_unique<GraphicsDevice>();
	s_renderTarget = s_device->CreateRenderWindow(TEXT("TileMap Demo"), 1600, 900);
	s_renderTarget->SetListener(&listener);

	LoadResources();

	std::thread s_renderThread { RenderThread };
	SetThreadDescription(s_renderThread.native_handle(), TEXT("RenderThread"));
	
	const int32_t exitCode = DoMessageLoop();

	s_terminate = true;
	s_renderThread.join();

	return exitCode;
}
