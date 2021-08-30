
#include "PngLoader.h"
#include "GraphicsDevice.h"
#include "Texture.h"

#include <cassert>
#include <vector>

#include <png.h>
#include <d3d11.h>

#include "ComUtils.h"

Texture* PngLoader::CreateTexture(const char* file_name, GraphicsDevice& device)
{
    png_byte header[8];

    FILE *fp = nullptr;
	fopen_s(&fp, file_name, "rb");
	assert(fp);

    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8))
		assert(false);

    /* initialize stuff */
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	assert(png_ptr);

    png_infop info_ptr = png_create_info_struct(png_ptr);
	assert(info_ptr);

    if (setjmp(png_jmpbuf(png_ptr)))
		assert(false);

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
    png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
    png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    int number_of_passes = png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);

    /* read file */
    if (setjmp(png_jmpbuf(png_ptr)))
		assert(false);

    png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for (png_uint_32 y = 0; y < height; y++)
		row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));

    png_read_image(png_ptr, row_pointers);

    fclose(fp);

	Format format = Format::R8G8B8A8;

	std::vector<uint8_t> mipData;
	if (color_type == PNG_COLOR_TYPE_RGB)
	{
		mipData.resize(width * height * 4, 0);
		for (unsigned y=0; y < height; y++) 
		{
			png_byte* row = row_pointers[y];
			for (unsigned x=0; x<width; x++) 
			{
				png_byte* ptr = &(row[x*3]);
				mipData[(y * width + x) * 4 + 0] = ptr[0];
				mipData[(y * width + x) * 4 + 1] = ptr[1];
				mipData[(y * width + x) * 4 + 2] = ptr[2];
				mipData[(y * width + x) * 4 + 3] = 255;
			}
		}
	}
	else if (color_type == PNG_COLOR_TYPE_RGBA)
	{
		mipData.resize(width * height * 4, 0);
		for (unsigned y=0; y < height; y++) 
		{
			png_byte* row = row_pointers[y];
			for (unsigned x=0; x<width; x++) 
			{
				png_byte* ptr = &(row[x*4]);
				mipData[(y * width + x) * 4 + 0] = ptr[0];
				mipData[(y * width + x) * 4 + 1] = ptr[1];
				mipData[(y * width + x) * 4 + 2] = ptr[2];
				mipData[(y * width + x) * 4 + 3] = ptr[3];
			}
		}
	}
	else
	{
		assert(false);
	}

	Texture* texture = device.CreateTexture(width, height, format, &mipData[0]);
	
    for (png_uint_32 y = 0; y < height; y++)
		free(row_pointers[y]);
	free(row_pointers);

	return texture;
}
