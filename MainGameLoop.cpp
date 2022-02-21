// FishingApp.cpp : This file contains the 'main' function. Program execution begins and ends there. //
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>

#include <GL/glut.h>

#include "GameObj.h"
#include "Timer.h"

#define Canna_OBJ "Canna_0"
#define Amo_OBJ "Amo_1"
#define Rocchetto_OBJ "Rocchetto_3"
#define Mano_OBJ "Mano_4"
#define Pugno_OBJ "Pugno_5"
#define Pesce0_OBJ "Trota_6"
#define Pesce1_OBJ "Persico_7"
#define Pesce2_OBJ "Carpa_8"
#define Pesce3_OBJ "Luccio_9"
#define Lago_OBJ "Laghetto_11"

#define face_n(x) (((x) - ((x) % 3)) / 3)

typedef uint32_t uint;

int XRES = 800;
int YRES = 600;
double xBound = 1.6;
double yBound = 1.2;
bool left_button = false;

map<string, GameObj> allGameObj;
map<int, Timer> allTimers;

Camera MainCamera;

ColorRGBA LightAmbient;
ColorRGBA LightDiffuse;
Vector3 LightPosition;

enum class read_state : unsigned int {
    read_start, read_new, read_vertices, read_uvs, read_normals, read_faces, read_lines
};

bool stringMatch(string s, const char* match) {
    return (s.find(match, 0, strlen(match)) == 0);
}

bool init(string path) {
    ColorRGBA nullColor = ColorRGBA(0, 0, 0, 0);
    map<string, Material> allMats;
    vector<Vector3> allVertices, allNormals, temp_vert, temp_norm;
    vector<Vector2> allUVs, temp_uv;
    vector<int> temp_face, temp_line;
    filebuf fb1, fb2;
    string newLine, name, mat;
    bool first = true, has_Texture = false;
    float x, y, z;
    ColorRGBA a, d, s, e;
    Texture tex;
    uint v1, v2, v3;
    uint n1, n2, n3;
    uint uv1, uv2, uv3;
    uint start = 0;
    read_state read_status = read_state::read_start;
    

    if (path.find(".obj", path.length() - 4) == path.npos || !fb1.open(path, ios::in)) {
        std::cout << "ERROR: file " << path << " not opened.\n";
        return false;
    }
    path.erase(path.length() - 4);
    path.append(".mtl");
    if (!fb2.open(path, ios::in)) {
        std::cout << "ERROR: file " << path << " not opened.\n";
        return false;
    }

    istream objfile(&fb1), mtlfile(&fb2);
    do {
        getline(mtlfile, newLine);
        if (stringMatch(newLine, "newmtl ")) {
            if (!first) {
                allMats[name] = Material(d, s, a, e);
                if (has_Texture) allMats[name].addTexture(tex);
            } else first = false;
            name.assign(newLine.substr(7, newLine.length() - 7));
            std::cout << "LOG: new material '" << name << "'.\n";
            a = ColorRGBA::White();
            d = ColorRGBA::Grey(0.8);
            s = ColorRGBA::Grey(0.5);
            e = ColorRGBA::Black();
            has_Texture = false;
        } else if (stringMatch(newLine, "Ka ")) {
            sscanf_s(newLine.c_str(), "Ka %f %f %f\n", &x, &z, &y);
            a.set(x, y, z, 1.0);
        } else if (stringMatch(newLine, "Kd ")) {
            sscanf_s(newLine.c_str(), "Kd %f %f %f\n", &x, &z, &y);
            d.set(x, y, z, 1.0);
        } else if (stringMatch(newLine, "Ks ")) {
            sscanf_s(newLine.c_str(), "Ks %f %f %f\n", &x, &z, &y);
            s.set(x, y, z, 1.0);
        } else if (stringMatch(newLine, "Ke ")) {
            sscanf_s(newLine.c_str(), "Ke %f %f %f\n", &x, &z, &y);
            e.set(x, y, z, 1.0);
        } else if (stringMatch(newLine, "d ")) {
            sscanf_s(newLine.c_str(), "d %f\n", &x);
            a.setAlpha(x);
            d.setAlpha(x);
            s.setAlpha(x);
            e.setAlpha(x);
        } else if (stringMatch(newLine, "map_Kd ")) {
            tex = Texture(".\\models\\" + newLine.substr(7, newLine.length() - 7));
            std::cout << "LOG: new texture '" << tex.getPath() << "' found.\n";
        }
    } while (!mtlfile.eof());
    allMats[name] = Material(d, s, a, e);
    if (has_Texture) allMats[name].addTexture(tex);
    std::cout << "LOG: found all materials\n"; first = true;
    do {
        getline(objfile, newLine);
        if (stringMatch(newLine, "o ")) {
            if (read_status != read_state::read_new) {
                if (!first) {
                    if (read_status == read_state::read_faces) {
                        allGameObj[name].addMesh(Mesh(temp_vert, temp_norm, temp_uv, temp_face));
                    } else if (read_status == read_state::read_lines) {
                        allGameObj[name].addCurve(Curve(temp_vert, temp_line));
                    }
                } else first = false;
                read_status = read_state::read_new;
            }
            name.assign(newLine.substr(2, newLine.length() - 2));
            std::cout << "LOG: new object '" << name << "'.\n";
            allGameObj[name] = GameObj(name);
        } else if (stringMatch(newLine, "v ")) {
            if (read_status != read_state::read_vertices) {
                read_status = read_state::read_vertices;
                allGameObj[name].addMesh(Mesh());
                if (allVertices.size() == 0) start = 1;
                else start = allVertices.size();
            }
            sscanf_s(newLine.c_str(), "v %f %f %f\n", &x, &y, &z);
            allVertices.push_back(Vector3(x, y, z));
        } else if (stringMatch(newLine, "vt ")) {
            if (read_status != read_state::read_uvs) read_status = read_state::read_uvs;
            sscanf_s(newLine.c_str(), "vt %f %f\n", &x, &y);
            allUVs.push_back(Vector2(x, y));
        } else if (stringMatch(newLine, "vn ")) {
            if (read_status != read_state::read_normals) read_status = read_state::read_normals;
            sscanf_s(newLine.c_str(), "vn %f %f %f\n", &x, &y, &z);
            allNormals.push_back(Vector3(x, y, z));
        } else if (stringMatch(newLine, "f ")) {
            if (read_status != read_state::read_faces) {
                read_status = read_state::read_faces;
                temp_vert.clear();
                temp_norm.clear();
                temp_uv.clear();
                temp_face.clear();
            }
            sscanf_s(newLine.c_str(), "f %u/%u/%u %u/%u/%u %u/%u/%u\n", &v1, &uv1, &n1, &v2, &uv2, &n2, &v3, &uv3, &n3);
            allGameObj[name].materials[face_n(temp_face.size())] = allMats[mat];
            temp_face.push_back(v1 - start);
            if (temp_vert.size() <= v1 - start) 
                temp_vert.resize(v1 - start + 1);
            temp_vert[v1 - start] = allVertices[v1 - 1];
            if (temp_norm.size() <= v1 - start) 
                temp_norm.resize(v1 - start + 1);
            temp_norm[v1 - start] = allNormals[n1 - 1];
            if (temp_uv.size() <= v1 - start) 
                temp_uv.resize(v1 - start + 1);
            temp_uv[v1 - start] = allUVs[uv1 - 1];
            temp_face.push_back(v2 - start);
            if (temp_vert.size() <= v2 - start) 
                temp_vert.resize(v2 - start + 1);
            temp_vert[v2 - start] = allVertices[v2 - 1];
            if (temp_norm.size() <= v2 - start) 
                temp_norm.resize(v2 - start + 1);
            temp_norm[v2 - start] = allNormals[n2 - 1];
            if (temp_uv.size() <= v2 - start) 
                temp_uv.resize(v2 - start + 1);
            temp_uv[v2 - start] = allUVs[uv2 - 1];
            temp_face.push_back(v3 - start);
            if (temp_vert.size() <= v3 - start) 
                temp_vert.resize(v3 - start + 1);
            temp_vert[v3 - start] = allVertices[v3 - 1];
            if (temp_norm.size() <= v3 - start) 
                temp_norm.resize(v3 - start + 1);
            temp_norm[v3 - start] = allNormals[n3 - 1];
            if (temp_uv.size() <= v3 - start) 
                temp_uv.resize(v3 - start + 1);
            temp_uv[v3 - start] = allUVs[uv3 - 1];
        } else if (stringMatch(newLine, "l ")) {
            if (read_status != read_state::read_lines) {
                read_status = read_state::read_lines;
                temp_vert.clear();
                temp_line.clear();
            }
            sscanf_s(newLine.c_str(), "l %u %u\n", &v1, &v2);
            temp_line.push_back(v1 - start);
            if (temp_vert.size() <= v1 - start) temp_vert.resize(v1 - start + 1);
            temp_vert[v1 - start] = allVertices[v1 - 1];
            temp_line.push_back(v2 - start);
            if (temp_vert.size() <= v2 - start) temp_vert.resize(v2 - start + 1);
            temp_vert[v2 - start] = allVertices[v2 - 1];
        } else if (stringMatch(newLine, "usemtl ")) {
            mat.assign(newLine.substr(7, newLine.length() - 7));
            std::cout << "LOG: '" << mat << "' material found.\n";
        }
    } while (!objfile.eof());
    if (read_status == read_state::read_faces) {
        allGameObj[name].addMesh(Mesh(temp_vert, temp_norm, temp_uv, temp_face));
    } else if (read_status == read_state::read_lines) {
        allGameObj[name].addCurve(Curve(temp_vert, temp_line));
    }

    return true;
}

void prepareScene() {
    allGameObj[Pugno_OBJ].hide();

    LightAmbient = ColorRGBA(127, 127, 127, 255);
    LightDiffuse = ColorRGBA(255, 255, 255, 255);
    LightPosition = Vector3(0.0f, 0.0f, 15.0f);

    MainCamera.lookAt(Vector3(-5.0, 0.0, 3.0), Vector3(0.0, 0.0, 0.0), Vector3(0.866, 0.0, 0.5));
    MainCamera.setPersp(1.0, XRES, YRES, 1000.0);
}

void prepareOpenGL() {
    float ambient[4], diffuse[4], position[4];
    string texturePath;
    filebuf fb;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Enables blending and alpha value
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);		 // Enables Smooth Shading
    glClearColor(0.1f, 0.8f, 0.6f, 1.0f);
    glClearDepth(1.0f);				// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);		// Enables Depth Testing
    glDepthFunc(GL_LEQUAL);			// The Type Of Depth Test To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculation
    
    glEnable(GL_LIGHT0);    // Uses default lighting parameters
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_NORMALIZE);

    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient.toFloat4(ambient));
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse.toFloat4(diffuse));
    glLightfv(GL_LIGHT1, GL_POSITION, LightPosition.toArray4f(position));
    glEnable(GL_LIGHT1);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
}

void doMotion() {
    static Vector3 left = Vector3(0, -1, 0);
    static int prev_time = 0;
    const float middle = 0.3;
    float deltaTime;
    int time = glutGet(GLUT_ELAPSED_TIME);

    deltaTime = (time - prev_time) * 0.001; // in seconds
    prev_time = time;

    MainCamera.lookAt(MainCamera.position, 
                      allGameObj[Canna_OBJ].transform.position * middle, 
                      MainCamera.forward % left);

    allGameObj[Amo_OBJ].move(deltaTime);
    if (allGameObj[Amo_OBJ].transform.position.x > xBound)
        allGameObj[Amo_OBJ].transform.position.x = xBound - 0.001;
    else if (allGameObj[Amo_OBJ].transform.position.x < -xBound)
        allGameObj[Amo_OBJ].transform.position.x = -xBound + 0.001;
    if (allGameObj[Amo_OBJ].transform.position.y > yBound)
        allGameObj[Amo_OBJ].transform.position.y = yBound - 0.001;
    else if (allGameObj[Amo_OBJ].transform.position.y < -yBound) 
        allGameObj[Amo_OBJ].transform.position.y = -yBound + 0.001;
    if (allGameObj[Amo_OBJ].transform.position.z != 0.0)
        allGameObj[Amo_OBJ].transform.position.z = 0.0;
    

    /*
    for (Timer t : allTimers) {
        t.pass(deltaTime);
    }*/

    glutPostRedisplay();
}

void renderScene() {
    static bool once = true;
    static string* orderedObjs = new string[allGameObj.size()];
    int id = -1;

    if (once) {
        // ordina gli elementi in base al numero contenuto nel nome
        once = false; // l'ordinamento va fatto solo una volta, 
        for (auto elem : allGameObj) {
            id = -1;
            for (uint i = 0; i < elem.first.length(); i++) {
                char c = elem.first.at(i);
                if (c >= '0' && c <= '9') {
                    if (id == -1) id = c - '0'; // una sola cifra => id < 10
                    else {
                        // altrimenti id > 10, quindi la prima viene considerata per le decine, la seconda cifra per le unità
                        id *= 10;
                        id += c - '0';
                    }
                }
            }
            // l'ordinamento viene eseguito solo sulle stringhe dei nomi, così da non duplicare i dati
            orderedObjs[id].assign(elem.first);
            std::cout << "LOG: rendering (" << id << ")->'" << elem.first << "'.\n"; // stampa per testing, penso di lasciarla
        } 
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(MainCamera.position.x, MainCamera.position.y, MainCamera.position.z,
              MainCamera.focus.x, MainCamera.focus.y, MainCamera.focus.z,
              MainCamera.up.x, MainCamera.up.y, MainCamera.up.z);
    glClear(GL_COLOR_BUFFER_BIT);
    for (uint i = 0; i < allGameObj.size(); i++) {
        // gli oggetti vengono renderizzati nell'ordine stabilito dentro 'orderedObjs'
        allGameObj[orderedObjs[i]].renderOpenGL();
    }

    glutSwapBuffers();
    //doMotion();
}

void processNormalKeys(unsigned char key, int x, int y) {
    static uint n = 0;
    n++;
    switch (key) { 
    // W A S D
    case 'w':
        break;
    case 'a':
        break;
    case 's':
        break;
    case 'd':
        break;
    // SPACE
    case ' ':
        break;
    default: break;
    }
    std::cout << "( " << n << " ) " << key << "\n";
}
/*
void processSpecialKeys(int key, int x, int y) {
    switch (key) {
    default: break;
    }
}
*/

void mouse(int button, int state, int x, int y) {
    switch (button) {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN) {
            left_button = true;
            glutSetCursor(GLUT_CURSOR_NONE);
            allGameObj[Mano_OBJ].hide();
            allGameObj[Pugno_OBJ].show();
        }
        else if (state == GLUT_UP) {
            left_button = false;
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
            allGameObj[Pugno_OBJ].hide();
            allGameObj[Mano_OBJ].show();
        }
        break;
    case GLUT_RIGHT_BUTTON:
        if (state == GLUT_DOWN) allGameObj[Lago_OBJ].hide();
        else if (state == GLUT_UP) allGameObj[Lago_OBJ].show();
        break;
    case GLUT_MIDDLE_BUTTON:
        //if (state == GLUT_DOWN)
        //else if (state == GLUT_UP)
        break;
    default: break;
    }
}

void drag_n_drop(int x, int y) {
    static uint8_t calls = 0;
    //static vector<float[3]> positions = vector<float[3]>(40);
    bool drag = false;
    int xres = glutGet(GLUT_WINDOW_WIDTH);
    int yres = glutGet(GLUT_WINDOW_HEIGHT);
    static Vector3 delta = Vector3(), on_screen = Vector3(), canna_xy = Vector3();

    if (x > xres) x = xres - 1;
    if (y > yres) y = yres - 1;
    if (x < 0) x = 1;
    if (y < 0) y = 1;
    on_screen.set((0.5 - (float)y / (float)yres) * 2.4f, (0.5 - (float)x / (float)xres) * 3.6f, 0.0);
    if (left_button) {
        canna_xy.set(allGameObj[Canna_OBJ].transform.position.x, allGameObj[Canna_OBJ].transform.position.y, 0);
        delta = on_screen - canna_xy;
        allGameObj[Canna_OBJ].translate(delta);
        allGameObj[Rocchetto_OBJ].translate(delta);
        allGameObj[Pugno_OBJ].translate(delta);
        MainCamera.focus = allGameObj[Canna_OBJ].transform.position * 0.4;
        if (allGameObj[Canna_OBJ].transform.position.distance(allGameObj[Amo_OBJ].transform.position) > 1.0) {
            allGameObj[Amo_OBJ].applyForce(allGameObj[Canna_OBJ].transform.position - allGameObj[Amo_OBJ].transform.position);
        }

        //std::cout << "Amo.vel(" << allGameObj[Amo_OBJ].transform.velocity.x << ", " << allGameObj[Amo_OBJ].transform.velocity.y << ")\n";
    }
}

void changeSize(int xres, int yres) {
    MainCamera.lookAt(MainCamera.position, MainCamera.focus, MainCamera.up);
    MainCamera.setPersp(1.0, xres, yres, 1000.0);
    glViewport(0, 0, xres, yres);
}

int main(int argc, char* argv[]) {
    /* test variables
    GameObj triangle = GameObj("triangle");
    vector<Vector3> vertsTest = vector<Vector3>(3);
    vector<Vector3> normsTest = vector<Vector3>(3);
    vector<Vector2> uvsTest = vector<Vector2>(3);
    vector<int> facesTest = vector<int>(3);
    vertsTest[0] = Vector3(0, 0, 0);
    vertsTest[1] = Vector3(0, 1, 0);
    vertsTest[2] = Vector3(1, 0, 0);
    normsTest[0] = Vector3(0, 0, -1);
    normsTest[1] = Vector3(0, 0, -1);
    normsTest[2] = Vector3(0, 0, -1);
    uvsTest[0] = Vector2(0, 0);
    uvsTest[1] = Vector2(0, 1);
    uvsTest[2] = Vector2(1, 0);
    facesTest[0] = 0;
    facesTest[1] = 1;
    facesTest[2] = 2;
    triangle.addMesh(Mesh(vertsTest, normsTest, uvsTest, facesTest));
    triangle.colors[0] = ColorRGBA(255, 0, 0, 255);
    allGameObj[triangle.getName()] = triangle;
    */
    // init GLUT and create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(XRES, YRES);
    glutCreateWindow("Fishing App");

    // register callbacks
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutIdleFunc(doMotion);
    glutKeyboardFunc(processNormalKeys);
    //glutSpecialFunc(processSpecialKeys);
    glutMouseFunc(mouse);
    glutMotionFunc(drag_n_drop);

    // OpenGL init
    glEnable(GL_DEPTH_TEST);
    if (!init(".\\models\\laghetto.obj")) {
        fprintf(stderr, "Initialization failed");
        return -1;
    }

    prepareScene();
    prepareOpenGL();
    glutGet(GLUT_ELAPSED_TIME);
    // enter GLUT event processing cycle
    glutMainLoop();

	return 0;
}

