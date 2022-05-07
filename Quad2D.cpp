#include "Quad2D.h"

//#define FONT GLUT_BITMAP_TIMES_ROMAN_24
#define FONT GLUT_BITMAP_HELVETICA_18

int screen2pixels(float n, int T) {
    return (int)((1 + n) * T / 2) ;
}

float pixels2screen(int p, int T) {
    return (float)(2 * p / T - 1.0);
}

void g_output(int x, int y, std::string str) {
    int len, i;

    //glWindowPos2i(x + off_txt_x, y + off_txt_y);
    glRasterPos2i(x, y);
    len = str.length();

    for (i = 0; i < len; i++)
        glutBitmapCharacter(FONT, str[i]);

    //glutPostRedisplay();
}

Quad2D::Quad2D() {
	text = "";
	bg_color = ColorRGBA();
	text_color = ColorRGBA::White();
	position = Vector2();
    scale = Vector2(1.0, 1.0);
    rotation = 0;
    dim_x = dim_y = 0;
    over = false;
}

Quad2D::Quad2D(double dx, double dy) {
	text = "";
	bg_color = ColorRGBA::White();
	text_color = ColorRGBA::Black();
	position = Vector2();
    scale = Vector2(1.0, 1.0);
    rotation = 0;
	dim_x = dx / 2;
	dim_y = dy / 2;
    over = false;
}

Quad2D::Quad2D(Vector2 v, double dx, double dy) {
	text = "";
	bg_color = ColorRGBA::White();
	text_color = ColorRGBA::Black();
	position = Vector2(v.x, v.y);
    scale = Vector2(1.0, 1.0);
    rotation = 0;
	dim_x = dx / 2;
	dim_y = dy / 2;
    over = false;
}

void Quad2D::applyText(std::string s, ColorRGBA c) {
	text = s;
	text_color.set(c.getRedi(), c.getGreeni(), c.getBluei(), c.getAlphai());
}

void Quad2D::mouseOver(bool t) { over = t; }
bool Quad2D::isOver() { return over; }

void Quad2D::applyColor(ColorRGBA c) {
	bg_color.set(c.getRedi(), c.getGreeni(), c.getBluei(), c.getAlphai());
}

void Quad2D::applyTexture(Texture t) {
	texture = t;
}

void Quad2D::drawOpenGL() {
    bool has_texture = (!texture.isEmpty());
    float c[4];

    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glTranslated(position.x, position.y, 0);
    glScaled(scale.x, scale.y, 1.0);
    glRotated(rotation, 0, 0, 1.0);

    glPushAttrib(GL_CURRENT_BIT);
    glColor4fv(bg_color.toFloat4(c));
    if (has_texture) {
        glEnable(GL_TEXTURE_2D);
        texture.Bind();
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_QUADS);
    if (has_texture) glTexCoord2f(0, 1);
    glVertex2d(-dim_x, -dim_y); //sx in basso
    if (has_texture) glTexCoord2f(1, 1);
    glVertex2d(dim_x, -dim_y); //dx in basso
    if (has_texture) glTexCoord2f(1, 0);
    glVertex2d(dim_x, dim_y); //dx in alto
    if (has_texture) glTexCoord2f(0, 0);
    glVertex2d(-dim_x, dim_y); //sx in alto
    glEnd();
    /*
    glBegin(GL_QUADS);
    if (has_texture) glTexCoord2f(0, 1);
    glVertex2d(position.x - dim_x, position.y - dim_y); //sx in basso
    if (has_texture) glTexCoord2f(1, 1);
    glVertex2d(position.x + dim_x, position.y - dim_y); //dx in basso
    if (has_texture) glTexCoord2f(1, 0);
    glVertex2d(position.x + dim_x, position.y + dim_y); //dx in alto
    if (has_texture) glTexCoord2f(0, 0);
    glVertex2d(position.x - dim_x, position.y + dim_y); //sx in alto
    glEnd();
    */
    if (has_texture) {
        texture.UnBind();
        glDisable(GL_TEXTURE_2D);
    }
    glPopAttrib();

    if (text != "") {
        gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0);
        glPushAttrib(GL_CURRENT_BIT);
        glColor4fv(text_color.toFloat4(c));
        g_output(screen2pixels(0, glutGet(GLUT_WINDOW_WIDTH)) - glutBitmapLength(FONT, (const unsigned char*)text.c_str()) / 2,
                 screen2pixels(0, glutGet(GLUT_WINDOW_HEIGHT)) + 8,
                 text);

        /*
        g_output(screen2pixels(position.x, glutGet(GLUT_WINDOW_WIDTH)) - glutBitmapLength(FONT, (const unsigned char*)text.c_str()) / 2,
                 screen2pixels(-position.y, glutGet(GLUT_WINDOW_HEIGHT)) + 8,
                 text);
        */

        glPopAttrib(); // This sets the colour back to its original value
    }
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void Quad2D::DrawQuad(double dx, double dy) {
    Quad2D q = Quad2D(dx, dy);
    q.drawOpenGL();
}

void Quad2D::DrawQuad(Vector2 v, double dx, double dy) {
    Quad2D q = Quad2D(v, dx, dy);
    q.drawOpenGL();
}

void Quad2D::DrawQuad(Vector2 v, double dx, double dy, Texture t) {
    Quad2D q = Quad2D(v, dx, dy);
    q.applyTexture(t);
    q.drawOpenGL();
}

void Quad2D::DrawQuad(Vector2 v, double dx, double dy, ColorRGBA c) {
    Quad2D q = Quad2D(v, dx, dy);
    q.applyColor(c);
    q.drawOpenGL();
}

void Quad2D::DrawQuad(Vector2 v, double dx, double dy, std::string s, ColorRGBA tx, ColorRGBA bg) {
    Quad2D q = Quad2D(v, dx, dy);
    q.applyColor(bg);
    q.applyText(s, tx);
    q.drawOpenGL();
}

void Quad2D::DrawQuad(Vector2 v, double dx, double dy, std::string s, ColorRGBA tx, Texture bg) {
    Quad2D q = Quad2D(v, dx, dy);
    q.applyTexture(bg);
    q.applyText(s, tx);
    q.drawOpenGL();
}

ColorRGBA Quad2D::getColor() { return ColorRGBA(bg_color); }
