#pragma once

#include "Color.h"
#include "Vector.h"

class Quad2D {
private:
	std::string text;
	ColorRGBA bg_color, text_color;
	Texture texture;
	bool over;
public:
	Vector2 position;
	double dim_x, dim_y;

	Quad2D();
	Quad2D(double, double);
	Quad2D(Vector2, double, double);

	void applyColor(ColorRGBA);
	void applyTexture(Texture);
	void applyText(std::string, ColorRGBA);

	void mouseOver(bool);
	bool isOver();

	void drawOpenGL();

	static void DrawQuad(double, double);
	static void DrawQuad(Vector2 pos, double dx, double dy);
	static void DrawQuad(Vector2 pos, double dx, double dy, Texture texture);
	static void DrawQuad(Vector2 pos, double dx, double dy, ColorRGBA color);
	static void DrawQuad(Vector2 pos, double dx, double dy, std::string, ColorRGBA text, ColorRGBA background);
	static void DrawQuad(Vector2 pos, double dx, double dy, std::string, ColorRGBA text, Texture texture);

};
