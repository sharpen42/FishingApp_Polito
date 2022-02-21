#include "Color.h"

//#include "stb_image.h"

bool IL_init = false;

ColorRGBA::ColorRGBA(){
	r = 0;
	g = 0;
	b = 0;
	a = 255;
}

ColorRGBA::ColorRGBA(float r0, float g0, float b0, float a0) {
	r = (r0 >= 1.0) ? 255 : (uint8_t)floor(r0 * 255.0);
	g = (g0 >= 1.0) ? 255 : (uint8_t)floor(g0 * 255.0);
	b = (b0 >= 1.0) ? 255 : (uint8_t)floor(b0 * 255.0);
	a = (a0 >= 1.0) ? 255 : (uint8_t)floor(a0 * 255.0);
}

ColorRGBA::ColorRGBA(int r0, int g0, int b0, int a0) {
	r = (r0 >= 255) ? 255 : (uint8_t)r0;
	g = (g0 >= 255) ? 255 : (uint8_t)g0;
	b = (b0 >= 255) ? 255 : (uint8_t)b0;
	a = (a0 >= 255) ? 255 : (uint8_t)a0;
}

bool ColorRGBA::operator==(ColorRGBA c) { return (r == c.r && g == c.g && b == c.b && a == c.a); }
bool ColorRGBA::operator!=(ColorRGBA c) { return (r != c.r || g != c.g || b != c.b || a != c.a); }

ColorRGBA ColorRGBA::operator*(float f) {
	if (f < 0) f = -f;
	if (f > 1) f -= floor(f);
	return ColorRGBA((uint8_t)floor(r * f), (uint8_t)floor(g * f), (uint8_t)floor(b * f), (uint8_t)floor(a * f));
}

ColorRGBA ColorRGBA::operator*=(float f) {
	if (f < 0) f = -f;
	if (f > 1) f -= floor(f);
	r = (uint8_t)floor(r * f);
	g = (uint8_t)floor(g * f); 
	b = (uint8_t)floor(b * f); 
	a = (uint8_t)floor(a * f);
	return *this;
}

void ColorRGBA::set(float r0, float g0, float b0, float a0) {
	r = (uint8_t)floor(r0 * 255.0);
	g = (uint8_t)floor(g0 * 255.0);
	b = (uint8_t)floor(b0 * 255.0);
	a = (uint8_t)floor(a0 * 255.0);
}

void ColorRGBA::set(int r0, int g0, int b0, int a0){
	r = (uint8_t)r0;
	g = (uint8_t)g0;
	b = (uint8_t)b0;
	a = (uint8_t)a0;
}

float* ColorRGBA::toFloat4(float c[4]) {
	c[0] = getRedf();
	c[1] = getGreenf();
	c[2] = getBluef();
	c[3] = getAlphaf();
	return c;
}

int* ColorRGBA::toInt4(int c[4]) {
	c[0] = getRedi();
	c[1] = getGreeni();
	c[2] = getBluei();
	c[3] = getAlphai();
	return c;
}

float ColorRGBA::getRedf(){ return ((float) r) / 255.0; }
float ColorRGBA::getBluef(){ return ((float) g) / 255.0; }
float ColorRGBA::getGreenf(){ return ((float) b) / 255.0; }
float ColorRGBA::getAlphaf(){ return ((float) a) / 255.0; }
int ColorRGBA::getRedi() { return (int)r; }
int ColorRGBA::getBluei() { return (int)g; }
int ColorRGBA::getGreeni() { return (int)b; }
int ColorRGBA::getAlphai() { return (int)a; }
void ColorRGBA::setAlpha(float a0) { a = (uint8_t)floor(a0 * 255.0); }

ColorRGBA ColorRGBA::White() {	return ColorRGBA(255, 255, 255, 255); }
ColorRGBA ColorRGBA::Black() {	return ColorRGBA(0, 0, 0, 255); }
ColorRGBA ColorRGBA::Grey(float f) {
	if (f < 0.0) f = -f;
	if(f > 1.0) f -= floor(f);
	return ColorRGBA((uint8_t)floor(255.0 * f), (uint8_t)floor(255.0 * f), (uint8_t)floor(255.0 * f), 255);
}

ColorRGBA ColorRGBA::Red() {	return ColorRGBA(255, 0, 0, 255); }
ColorRGBA ColorRGBA::Green() { return ColorRGBA(0, 255, 0, 255); }
ColorRGBA ColorRGBA::Blue() { return ColorRGBA(0, 0, 255, 255); }
ColorRGBA ColorRGBA::Yellow() { return ColorRGBA(255, 255, 0, 255); }
ColorRGBA ColorRGBA::Cyan() { return ColorRGBA(0, 255, 255, 255); }
ColorRGBA ColorRGBA::Magenta() { return ColorRGBA(255, 0, 255, 255); }

bool Texture::init() {
	// Before calling ilInit() version should be checked.
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		ILint test = ilGetInteger(IL_VERSION_NUM);
		/// wrong DevIL version ///
		std::string err_msg = "Wrong DevIL version. Old devil.dll in system32/SysWow64?";
		char* cErr_msg = (char*)err_msg.c_str();

		return false;
	}

	ilInit(); // Initialization of DevIL
	IL_init = true;
	return true;
}

Texture::Texture() {
	rendererID = 0;
	imageID = 0;
	bpp = 0;
	type = COLOR_TYPE::COLOR_UNDEFINED;
	height = width = 0;
}


Texture::Texture(std::string filename) {
	ILboolean success = false;

	rendererID = 0;
	imageID = 0;
	bpp = 0;
	type = COLOR_TYPE::COLOR_UNDEFINED;
	height = width = 0;
	path = "";

	if (!IL_init && !init()) return;
	imageID = ilGenImage(); // Generation of image name, save IL image ID
	ilBindImage(imageID); // Binding of DevIL image name
	success = ilLoadImage((const wchar_t*)filename.c_str());
	std::cerr << "LOG: Loading Image: '" << filename.data() << "'.\n";
	path.assign(filename);

	if (success) // If no error occured:
	{
		success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE); // Convert every colour component into unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA
		if (!success)
		{
			// Error occured
			std::cerr << "ERROR: Couldn't convert image";
			return;
		}

		bpp = ilGetInteger(IL_IMAGE_BPP);
		width = ilGetInteger(IL_IMAGE_WIDTH);
		height = ilGetInteger(IL_IMAGE_HEIGHT);

		switch (ilGetInteger(IL_IMAGE_FORMAT)) {
		case (GL_RGB): type = COLOR_TYPE::COLOR_RGB; break;
		case (GL_RGBA): type = COLOR_TYPE::COLOR_RGBA; break;
		default: break;
		}


		glGenTextures(1, &rendererID); // Texture name generation
		glBindTexture(GL_TEXTURE_2D, rendererID); // Binding of texture name
		//redefine standard texture values
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // We will use linear interpolation for magnification filter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // We will use linear interpolation for minifying filter
		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), 
					 ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, 
					 ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, 
					 ilGetData());
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		// Error occured 
		std::cerr << "ERROR: Couldn't load Image: '" << filename.data() << "'\n";
	}
	ilDeleteImage(imageID); // Because we have already copied image data into texture data we can release memory used by image.
}

Texture::~Texture() {
	//if (pixels) stbi_image_free(pixels);
	//ilDeleteImage(imageID);
	glDeleteTextures(1, &rendererID);
}

bool Texture::isEmpty() {
	return (rendererID == 0);
}

void Texture::Bind(/*unsigned int slot = 0*/) {
	if (rendererID == 0) return;
	//glActiveTexture(GL_TEXTURE0 + slot); 
	// a quanto pare non funziona se non si includono anche altre librerie, ma non è necessario; 
	// serve nel caso si debbano usare più textures per lo stesso oggetto, ci sono degli slot dedicati, 
	// ma possiamo ottenere lo stesso effetto in altri modi.
	glBindTexture(GL_TEXTURE_2D, rendererID);
}

void Texture::UnBind() {
	if (rendererID == 0) return;
	glBindTexture(GL_TEXTURE_2D, 0);
}

int Texture::getHeight() { return height; }
int Texture::getWidth() { return width; }
const char* Texture::getPath() { return path.c_str(); }

bool Material::operator==(Material m) { return (ambient == m.ambient && diffuse == m.diffuse && specular == m.specular && emission == m.emission); }
bool Material::operator!=(Material m) { return (ambient != m.ambient || diffuse != m.diffuse || specular != m.specular || emission == m.emission); }

Material::Material() {
	ambient = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
	diffuse = ColorRGBA(0.8f, 0.8f, 0.8f, 1.0f);
	specular = ColorRGBA(0.5f, 0.5f, 0.5f, 1.0f);
	emission = ColorRGBA();
	texture = Texture();
	//shader = Shader();
}

Material::Material(ColorRGBA d, ColorRGBA s, ColorRGBA a, ColorRGBA e) {
	diffuse = d;
	specular = s;
	ambient = a;
	emission = e;
	texture = Texture();
}

Texture Material::addTexture(Texture t0) {
	Texture t = texture;
	texture = t0;
	return t;
}
