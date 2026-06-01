#include "helpers.h"

Rectangle GetTextureAltasBlock(int x, int y, int cellSizePixelsX, int cellSizePixelsY)
{
	return Rectangle{ 
		static_cast<float>(x * cellSizePixelsX), 
		static_cast<float>(y * cellSizePixelsY), 
		static_cast<float>(cellSizePixelsX), 
		static_cast<float>(cellSizePixelsY) 
	};
}