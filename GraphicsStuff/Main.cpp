
#include "General/RenderLoop.h"

#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"

int main(int argc, char* argv[])
{
	RenderLoop loop;
	return loop.Run();
}
