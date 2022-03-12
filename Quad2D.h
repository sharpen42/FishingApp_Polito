#pragma once

#include "Color.h"
#include "Vector.h"

class Quad2D {
private:
	std::string text;
	ColorRGBA bg_color, text_color;
	Texture texture;
public:
	Vector2 position;
	float dim_x, dim_y;

	Quad2D();
	Quad2D(double, double);
	Quad2D(Vector2, double, double);

	void applyText(std::string, ColorRGBA);
	void applyColor(ColorRGBA);
	void applyTexture(Texture);

	//void drawOpenGL();
};