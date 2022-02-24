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
#define Pesce0_OBJ "TrotaA_6"
#define Pesce1_OBJ "PersicoA_9"
#define Pesce2_OBJ "CarpaA_12"
#define Pesce3_OBJ "LuccioA_14"
#define Lago_OBJ "Laghetto_16"
#define MAX_Pesci 9
#define deg2rad(x) ((x) * 2 * PI / 360.0)

#define face_n(x) (((x) - ((x) % 3)) / 3)

typedef uint32_t uint;

int XRES = 800;
int YRES = 600;
double xBound = 1.4;
double yBound = 2.3;
bool left_button = false;
bool reorder = false;

map<string, GameObj> allGameObj;
map<string, Timer> allTimers;

Camera MainCamera;

ColorRGBA LightAmbient;
ColorRGBA LightDiffuse;
ColorRGBA LightSpecular;
Vector3 LightPosition;

const std::string pesci[MAX_Pesci] = {
        "TrotaA_6", "TrotaB_7", "TrotaC_8",
        "PersicoA_9", "PersicoB_10", "PersicoC_11",
        "CarpaA_12", "CarpaB_13",
        "LuccioA_14"
};

enum class read_state : unsigned int {
    read_start, read_new, read_vertices, read_uvs, read_normals, read_faces, read_lines
};

enum class pesce_state : unsigned int {
    pesce_null, pesce_inactive, pesce_active, pesce_interest, pesce_hooked, pesce_scared
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
    string newLine, name, mat, tex_path;
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
            s = ColorRGBA::Grey(0.1);
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
            has_Texture = true;
            tex_path = "models\\" + newLine.substr(7, newLine.length() - 7);
            tex = Texture(tex_path);
            if(!tex.isEmpty()) 
                std::cout << "LOG: new texture '(" << tex.getId() << ") " << tex.getPath() << "' found.\n";
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

void prepareOpenGL() {
    float ambient[4], diffuse[4], position[4];
    string texturePath;
    filebuf fb;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Enables blending and alpha value
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);		 // Enables Smooth Shading
    glClearColor(0.3f, 0.6f, 0.8f, 1.0f);
    glClearDepth(1.0f);				// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);		// Enables Depth Testing
    glDepthFunc(GL_LEQUAL);			// The Type Of Depth Test To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculation
    
    glEnable(GL_LIGHT0);    // Uses default lighting parameters
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_NORMALIZE);

    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient.toFloat4(ambient));
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse.toFloat4(diffuse));
    glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecular.toFloat4(diffuse));
    glLightfv(GL_LIGHT1, GL_POSITION, LightPosition.toArray4f(position));
    glEnable(GL_LIGHT1);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
}

void prepareScene() {
    const double SCALE = 1.0;
    allGameObj[Pugno_OBJ].hide();
    for (int i = 0; i < 9; i++) {
        allGameObj[pesci[i]].hide();
        allGameObj[pesci[i]].setState((int) pesce_state::pesce_inactive);
        allTimers[pesci[i]] = Timer();
    }

    allGameObj[Amo_OBJ].setBoundingBox2D(SCALE * 0.1, SCALE * 0.05);
    for (uint i = 0; i < 3; i++) {
        allGameObj[pesci[i]].setBoundingBox2D(SCALE * 0.25, SCALE * 0.5);
    }
    for (uint i = 3; i < 6; i++) {
        allGameObj[pesci[i]].setBoundingBox2D(SCALE * 0.2, SCALE * 0.3);
    }
    for (uint i = 6; i < 8; i++) {
        allGameObj[pesci[i]].setBoundingBox2D(SCALE * 0.25, SCALE * 0.7);
    }
    allGameObj[pesci[8]].setBoundingBox2D(SCALE * 0.25, SCALE * 1.3);
    //allGameObj[Lago_OBJ].setBoundingBox2D(0.8 * xBound, 0.8 * yBound);
    //allGameObj[Lago_OBJ].boundingBox.setOutside();

    LightAmbient = ColorRGBA(20, 20, 20, 255);
    LightDiffuse = ColorRGBA(127, 127, 127, 255);
    LightSpecular = ColorRGBA(0, 0, 0, 220);
    LightPosition = Vector3(0.0f, 0.0f, 5.0f);

    allTimers["spawnFish"] = Timer(5.0);
    allTimers["spawnFish"].start();

    MainCamera.lookAt(Vector3(-5.0, 0.0, 4.0), Vector3(0.0, 0.0, 0.0), Vector3(0.5, 0.0, 0.5));
    MainCamera.setPersp(1.0, XRES, YRES, 1000.0);
}

void updateScene(double deltaTime) {
    static uint pesci_ord[MAX_Pesci] = { 0, 3, 6, 1, 8, 4, 2, 7, 5 };
    static bool is_spawned[MAX_Pesci] = { false, false, false, false, false, false, false, false, false };
    static Vector3 floater = Vector3();
    static uint n_pesci = 0;
    static uint pesci_attivi = 0;
    static double theta;
    static const double posizioni[10][2] = {
        {-4/5 * xBound, 0.7 * yBound}, {-1/5 * xBound, 0.7 * yBound}, {0, 0.7 * yBound}, {1/5 * xBound, 0.7 * yBound}, {4/5 * xBound, 0.7 * yBound},
        {-4/5 * xBound,-0.7 * yBound}, {-1/5 * xBound,-0.7 * yBound}, {0,-0.7 * yBound}, {1/5 * xBound,-0.7 * yBound}, {4/5 * xBound,-0.7 * yBound}
    };
    double dist = allGameObj[Canna_OBJ].transform.position.distance(allGameObj[Amo_OBJ].transform.position);

    theta += deltaTime;
    floater.set(0, 0, sin(2 * PI * theta));
    if (dist > 0.4) {
        allGameObj[Amo_OBJ].setVelocity(floater + (allGameObj[Canna_OBJ].transform.position - allGameObj[Amo_OBJ].transform.position) * 0.4);
    } else if (dist < 0.05) {
        allGameObj[Amo_OBJ].setVelocity(floater + (allGameObj[Canna_OBJ].transform.position - allGameObj[Amo_OBJ].transform.position) * 0.01);
    } else allGameObj[Amo_OBJ].setVelocity(floater);
    allGameObj[Amo_OBJ].checkCollision(xBound, yBound, 0.0);
    for (uint i = 0; i < MAX_Pesci; i++) {
        uint j = pesci_ord[i];
        pesce_state state = (pesce_state) allGameObj[pesci[j]].getState();
        switch (state) {
        case pesce_state::pesce_active:
            if (allGameObj[Amo_OBJ].transform.velocity.xy().magnitude() < 0.1) {
                if (allGameObj[pesci[j]].boundingBox.collide(allGameObj[Amo_OBJ].boundingBox.position)) {
                    std::cout << "LOG: Collision(1_point): (" << Amo_OBJ << ", " << pesci[j] << ")\n";
                    //allGameObj[pesci[j]].hide();
                    allGameObj[pesci[j]].setState((int)pesce_state::pesce_interest);
                    allGameObj[pesci[j]].rotate(-60, 0, 0);
                    allGameObj[pesci[j]].setVelocity(Vector3::Polar(1, deg2rad(allGameObj[pesci[j]].transform.rotation.z), deg2rad(allGameObj[pesci[j]].transform.rotation.y)));
                    if (allTimers[pesci[j]].isCounting()) allTimers[pesci[j]].stop();
                    allTimers[pesci[j]].start(3.0);
                    pesci_attivi--;
                }
            }
            //if(allGameObj[Amo_OBJ].checkCollision(allGameObj[pesci[i]])) std::cout << "LOG: Collision(2_box): (" << Amo_OBJ << ", " << pesci[i] << ")\n";
            if (allGameObj[pesci[j]].checkCollision(xBound - allGameObj[pesci[j]].boundingBox.dimensions.x, yBound - allGameObj[pesci[j]].boundingBox.dimensions.y, 0.0)) {
                double speed = 0.1;
                if (j == 0 && j < 3) speed = 0.3;
                else if (j >= 3 && j < 6) speed = 0.5;
                else if (j >= 6 && j < 8) speed = 0.2;
                else if (j == 8) speed = 0.1;
                allGameObj[pesci[j]].rotate(0, 0, 90 + rand() % 60);
                allGameObj[pesci[j]].setVelocity(Vector3::Polar(speed, deg2rad(allGameObj[pesci[j]].transform.rotation.z), 0));
            }
            break;
        case pesce_state::pesce_interest:
            if (allGameObj[Amo_OBJ].transform.velocity.xy().magnitude() > 0.0) {
                allGameObj[pesci[j]].setState((int)pesce_state::pesce_scared);
                allGameObj[pesci[j]].rotate(60, 0, 0);
                allGameObj[pesci[j]].setVelocity(Vector3::Polar(1.2, deg2rad(allGameObj[pesci[j]].transform.rotation.z), 0));
                if (allTimers[pesci[j]].isCounting()) allTimers[pesci[j]].stop();
                allTimers[pesci[j]].start(4.0);
            } else {
                if (allTimers[pesci[j]].isCounting()) {
                    allGameObj[pesci[j]].setVelocity(allGameObj[pesci[j]].transform.velocity * sin(theta));
                } else {
                    //game_state = skill_check;
                    //if(skillCheck()) {
                    allGameObj[pesci[j]].setState((int)pesce_state::pesce_hooked);
                    allGameObj[pesci[j]].rotate(-30, 0, 0);
                    allGameObj[Amo_OBJ].translate(0, 0, 1.0);
                }
            }
            break;
        case pesce_state::pesce_hooked:
            allGameObj[pesci[j]].place(allGameObj[Amo_OBJ].transform.position);
            if (allGameObj[Amo_OBJ].transform.position.xy().magnitude() <= 0.1) {
                allGameObj[pesci[j]].setState((int) pesce_state::pesce_inactive);
                allGameObj[pesci[j]].hide();
                allGameObj[Amo_OBJ].translate(0, 0, -1.0);
                // aggiorna punteggio
            }
            break;
        case pesce_state::pesce_scared:
            if (allTimers[pesci[i]].isCounting()) {
                if (allGameObj[pesci[j]].checkCollision(xBound - allGameObj[pesci[j]].boundingBox.dimensions.x, yBound - allGameObj[pesci[j]].boundingBox.dimensions.y, 0.0)) {
                    allGameObj[pesci[j]].rotate(0, 0, 90 + rand() % 60);
                    allGameObj[pesci[j]].setVelocity(Vector3::Polar(1.2, deg2rad(allGameObj[pesci[j]].transform.rotation.z), 0));
                }
            } else allGameObj[pesci[j]].setState((int) pesce_state::pesce_active);
            break;
        case pesce_state::pesce_inactive: default: break;
        }
    }
    if (!allTimers["spawnFish"].isCounting()){
        if (pesci_attivi < 6) {
            int i = pesci_ord[n_pesci % MAX_Pesci];
            int p = n_pesci % 10;
            allGameObj[pesci[i]].place(posizioni[p][0], posizioni[p][1], 0);
            allGameObj[pesci[i]].rotate(0, 0, rand() % 180);
            allGameObj[pesci[i]].setVelocity(Vector3::Polar(0.4, deg2rad(allGameObj[pesci[i]].transform.rotation.z), 0));
            allGameObj[pesci[i]].setState((int) pesce_state::pesce_active);
            allGameObj[pesci[i]].show();
            n_pesci++;
            pesci_attivi++;
            std::cout << "LOG: Spawned fish in (" << posizioni[p][0] << ", " << posizioni[p][1] << ") [" << n_pesci << "].\n";
        }
        allTimers["spawnFish"].start();
    }
}

void doMotion() {
    static Vector3 left = Vector3(0, -1, 0);
    static int prev_time = 0;
    const float middle = 0.3;
    double deltaTime;
    int time = glutGet(GLUT_ELAPSED_TIME);

    deltaTime = ((double)time - (double)prev_time) * 0.001; // in seconds
    prev_time = time;

    MainCamera.lookAt(MainCamera.position, 
                      allGameObj[Canna_OBJ].transform.position * middle, 
                      MainCamera.forward % left);

    allGameObj[Amo_OBJ].move(deltaTime);
    for (int i = 0; i < MAX_Pesci; i++) {
        pesce_state ps = (pesce_state) allGameObj[pesci[i]].getState();
        switch (ps){
        case pesce_state::pesce_active: case pesce_state::pesce_interest: case pesce_state::pesce_scared:
            allGameObj[pesci[i]].move(deltaTime);
            allTimers[pesci[i]].pass(deltaTime);
        }
    }

    allTimers["spawnFish"].pass(deltaTime);

    updateScene(deltaTime);
    glutPostRedisplay();
}

void renderScene() {
    static bool once = true;
    static string* orderedObjs;
    static Vector2 dim = Vector2();
    int id = -1;

    if (reorder) once = true;
    if (once) {
        // ordina gli elementi in base al numero contenuto nel nome
        once = false; // l'ordinamento va fatto solo una volta, 
        if(reorder) delete[] orderedObjs;
        reorder = false;
        orderedObjs = new string[allGameObj.size()];
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

    //doMotion();
    glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y) {
    static uint n = 0;
    n++;
    switch (key) { 
    // W A S D
    case 'w': case 'W':
        break;
    case 'a': case 'A':
        break;
    case 's': case 'S':
        break;
    case 'd': case 'D':
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
    on_screen.set((0.5 - (float)y / (float)yres) * 2 * (xBound - 0.1), (0.5 - (float)x / (float)xres) * 2 * (yBound - 0.1), 0.0);
    if (left_button) {
        canna_xy.set(allGameObj[Canna_OBJ].transform.position.x, allGameObj[Canna_OBJ].transform.position.y, 0);
        delta = on_screen - canna_xy;
        allGameObj[Canna_OBJ].translate(delta);
        allGameObj[Rocchetto_OBJ].translate(delta);
        allGameObj[Pugno_OBJ].translate(delta);

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

