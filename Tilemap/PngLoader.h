#pragma once

class Texture;
class GraphicsDevice;

class PngLoader final
{
public:
	static Texture* CreateTexture(const char* file_name, GraphicsDevice& device);
};
