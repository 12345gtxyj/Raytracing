#pragma once
#include <cassert>
#include "color.h"
#include <corecrt_malloc.h>
class RenderBuffer {
public:
	RenderBuffer(int width, int height);
	~RenderBuffer();

	int width, height;
	unsigned char* color_buffer;
	float* depth_buffer;

	void set_depth(int x, int y, float depth);
	float get_depth(int x, int y);
	void set_color0_1(int x, int y, Color color);
	void set_color0_255(int x, int y, Color color);
	Color get_color(int x, int y);

	void renderbuffer_release();
	void renderbuffer_clear_color(Color color);
	void renderbuffer_clear_depth(float depth);
};