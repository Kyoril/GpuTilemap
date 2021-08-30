
#include "RenderWindow.h"

#include "ComUtils.h"

#include <cassert>

// TODO: Implement these boolean with actual values.
const bool hasTearingSupport = true;
const bool useVSync = true;

static const TCHAR* s_windowClassName = TEXT("D3D11RenderWindow");


RenderWindow::RenderWindow(const TCHAR name[], uint16_t width, uint16_t height, ID3D11Device& device)
	: m_device(device)
	, m_width(width)
	, m_height(height)
{
	assert(width > 0);
	assert(height > 0);

	m_format = DXGI_FORMAT_B8G8R8A8_UNORM;
	m_handle = CreateRenderWindow(name, width, height);
	m_ownHandle = true;

	::ShowWindow(m_handle, SW_SHOWNORMAL);
	::UpdateWindow(m_handle);

	CreateSwapChain(device);
	CreateSizeDependantResources();
}

RenderWindow::~RenderWindow()
{
	if (m_ownHandle && m_handle)
	{
		::DestroyWindow(m_handle);
		m_handle = nullptr;
	}
}

void RenderWindow::Resize(uint16_t newWidth, uint16_t newHeight)
{
	assert(newWidth > 0);
	assert(newHeight > 0);

	std::lock_guard<std::mutex> lock { m_resizeLock };
	m_newWidth = newWidth;
	m_newHeight = newHeight;
	m_resizePending = true;
}

void RenderWindow::Present()
{		
	BOOL IsFullscreenState;
	HRESULT hr = m_swapChain->GetFullscreenState(&IsFullscreenState, nullptr);
	ThrowIfFail(hr);

	const UINT presentFlags = hasTearingSupport && !useVSync && !IsFullscreenState ? DXGI_PRESENT_ALLOW_TEARING : 0;
	hr = m_swapChain->Present(useVSync ? 1 : 0, presentFlags);
	ThrowIfFail(hr);

	// Apply pending resize
	if (m_resizePending)
	{
		PerformResize();
		m_resizePending = false;
	}
}

void RenderWindow::SetListener(RenderWindowListener* listener)
{
	m_listener = listener;
}

void RenderWindow::CreateSwapChain(ID3D11Device& device)
{
	// Create swap chain
	ComPtr<IDXGIDevice> DXGIDevice;
	HRESULT hr = device.QueryInterface(IID_PPV_ARGS(&DXGIDevice));
	ThrowIfFail(hr);

	// Query the adapter that created the device
	ComPtr<IDXGIAdapter> DXGIAdapter;
	hr = DXGIDevice->GetAdapter(&DXGIAdapter);
	ThrowIfFail(hr);

	// Now query the factory that created the adapter object
	ComPtr<IDXGIFactory5> DXGIFactory;
	hr = DXGIAdapter->GetParent(IID_PPV_ARGS(&DXGIFactory));
	ThrowIfFail(hr);

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));
	scd.BufferCount = 2;
	scd.BufferDesc.Format = m_format;
	scd.BufferDesc.Width = m_width;
	scd.BufferDesc.Height = m_height;
	scd.BufferDesc.RefreshRate.Numerator = 60;	// TODO: Determine monitor refresh rate in Hz and use the value here to
	scd.BufferDesc.RefreshRate.Denominator = 1;	// support high refresh rate monitors
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.Flags = hasTearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
	scd.OutputWindow = m_handle;
	scd.SampleDesc.Count = 1;
	scd.SwapEffect = hasTearingSupport ? DXGI_SWAP_EFFECT_FLIP_DISCARD : DXGI_SWAP_EFFECT_DISCARD;
	scd.Windowed = TRUE;
	hr = DXGIFactory->CreateSwapChain(&device, &scd, &m_swapChain);
	ThrowIfFail(hr);
}

void RenderWindow::PerformResize()
{
	std::lock_guard<std::mutex> lock { m_resizeLock };
	m_width = m_newWidth;
	m_height = m_newHeight;

	m_renderTargetView.Reset();

	HRESULT hr = m_swapChain->ResizeBuffers(2, m_width, m_height, m_format, hasTearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0);
	ThrowIfFail(hr);

	CreateSizeDependantResources();
}
	
void RenderWindow::CreateSizeDependantResources()
{
	ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	ThrowIfFail(hr);

	hr = m_device.CreateRenderTargetView(backBuffer.Get(), nullptr, &m_renderTargetView);
	ThrowIfFail(hr);
}

LRESULT CALLBACK RenderWindow::MainWindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_NCCREATE)
	{
		LPCREATESTRUCT cs = (LPCREATESTRUCT)lparam;
		::SetWindowLongPtr(wnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
	}
	else
	{
		RenderWindow* window = (RenderWindow*)::GetWindowLongPtr(wnd, GWLP_USERDATA);
		if (window != nullptr)
		{
			switch(msg)
			{
			case WM_SIZE:
				{
					RECT r{};
					::GetClientRect(wnd, &r);
					window->Resize(r.right - r.left, r.bottom - r.top);
				}
				return 1;
			case WM_PAINT:
				return 1;
			case WM_ERASEBKGND:
				return 1;
			case WM_CLOSE:
				::DestroyWindow(wnd);
				window->m_handle = nullptr;
				return 1;
			case WM_DESTROY:
				if (window->m_listener)
				{
					window->m_listener->OnRenderWindowClosed(*window);
				}
				return 1;
			}
		}
	}
		
	return ::DefWindowProc(wnd, msg, wparam, lparam);
}
	
void RenderWindow::EnsureWindowClassRegistered()
{
	static bool s_classRegistered = false;
	if (!s_classRegistered)
	{
		WNDCLASSEX wc;
		ZeroMemory(&wc, sizeof(wc));

		wc.cbSize = sizeof(wc);
		wc.hbrBackground = reinterpret_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH));
		wc.hInstance = GetModuleHandle(nullptr);
		wc.lpszClassName = s_windowClassName;
		wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.lpfnWndProc = MainWindowProc;
		wc.style = CS_HREDRAW | CS_VREDRAW;

		s_classRegistered = (RegisterClassEx(&wc) > 0);
		assert(s_classRegistered);
	}
}

HWND RenderWindow::CreateRenderWindow(const TCHAR title[], uint16_t width, uint16_t height)
{
	EnsureWindowClassRegistered();

	const DWORD ws = WS_OVERLAPPEDWINDOW;
	const DWORD ex = 0;

	RECT r { 0, 0, width, height };
	::AdjustWindowRectEx(&r, ws, FALSE, ex);

	const int sx = ::GetSystemMetrics(SM_CXSCREEN);
	const int sy = ::GetSystemMetrics(SM_CYSCREEN);
	const int w = r.right - r.left;
	const int h = r.bottom - r.top;
	const int x = sx / 2 - w / 2;
	const int y = sy / 2 - h / 2;

	HWND handle = ::CreateWindowEx(ex, s_windowClassName, title, ws, x, y, w, h, nullptr, nullptr, ::GetModuleHandle(nullptr), this);
	assert(handle);

	return handle;
}
