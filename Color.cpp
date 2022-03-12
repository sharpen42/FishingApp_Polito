#include "Color.h"

bool IL_init = false;

ColorRGBA::ColorRGBA(){
	r = 0;
	g = 0;
	b = 0;
	a = 255;
}

ColorRGBA::ColorRGBA(float r0, float g0, float b0, float a0) {
	r = (r0 >= 1.0) ? 255 : (int)floor(r0 * 255.0);
	g = (g0 >= 1.0) ? 255 : (int)floor(g0 * 255.0);
	b = (b0 >= 1.0) ? 255 : (int)floor(b0 * 255.0);
	a = (a0 >= 1.0) ? 255 : (int)floor(a0 * 255.0);
}

ColorRGBA::ColorRGBA(int r0, int g0, int b0, int a0) {
	r = (r0 < 0) ? 0 : (int)r0;
	g = (g0 < 0) ? 0 : (int)g0;
	b = (b0 < 0) ? 0 : (int)b0;
	a = (a0 < 0) ? 0 : (int)a0;
}

bool ColorRGBA::operator==(ColorRGBA c) { return (r == c.r && g == c.g && b == c.b && a == c.a); }
bool ColorRGBA::operator!=(ColorRGBA c) { return (r != c.r || g != c.g || b != c.b || a != c.a); }

ColorRGBA ColorRGBA::operator*(float f) {
	if (f < 0) f = -f;
	if (f > 1) f -= floor(f);
	return ColorRGBA((int)floor(r * f), (int)floor(g * f), (int)floor(b * f), (int)floor(a * f));
}

ColorRGBA ColorRGBA::operator*=(float f) {
	if (f < 0) f = -f;
	if (f > 1) f -= floor(f);
	r = (int)floor(r * f);
	g = (int)floor(g * f);
	b = (int)floor(b * f);
	a = (int)floor(a * f);
	return *this;
}

void ColorRGBA::set(float r0, float g0, float b0, float a0) {
	r = (int)floor(r0 * 255.0);
	g = (int)floor(g0 * 255.0);
	b = (int)floor(b0 * 255.0);
	a = (int)floor(a0 * 255.0);
}

void ColorRGBA::set(int r0, int g0, int b0, int a0){
	r = (r0 < 0) ? 0 : r0;
	g = (g0 < 0) ? 0 : g0;
	b = (b0 < 0) ? 0 : b0;
	a = (a0 < 0) ? 0 : b0;
}

float* ColorRGBA::toFloat4(float c[4]) {
	c[0] = getRedf();
	c[1] = getGreenf();
	c[2] = getBluef();
	c[3] = getAlphaf();
	return c;
}

int* ColorRGBA::toInt4(int c[4]) {
	c[0] = (int) r;
	c[1] = (int) g;
	c[2] = (int) b;
	c[3] = (int) a;
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
void ColorRGBA::setAlpha(float a0) { a = (uint32_t)floor(a0 * 255.0); }
void ColorRGBA::Bind() { glColor4i(r, g, b, a); }
void ColorRGBA::UnBind() { glColor4f(1.0, 1.0, 1.0, 1.0); }

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
		std::cout << "ERROR: Wrong DevIL version. Old devil.dll in system32/SysWow64?";
		return false;
	}

	ilInit(); // Initialization of DevIL
	ILenum devilError = ilGetError();
	if (devilError != IL_NO_ERROR)
		std::cout << "Devil Error: " << (unsigned char*)iluErrorString(devilError) << "\n";
	IL_init = true; 
	ilutRenderer(ILUT_OPENGL);
	return true;
}

Texture::Texture() {
	rendererID = -1;
	imageID = 0;
	bpp = 0;
	type = COLOR_TYPE::COLOR_UNDEFINED;
	height = width = 0;
	pixels = nullptr;
}

/*
Texture::Texture(std::string filename) {
	rendererID = -1;
	imageID = 0;
	bpp = 0;
	type = COLOR_TYPE::COLOR_RGBA;
	height = width = 0;
	pixels = nullptr;
	path.assign(filename);

	stbi_set_flip_vertically_on_load(0);
	pixels = stbi_load(filename.c_str(), &width, &height, &bpp, 4);
	if(pixels == nullptr) std::cout << "ERROR: couldn't load image " << filename << ".\n";
	glGenTextures(1, &rendererID); // Texture name generation
	glBindTexture(GL_TEXTURE_2D, rendererID); // Binding of texture name
	//redefine standard texture values
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // We will use linear interpolation for magnification filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // We will use linear interpolation for minifying filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (pixels) stbi_image_free(pixels);
}
*/

Texture::Texture(std::string filename) {
	ILboolean success = false;
	ILenum devilError;

	rendererID = -1;
	imageID = 0;
	bpp = 0;
	type = COLOR_TYPE::COLOR_UNDEFINED;
	height = width = 0;
	path = "";

	if (!IL_init) { init(); }
	imageID = ilGenImage(); // Image name generation
	devilError = ilGetError();
	if (devilError != IL_NO_ERROR)
		std::cout << "Devil Error: post gen image - " << devilError << "\n";
	IL_init = true;
	ilBindImage(imageID); // Binding of DevIL image name
	devilError = ilGetError();
	if (devilError != IL_NO_ERROR)
		std::cout << "Devil Error: post bind image - " << devilError << "\n";
	IL_init = true;
	path += filename;
	success = ilLoadImage((const wchar_t*) path.c_str());
	devilError = ilGetError();
	if (devilError != IL_NO_ERROR)
		std::cout << "Devil Error: post load image - " << devilError << "\n";

	if (success) // If no error occured:
	{
		std::cerr << "LOG: Loading Image: '" << path.data() << "'.\n";
		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE); // Convert every colour component into unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA
		if (!success)
		{
			// Error occured
			std::cerr << "ERROR: Couldn't convert image";
			devilError = ilGetError();
			if (devilError != IL_NO_ERROR)
				std::cout << "Devil Error: " << (unsigned char*)iluErrorString(devilError) << "\n";
			IL_init = true;
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP),
					 ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, 
					 ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
					 ilGetData());
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		// Error occured 
		std::cerr << "ERROR: Couldn't load Image: '" << path.data() << "'\n";
	}
	ilDeleteImage(imageID); // Because we have already copied image data into texture data we can release memory used by image.
}


Texture::~Texture() {
	//if (pixels) stbi_image_free(pixels);
	//ilDeleteImage(imageID);
	if(rendererID > -1) glDeleteTextures(1, &rendererID);
}

bool Texture::isEmpty() {
	return (rendererID == -1);
}

void Texture::Bind(/*unsigned int slot = 0*/) {
	if (rendererID == -1) return;
	//glActiveTexture(GL_TEXTURE0); 
	// a quanto pare non funziona se non si includono anche altre librerie, ma non è necessario; 
	// serve nel caso si debbano usare più textures per lo stesso oggetto, ci sono degli slot dedicati, 
	// ma possiamo ottenere lo stesso effetto in altri modi.
	glBindTexture(GL_TEXTURE_2D, rendererID);
}

void Texture::UnBind() {
	if (rendererID == -1) return;
	glBindTexture(GL_TEXTURE_2D, 0);
}

int Texture::getHeight() { return height; }
int Texture::getWidth() { return width; }
const char* Texture::getPath() { return path.c_str(); }
int Texture::getId() { return rendererID; }

bool Material::operator==(Material m) { return (ambient == m.ambient && diffuse == m.diffuse && specular == m.specular && emission == m.emission); }
bool Material::operator!=(Material m) { return (ambient != m.ambient || diffuse != m.diffuse || specular != m.specular || emission == m.emission); }

Material::Material() {
	ambient = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
	diffuse = ColorRGBA(0.8f, 0.8f, 0.8f, 1.0f);
	specular = ColorRGBA(0.5f, 0.5f, 0.5f, 1.0f);
	emission = ColorRGBA();
	//shader = Shader();
}

Material::Material(ColorRGBA d, ColorRGBA s, ColorRGBA a, ColorRGBA e) {
	diffuse = d;
	specular = s;
	ambient = a;
	emission = e;
}
