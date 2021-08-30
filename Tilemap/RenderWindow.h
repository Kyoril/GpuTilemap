#pragma once

#include <cstdint>
#include <mutex>

#include <Windows.h>
#include <d3d11.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

/// @brief Interface of a listener which is notified on certain events of a RenderWindow.
class RenderWindowListener
{
public:
	/// @brief Virtual default destructor due to inheritance.
	virtual ~RenderWindowListener() = default;

public:
	/// @brief Executed if a render window is closed.
	/// @param window The render window that was just closed.
	virtual void OnRenderWindowClosed(class RenderWindow& window) {};
};

class RenderWindow final
{
	friend class GraphicsContext;

public:
	/// @brief Creates a new instance of the RenderWindow class and initializes it.
	/// @param name Name of the render window, also the title.
	/// @param width Width of the render window in pixels. Has to be greater than 0.
	/// @param height Height of the render window in pixels. Has to be greater than 0.
	/// @param device A graphics device which is required to create d3d11 resources.
	explicit RenderWindow(const TCHAR name[], uint16_t width, uint16_t height, ID3D11Device& device);

	/// @brief Default destructor. Destroy custom window handle if owned.
	~RenderWindow();

public:
	/// @brief Resizes the render window. Resize is implemented thread safe and does not occur immediately.
	/// @param newWidth The new width of the render window in pixels. Has to be greater than 0.
	/// @param newHeight The new height of the render window in pixels. Has to be greater than 0.
	void Resize(uint16_t newWidth, uint16_t newHeight);

	/// @brief Swaps the back buffer with the front buffer to make rendering operations visible. 
	///        Also performs resizing if required, which might lock the calling thread.
	void Present();

	/// @brief Sets or resets the render window listener which will be notified on certain window events like close.
	/// @param listener The new listener to be notified or nullptr if you don't want to have a listener.
	void SetListener(RenderWindowListener* listener);

private:
	/// @brief Creates the swap chain.
	/// @param device The d3d11 device to use for creating the swap chain.
	void CreateSwapChain(ID3D11Device& device);

	/// @brief Resizes the actual swap chain.
	void PerformResize();
	
	/// @brief Creates gpu resources which depend on the size of the render target view.
	void CreateSizeDependantResources();

	/// @brief Handle for OS messages targeted for our custom created render window instance.
	/// @param wnd The window handle.
	/// @param msg The message id.
	/// @param wparam Parameter #1. Value depends on message id.
	/// @param lparam Paramter #2. Value depends on message id.
	/// @return Return values depends on message id and whether or not it was handled by this method.
	static LRESULT CALLBACK MainWindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);
	
	/// @brief Ensures that our internal window class has been registered and is ready to be used.
	void EnsureWindowClassRegistered();

	/// @brief Creates a render window using our own internal window class.
	/// @param title The title of the window.
	/// @param width Width of the window's client area in pixels.
	/// @param height Height of the window's client area in pixels.
	/// @return The new window handle.
	HWND CreateRenderWindow(const TCHAR title[], uint16_t width, uint16_t height);

public:
	/// @brief Gets the current width of the render window in pixels.
	/// @return The current width of the render window in pixels.
	uint16_t GetWidth() const { return m_width; }

	/// @brief Gets the current height of the render window in pixels.
	/// @return The current height of the render window in pixels.
	uint16_t GetHeight() const { return m_height; }

private:
	ID3D11Device& m_device;
	HWND m_handle{nullptr};
	bool m_ownHandle = false;
	DXGI_FORMAT m_format{};
	uint16_t m_width;
	uint16_t m_height;
	volatile bool m_resizePending = false;
	volatile uint16_t m_newWidth = 0;
	volatile uint16_t m_newHeight = 0;
	std::mutex m_resizeLock;
	ComPtr<IDXGISwapChain> m_swapChain;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	RenderWindowListener* m_listener;
};