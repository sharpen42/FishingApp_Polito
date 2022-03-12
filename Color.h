#pragma once
#include <stdio.h>
#include <string>
#include <iostream>

#include <GL/glut.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
//#include "stb_image.h"

enum class COLOR_TYPE {
	COLOR_UNDEFINED, COLOR_BW, COLOR_BWA, COLOR_RGB, COLOR_RGBA
};

class ColorRGBA {
private:
	uint32_t r, g, b, a;

public:
	ColorRGBA();
	ColorRGBA(float r, float g, float b, float a);
	ColorRGBA(int r, int g, int b, int a);

	bool operator==(ColorRGBA);
	bool operator!=(ColorRGBA);
	
	ColorRGBA operator*(float);
	ColorRGBA operator*=(float);

	void set(float r, float g, float b, float a);
	void set(int r, int g, int b, int a);
	float* toFloat4(float c[4]);
	int* toInt4(int c[4]);
	float getRedf();
	float getBluef();
	float getGreenf();
	float getAlphaf();
	int getRedi();
	int getBluei();
	int getGreeni();
	int getAlphai();
	void setAlpha(float a);
	void Bind();
	void UnBind();
	
	static ColorRGBA White();
	static ColorRGBA Black();
	static ColorRGBA Grey(float);
	static ColorRGBA Red();
	static ColorRGBA Green();
	static ColorRGBA Blue();
	static ColorRGBA Yellow();
	static ColorRGBA Cyan();
	static ColorRGBA Magenta();
	
};

class Shader {
public:
	int shaderID;
	std::string name, code;
	Shader();
};

class Texture {
private:
	COLOR_TYPE type;
	int height, width, bpp; // bpp: bits per pixels
	unsigned int rendererID, imageID;
	std::string path;
	unsigned char* pixels;
public:

	Texture();
	Texture(std::string path);
	~Texture();
	bool isEmpty();
	void Bind(/*unsigned int slot = 0*/);
	void UnBind();
	int getHeight();
	int getWidth();
	const char* getPath();
	int getId();

	static bool init();
};

class Material {
public:
	ColorRGBA diffuse, specular, ambient, emission;
	//Texture texture;
	//Shader shader;

	bool operator==(Material);
	bool operator!=(Material);

	Material();
	Material(ColorRGBA diffuse, ColorRGBA specular, ColorRGBA ambient, ColorRGBA emission);
};