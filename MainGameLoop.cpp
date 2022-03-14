// FishingApp.cpp : This file contains the 'main' function. Program execution begins and ends there. //
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cctype>

#include <GL/glut.h>
//#include <irrKlang.h>
#include <Windows.h>

#include "GameObj.h"
#include "Quad2D.h"
#include "Timer.h"

#define MAX_Pesci 9
#define SEC_Duration 40

#define deg2rad(x) ((x) * (2 * PI) / 360.0)
#define rad2deg(x) ((x) * 360.0 / (2 * PI))
#define face_n(x) (((x) - ((x) % 3)) / 3)

#define Canna_OBJ "Canna_0"
#define Amo_OBJ "Amo_1"
#define Filo_OBJ "Filo_2"
#define Rocchetto_OBJ "Rocchetto_3"
#define Mano_OBJ "Mano_4"
#define Pugno_OBJ "Pugno_5"
#define Splash_OBJ "Splash_17"
#define Lago_OBJ "Laghetto_18"

enum class GameState : unsigned int {
    GameState_TitleScreen, GameState_Play, GameState_SkillCheck, GameState_Pause, GameState_End
};

enum class read_state : unsigned int {
    read_start, read_new, read_vertices, read_uvs, read_normals, read_faces, read_lines
};

enum class pesce_state : unsigned int {
    pesce_null, pesce_inactive, pesce_active, pesce_interest, pesce_hooked, pesce_scared
};

enum class menu_state : unsigned int {
    menu, play, setDifficulty, scoreboard
}; // to define state of our game

enum class difficulty_level : unsigned int {
    easy, medium, hard
};

typedef uint32_t uint;

typedef struct button_t {
    bool over; /* TRUE if polygon exists */
    float xmin, xmax, ymin, ymax; /* bounding box */
    std::string text;
    float tx, ty;
} Button;

typedef struct {
    tm timestamp;
    int score;
} Scoreboard;

int n_buttons = 4, n_difficulty_buttons = 3, n_score = 5;

menu_state CurrentState = menu_state::menu; // initiation global variable
difficulty_level difficulty = difficulty_level::medium;

Texture background;

//Button ;
Quad2D buttons[4], back, difficulty_buttons[3], score_buttons[5];
Scoreboard score_easy[5], score_medium[5], score_hard[5];

void* font = GLUT_BITMAP_TIMES_ROMAN_24;
int XRES = 800;
int YRES = 600;
double xBound = 1.4;
double yBound = 2.3;

map<string, GameObj> allGameObj;
map<string, Timer> allTimers;

Camera MainCamera;

float LightAmbient[4];
float LightDiffuse[4];
float LightPosition[4];
float LightSpecular[4];

//irrklang::ISoundEngine* soundEngine;

bool left_button = false;
bool skill_check_successful = false;
bool skill_check_failed = false;
const char* score_easy_filename = ".\\score_easy.txt";
const char* score_medium_filename = ".\\score_medium.txt";
const char* score_hard_filename = ".\\score_hard.txt";
bool check_sequence_generation = false;
std::string skill_check_sequence;
int sequence_length = 4;
double skill_check_timer_count = 5000;

GameState game_state = GameState::GameState_TitleScreen;
int game_score = 0;
int game_time = SEC_Duration;


const std::string pesci[MAX_Pesci] = {
        "Trota0_6", "Trota1_7", "Trota2_8",
        "Persico0_9", "Persico1_10", "Persico2_11",
        "Carpa0_12", "Carpa1_13",
        "Luccio0_14"
};

void changeSize(int w, int h);

void setFloat4(float v[4], float x0, float x1, float x2, float x3) {
    v[0] = x0; v[1] = x1; v[2] = x2; v[3] = x3;
}

bool stringMatch(string s, const char* match) {
    return (s.find(match, 0, strlen(match)) == 0);
}

void playSound(std::string path) {
    //soundEngine->play2D(path.c_str());
    //PlaySound(LPCWSTR(path.c_str()), NULL, SND_ASYNC | SND_FILENAME);
}

void loopSound(std::string path) {
    //soundEngine->play2D(path.c_str(), true);
    //PlaySound(LPCWSTR(path.c_str()), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
}

tm FixMonth(tm ts, int flag) {
    if (flag == 1)
        ts.tm_mon -= 1;
    else
        ts.tm_mon += 1;

    return ts;
}

float FromPixelToNormalized(int pixel, int flag) {
    float f = pixel;

    if (flag == 1)
        return f / (XRES / 2) - 1.0;
    return -(f / (YRES / 2) - 1.0);
}

int FromNormalizedToPixel(float norm, int flag) {
    if (flag == 1)
        return (norm + 1) * XRES / 2;
    return (1 - norm) * YRES / 2;
}

std::string genRandom(const int len) {
    static const char wasd[] = "wasd";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i)
        tmp_s += wasd[rand() % (sizeof(wasd) - 1)];

    return tmp_s;
}

void output(int x, int y, std::string str) {
    int len, i;

    //glWindowPos2i(x + off_txt_x, y + off_txt_y);
    glRasterPos2i(x, y);
    len = str.length();

    for (i = 0; i < len; i++)
        glutBitmapCharacter(font, str[i]);

    //glutPostRedisplay();
}

void InitMenu() {
    buttons[0] = Quad2D(Vector2(0, 0.2), 0.8, 0.1);
    buttons[0].applyText("Play", ColorRGBA::White());

    buttons[1] = Quad2D(Vector2(0, 0.0), 0.8, 0.1);
    buttons[1].applyText("Set difficulty", ColorRGBA::White());

    buttons[2] = Quad2D(Vector2(0,-0.2), 0.8, 0.1);
    buttons[2].applyText("Scoreboard", ColorRGBA::White());

    buttons[3] = Quad2D(Vector2(0,-0.8), 0.8, 0.1);
    buttons[3].applyText("Quit", ColorRGBA::White());

    difficulty_buttons[0] = Quad2D(Vector2(0, 0.20), 0.8, 0.1);
    difficulty_buttons[0].applyText("Easy", ColorRGBA::White());

    difficulty_buttons[1] = Quad2D(Vector2(0, 0.0), 0.8, 0.1);
    difficulty_buttons[1].applyText("Medium", ColorRGBA::White());

    difficulty_buttons[2] = Quad2D(Vector2(0, -0.20), 0.8, 0.1);
    difficulty_buttons[2].applyText("Hard", ColorRGBA::White());

    back = Quad2D(Vector2(0.8, 0.8), 0.1, 0.1 * XRES / YRES);
    back.applyTexture(Texture(".\\models\\textures\\back_button_2.png"));

    score_buttons[0] = Quad2D(Vector2(0, 0.2), 0.2, 0.1);
    score_buttons[0].applyColor(ColorRGBA::Grey(0.2));

    score_buttons[1] = Quad2D(Vector2(0, 0.0), 0.2, 0.1);
    score_buttons[1].applyColor(ColorRGBA::Grey(0.2));

    score_buttons[2] = Quad2D(Vector2(0, -0.2), 0.2, 0.1);
    score_buttons[2].applyColor(ColorRGBA::Grey(0.2));

    score_buttons[3] = Quad2D(Vector2(0, -0.4), 0.2, 0.1);
    score_buttons[3].applyColor(ColorRGBA::Grey(0.2));

    score_buttons[4] = Quad2D(Vector2(0, -0.6), 0.2, 0.1);
    score_buttons[4].applyColor(ColorRGBA::Grey(0.2));

    /*
    buttons[0].xmin = -0.4;
    buttons[0].xmax = 0.4;
    buttons[0].ymin = 0.2;
    buttons[0].ymax = 0.3;
    buttons[0].text = "Play";
    buttons[0].tx = FromPixelToNormalized(XRES / 2 - glutBitmapLength(font, (const unsigned char*)"Play") / 2, 1);
    buttons[0].ty = FromPixelToNormalized(YRES / 2 - 68, -1);

    buttons[1].xmin = -0.4;
    buttons[1].xmax = 0.4;
    buttons[1].ymin = 0;
    buttons[1].ymax = 0.1;
    buttons[1].text = "Set difficulty";
    buttons[1].tx = FromPixelToNormalized(XRES / 2 - glutBitmapLength(font, (const unsigned char*)"Set difficulty") / 2, 1);
    buttons[1].ty = FromPixelToNormalized(YRES / 2 - 8, -1);

    buttons[2].xmin = -0.4;
    buttons[2].xmax = 0.4;
    buttons[2].ymin = -0.2;
    buttons[2].ymax = -0.1;
    buttons[2].text = "Scoreboard";
    buttons[2].tx = FromPixelToNormalized(XRES / 2 - glutBitmapLength(font, (const unsigned char*)"Scoreboard") / 2, 1);
    buttons[2].ty = FromPixelToNormalized(YRES / 2 + 50, -1);

    buttons[3].xmin = -0.4;
    buttons[3].xmax = 0.4;
    buttons[3].ymin = -0.8;
    buttons[3].ymax = -0.7;
    buttons[3].text = "Quit";
    buttons[3].tx = FromPixelToNormalized(XRES / 2 - glutBitmapLength(font, (const unsigned char*)"Quit") / 2, 1);
    buttons[3].ty = FromPixelToNormalized(YRES - 68, -1);
    */


    /*
    difficulty_buttons[0].xmin = -0.4;
    difficulty_buttons[0].xmax = 0.4;
    difficulty_buttons[0].ymin = 0.15;
    difficulty_buttons[0].ymax = 0.25;
    difficulty_buttons[0].text = "Easy";
    difficulty_buttons[0].tx = FromPixelToNormalized(XRES / 2 - glutBitmapLength(font, (const unsigned char*)"Easy") / 2, 1);
    difficulty_buttons[0].ty = FromPixelToNormalized(YRES / 2 - 52, -1);

    difficulty_buttons[1].xmin = -0.4;
    difficulty_buttons[1].xmax = 0.4;
    difficulty_buttons[1].ymin = -0.05;
    difficulty_buttons[1].ymax = 0.05;
    difficulty_buttons[1].text = "Medium";
    difficulty_buttons[1].tx = FromPixelToNormalized(XRES / 2 - glutBitmapLength(font, (const unsigned char*)"Medium") / 2, 1);
    difficulty_buttons[1].ty = FromPixelToNormalized(YRES / 2 + 8, -1);

    difficulty_buttons[2].xmin = -0.4;
    difficulty_buttons[2].xmax = 0.4;
    difficulty_buttons[2].ymin = -0.25;
    difficulty_buttons[2].ymax = -0.15;
    difficulty_buttons[2].text = "Hard";
    difficulty_buttons[2].tx = FromPixelToNormalized(XRES / 2 - glutBitmapLength(font, (const unsigned char*)"Hard") / 2, 1);
    difficulty_buttons[2].ty = FromPixelToNormalized(YRES / 2 + 68, -1);
    

    back.xmin = 0.9;
    back.xmax = 1;
    back.ymin = 0.9;
    back.ymax = 1.0;
    back.text = "X";
    back.tx = FromPixelToNormalized(XRES - glutBitmapLength(font, (const unsigned char*)"X") * 1.5, 1);
    back.ty = FromPixelToNormalized(24, -1);

    score_buttons[0].xmin = -0.3;
    score_buttons[0].xmax = 0.3;
    score_buttons[0].ymin = 0.15;
    score_buttons[0].ymax = 0.25;
    score_buttons[0].ty = FromPixelToNormalized(YRES / 2 - 52, -1);

    score_buttons[1].xmin = -0.3;
    score_buttons[1].xmax = 0.3;
    score_buttons[1].ymin = -0.05;
    score_buttons[1].ymax = 0.05;
    score_buttons[1].ty = FromPixelToNormalized(YRES / 2 + 8, -1);

    score_buttons[2].xmin = -0.3;
    score_buttons[2].xmax = 0.3;
    score_buttons[2].ymin = -0.25;
    score_buttons[2].ymax = -0.15;
    score_buttons[2].ty = FromPixelToNormalized(YRES / 2 + 68, -1);

    score_buttons[3].xmin = -0.3;
    score_buttons[3].xmax = 0.3;
    score_buttons[3].ymin = -0.45;
    score_buttons[3].ymax = -0.35;
    score_buttons[3].ty = FromPixelToNormalized(YRES / 2 + 128, -1);

    score_buttons[4].xmin = -0.3;
    score_buttons[4].xmax = 0.3;
    score_buttons[4].ymin = -0.65;
    score_buttons[4].ymax = -0.55;
    score_buttons[4].ty = FromPixelToNormalized(YRES / 2 + 188, -1);
    */

}

void ReadFile(const char* filename, difficulty_level difficulty) {
    Scoreboard sb;

    FILE* score_file;
    fopen_s(&score_file, filename, "r");

    for (int i = 0; fscanf_s(score_file, "%d/%d/%d %d:%d:%d %d", &(sb.timestamp.tm_year), &(sb.timestamp.tm_mon), &(sb.timestamp.tm_mday), &(sb.timestamp.tm_hour), &(sb.timestamp.tm_min), &(sb.timestamp.tm_sec), &(sb.score)) != EOF; i++) {
        sb.timestamp = FixMonth(sb.timestamp, 1);

        if (difficulty == difficulty_level::easy)
            score_easy[i] = sb;
        else if (difficulty == difficulty_level::medium)
            score_medium[i] = sb;
        else if (difficulty == difficulty_level::hard)
            score_hard[i] = sb;
    }


    fclose(score_file);
}

void WriteFile(const char* filename, difficulty_level difficulty) {
    Scoreboard sb;

    FILE* score_file;
    fopen_s(&score_file, filename, "w");

    for (int i = 0; i < n_score; i++) {
        if (difficulty == difficulty_level::easy)
            sb = score_easy[i];
        else if (difficulty == difficulty_level::medium)
            sb = score_medium[i];
        else if (difficulty == difficulty_level::hard)
            sb = score_hard[i];

        sb.timestamp = FixMonth(sb.timestamp, -1);
        fprintf(score_file, "%04d/%02d/%02d %02d:%02d:%02d %d\n", sb.timestamp.tm_year, sb.timestamp.tm_mon, sb.timestamp.tm_mday, sb.timestamp.tm_hour, sb.timestamp.tm_min, sb.timestamp.tm_sec, sb.score);
    }

    fclose(score_file);
}

void ReadScoreboard() {
    ReadFile(score_easy_filename, difficulty_level::easy);
    ReadFile(score_medium_filename, difficulty_level::medium);
    ReadFile(score_hard_filename, difficulty_level::hard);
}

void PrintScoreboard() {
    WriteFile(score_easy_filename, difficulty_level::easy);
    WriteFile(score_medium_filename, difficulty_level::medium);
    WriteFile(score_hard_filename, difficulty_level::hard);
}

void PickButton(int x, int y) {
    float ox = FromPixelToNormalized(x, 1);
    float oy = FromPixelToNormalized(y, -1);

    if (game_state == GameState::GameState_TitleScreen) {
        switch (CurrentState) {
        case menu_state::menu:
            for (int i = 0; i < n_buttons; i++) {
                if (ox >= buttons[i].position.x - buttons[i].dim_x && ox <= buttons[i].position.x + buttons[i].dim_x &&
                    oy >= buttons[i].position.y - buttons[i].dim_y && oy <= buttons[i].position.y + buttons[i].dim_y)
                    buttons[i].mouseOver(true);
                else buttons[i].mouseOver(false);
            }
            break;
        case menu_state::setDifficulty:
            for (int i = 0; i < n_difficulty_buttons; i++) {
                if (ox >= difficulty_buttons[i].position.x - difficulty_buttons[i].dim_x && ox <= difficulty_buttons[i].position.x + difficulty_buttons[i].dim_x &&
                    oy >= difficulty_buttons[i].position.y - difficulty_buttons[i].dim_y && oy <= difficulty_buttons[i].position.y + difficulty_buttons[i].dim_y)
                    difficulty_buttons[i].mouseOver(true);
                else difficulty_buttons[i].mouseOver(false);
            }

            if (ox >= back.position.x - back.dim_x && ox <= back.position.x + back.dim_x &&
                oy >= back.position.y - back.dim_y && oy <= back.position.y + back.dim_y)
                back.mouseOver(true);
            else back.mouseOver(false);
            break;
        case menu_state::scoreboard:
            if (ox >= back.position.x - back.dim_x && ox <= back.position.x + back.dim_x &&
                oy >= back.position.y - back.dim_y && oy <= back.position.y + back.dim_y)
                back.mouseOver(true);
            else back.mouseOver(false);
            break;
        default:
            break;
        }
    }
}

void stopGameTimer(int a) {
    static uint seconds = 0;
    seconds++;
    game_time--;
    if (seconds < SEC_Duration) {
        if (SEC_Duration - seconds == 10) playSound(".\\sounds\\clock_1.wav");
        if (SEC_Duration - seconds == 9) playSound(".\\sounds\\clock_2.wav");
        if (SEC_Duration - seconds == 8) playSound(".\\sounds\\clock_3.wav");
        if (SEC_Duration - seconds == 7) playSound(".\\sounds\\clock_4.wav");
        if (SEC_Duration - seconds == 1) playSound(".\\sounds\\wishtle.wav");
        std::cout << "LOG: " << seconds << " second passed.\n";
        allTimers["gameTimer"].start();
    } else {
        playSound(".\\sounds\\wishtle.wav");
        std::cout << "LOG: Match ended after " << std::to_string(SEC_Duration) << " sec!" << endl;
        std::cout << "LOG: score = " << game_score << endl;
        game_state = GameState::GameState_End;
    }
}

void myMouse(int button, int state, int x, int y) {
    float ox = FromPixelToNormalized(x, 1);
    float oy = FromPixelToNormalized(y, -1);

    int pressed = -1;

    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) {
            switch (CurrentState) {
            case menu_state::menu:
                for (int i = 0; i < n_buttons; i++) {
                    if(buttons[i].isOver())
                        pressed = i;
                }

                if (pressed == -1)
                    return;

                switch (pressed) {
                case 0:
                    CurrentState = menu_state::play;
                    break;
                case 1:
                    CurrentState = menu_state::setDifficulty;
                    break;
                case 2:
                    CurrentState = menu_state::scoreboard;
                    break;
                case 3:
                    PrintScoreboard();
                    exit(0);
                    break;
                default:
                    break;
                }
                break;
            case menu_state::setDifficulty:
                for (int i = 0; i < n_difficulty_buttons; i++) {
                    //if ((ox >= difficulty_buttons[i].xmin) && (ox <= difficulty_buttons[i].xmax) && (oy >= difficulty_buttons[i].ymin) && (oy <= difficulty_buttons[i].ymax))
                    if (difficulty_buttons[i].isOver())
                        pressed = i;
                }

                //if ((ox >= back.xmin) && (ox <= back.xmax) && (oy >= back.ymin) && (oy <= back.ymax))
                if(back.isOver())
                    pressed = -2;

                if (pressed == -1)
                    return;

                switch (pressed) {
                case 0:
                    // Setta difficoltà a EASY
                    difficulty = difficulty_level::easy;
                    sequence_length = 2;
                    break;
                case 1:
                    // Setta difficoltà a MEDIUM (default)
                    difficulty = difficulty_level::medium;
                    sequence_length = 4;
                    break;
                case 2:
                    // Setta difficoltà a HARD
                    difficulty = difficulty_level::hard;
                    sequence_length = 6;
                    break;
                default:
                    break;
                }
                CurrentState = menu_state::menu;
                break;
            case menu_state::scoreboard:
                //if ((ox >= back.xmin) && (ox <= back.xmax) && (oy >= back.ymin) && (oy <= back.ymax))
                if(back.isOver())
                    pressed = -2;
                if (pressed == -2)
                    CurrentState = menu_state::menu;
                break;
            default:
                break;
            }
        }
    }
}

void Menu() {
    static Texture tex = Texture(".\\models\\textures\\logo_tutorial.png");

    Quad2D::DrawQuad(Vector2(), 2, 2, background);
    //Quad2D::DrawQuad(Vector2(-0.7, 0.3), 0.4, 0.4, "provaprovaprova", ColorRGBA::Yellow(), tex);
    //Quad2D::DrawQuad(Vector2(0.7, 0.5), 0.7, 0.2, "provaprovaprova", ColorRGBA::Yellow(), ColorRGBA());

    Quad2D::DrawQuad(Vector2(0, 0.6), 1, 0.3 * XRES / YRES, tex);
    for (int i = 0; i < n_buttons; i++) {
        Quad2D::DrawQuad(buttons[i].position, buttons[i].dim_x * 2 + 0.01, buttons[i].dim_y * 2 + 0.01, ColorRGBA(0.8f, 0.8f, 0.2f, 1.0f));
        if (buttons[i].isOver()) buttons[i].applyColor(ColorRGBA::Grey(0.4)); // TODO
        else buttons[i].applyColor(ColorRGBA::Grey(0.2));
        buttons[i].drawOpenGL();
    }

    /*
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    for (int i = 0; i < n_buttons; i++) {
        glPushAttrib(GL_CURRENT_BIT);
        if (buttons[i].over == true) glColor3f(0.4, 0.4, 0.4);
        else glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_QUADS);
        // TODO: binding texture
        glVertex2f(buttons[i].xmax, buttons[i].ymin);
        glVertex2f(buttons[i].xmax, buttons[i].ymax);
        glVertex2f(buttons[i].xmin, buttons[i].ymax);
        glVertex2f(buttons[i].xmin, buttons[i].ymin);
        glEnd();
        glPopAttrib();
    }

    gluOrtho2D(0, XRES, YRES, 0);
    glPushAttrib(GL_CURRENT_BIT);
    glColor3f(0.0, 0.2, 0.4);

    output(XRES / 2 - glutBitmapLength(font, (const unsigned char*)"Fishing App") / 2, 144, "Fishing App");

    glPopAttrib(); // This sets the colour back to its original value

    glPushAttrib(GL_CURRENT_BIT);
    glColor3f(0.0, 0.0, 1.0);

    for (int i = 0; i < n_buttons; i++)
        output(FromNormalizedToPixel(buttons[i].tx, 1), FromNormalizedToPixel(buttons[i].ty, -1), buttons[i].text);

    glPopAttrib(); // This sets the colour back to its original value
    glPopMatrix();
        */
}

void SetDifficulty() {
    Quad2D::DrawQuad(Vector2(), 2, 2, background);

    back.dim_y = back.dim_x * XRES / YRES;
    back.drawOpenGL();
    for (int i = 0; i < n_difficulty_buttons; i++) {
        Quad2D::DrawQuad(difficulty_buttons[i].position, difficulty_buttons[i].dim_x * 2 + 0.01, difficulty_buttons[i].dim_y * 2 + 0.01, ColorRGBA(0.8f, 0.8f, 0.2f, 1.0f));
        if (i == (int)difficulty || difficulty_buttons[i].isOver() == true) {
            if (difficulty_buttons[i].isOver() == true && i != (int)difficulty)
                difficulty_buttons[i].applyColor(ColorRGBA::Grey(0.4));
            else switch (difficulty) {
            case difficulty_level::easy:
                //difficulty_buttons[i].applyColor(ColorRGBA::Green()); break;
                difficulty_buttons[i].applyColor(ColorRGBA(0.1f, 0.6f, 0.2f, 1.0f)); break;
            case difficulty_level::medium:
                //difficulty_buttons[i].applyColor(ColorRGBA::Yellow()); break;
                difficulty_buttons[i].applyColor(ColorRGBA(0.6f, 0.6f, 0.1f, 1.0f)); break;
            case difficulty_level::hard:
                //difficulty_buttons[i].applyColor(ColorRGBA::Red()); break;
                difficulty_buttons[i].applyColor(ColorRGBA(0.6f, 0.2f, 0.1f, 1.0f)); break;
            }
        } else difficulty_buttons[i].applyColor(ColorRGBA::Grey(0.2));
        difficulty_buttons[i].drawOpenGL();
    }
    /*
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glPushAttrib(GL_CURRENT_BIT);
    if (back.over == true) {
        glColor3f(0.4, 0.4, 0.4);
    } else glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    glVertex2f(back.xmax, back.ymin);
    glVertex2f(back.xmax, back.ymax);
    glVertex2f(back.xmin, back.ymax);
    glVertex2f(back.xmin, back.ymin);
    glEnd();
    if (back.over == true)
        glPopAttrib();
    glPopMatrix();
    for (int i = 0; i < n_difficulty_buttons; i++) {
        glPushAttrib(GL_CURRENT_BIT);
        if (i == (int)difficulty || difficulty_buttons[i].over == true) {
            if (difficulty_buttons[i].over == true && i != (int)difficulty)
                glColor3f(0.4, 0.4, 0.4);
            else if (difficulty == difficulty_level::easy)
                glColor3f(0.0, 1.0, 0.0);
            else if (difficulty == difficulty_level::medium)
                glColor3f(1.0, 1.0, 0.0);
            else if (difficulty == difficulty_level::hard)
                glColor3f(1.0, 0.0, 0.0);
        } else 
            glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_QUADS);
        glVertex2f(difficulty_buttons[i].xmax, difficulty_buttons[i].ymin);
        glVertex2f(difficulty_buttons[i].xmax, difficulty_buttons[i].ymax);
        glVertex2f(difficulty_buttons[i].xmin, difficulty_buttons[i].ymax);
        glVertex2f(difficulty_buttons[i].xmin, difficulty_buttons[i].ymin);
        glEnd();
        //if (i == (int) difficulty || difficulty_buttons[i].over == true)
        glPopAttrib();
    }

    glPopMatrix();


    glPushAttrib(GL_CURRENT_BIT);
    glColor3f(0.0, 1.0, 0.8);

    output(XRES / 2 - glutBitmapLength(font, (const unsigned char*)"Set Difficulty") / 2, 144, "Set Difficulty");

    glPopAttrib();

    glPushAttrib(GL_CURRENT_BIT);
    glColor3f(0.0, 0.0, 0.0);

    output(FromNormalizedToPixel(back.tx, 1), FromNormalizedToPixel(back.ty, -1), back.text);
    for (int i = 0; i < n_buttons; i++)
        output(FromNormalizedToPixel(difficulty_buttons[i].tx, 1), FromNormalizedToPixel(difficulty_buttons[i].ty, -1), difficulty_buttons[i].text);

    glPopAttrib();
    */
}

void ScoreBoard() {
    std::string str;
    char buffer[50];
    tm ts;

    Quad2D::DrawQuad(Vector2(), 2, 2, background);

    back.dim_y = back.dim_x * XRES / YRES;
    back.drawOpenGL();
    Quad2D::DrawQuad(Vector2(0, 0.6), 0.61, 0.11, ColorRGBA::Grey(0.2));
    switch (difficulty) {
    case difficulty_level::easy:
        Quad2D::DrawQuad(Vector2(0, 0.6), 0.6, 0.1, "ScoreBoard: Easy", ColorRGBA::White(), ColorRGBA(0.2f, 0.6f, 0.1f, 1.0f));
        break;
    case difficulty_level::medium:
        Quad2D::DrawQuad(Vector2(0, 0.6), 0.6, 0.1, "ScoreBoard: Medium", ColorRGBA::White(), ColorRGBA(0.6f, 0.6f, 0.2f, 1.0f));
        break;
    case difficulty_level::hard:
        Quad2D::DrawQuad(Vector2(0, 0.6), 0.6, 0.1, "ScoreBoard: Hard", ColorRGBA::White(), ColorRGBA(0.6f, 0.2f, 0.1f, 1.0f));
        break;
    default:break;
    }
    
    for (int i = 0; i < n_score; i++) {
        switch (difficulty) {
        case difficulty_level::easy:
            str = std::to_string(score_easy[i].score);
            ts = FixMonth(score_easy[i].timestamp, -1);
            break;
        case difficulty_level::medium:
            str = std::to_string(score_easy[i].score);
            ts = FixMonth(score_easy[i].timestamp, -1);
            break;
        case difficulty_level::hard:
            str = std::to_string(score_easy[i].score);
            ts = FixMonth(score_easy[i].timestamp, -1);
            break;
        default:break;
        }
        sprintf_s(buffer, "%04d/%02d/%02d %02d:%02d:%02d", ts.tm_year, ts.tm_mon, ts.tm_mday, ts.tm_hour, ts.tm_min, ts.tm_sec);

        Quad2D::DrawQuad(score_buttons[i].position, score_buttons[i].dim_x * 2 + 0.01, score_buttons[i].dim_y * 2 + 0.01, ColorRGBA(0.8f, 0.8f, 0.2f, 1.0f));
        score_buttons[i].applyText(str, ColorRGBA(0.6f, 0.6f, 0.2f, 1.0f));
        score_buttons[i].drawOpenGL();
        str = buffer;
        Quad2D::DrawQuad(score_buttons[i].position - Vector2::I() * 0.5, 0.6, 0.1, str, ColorRGBA(0.6f, 0.6f, 0.2f, 1.0f), ColorRGBA::Grey(0.2));
    }

    /*
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    if (back.over == true) {
        glPushAttrib(GL_CURRENT_BIT);
        glColor3f(0.4, 0.4, 0.4);
    }
    glBegin(GL_QUADS);
    glVertex2f(back.xmax, back.ymin);
    glVertex2f(back.xmax, back.ymax);
    glVertex2f(back.xmin, back.ymax);
    glVertex2f(back.xmin, back.ymin);
    glEnd();
    if (back.over == true)
        glPopAttrib();

    for (int i = 0; i < n_score; i++) {

        glBegin(GL_QUADS);
        glVertex2f(score_buttons[i].xmax, score_buttons[i].ymin);
        glVertex2f(score_buttons[i].xmax, score_buttons[i].ymax);
        glVertex2f(score_buttons[i].xmin, score_buttons[i].ymax);
        glVertex2f(score_buttons[i].xmin, score_buttons[i].ymin);
        glEnd();

    }

    glPopMatrix();


    glPushAttrib(GL_CURRENT_BIT);
    glColor3f(1.0, 1.0, 1.0);

    if (difficulty == difficulty_level::easy)
        output(XRES / 2 - glutBitmapLength(font, (const unsigned char*)"Scoreboard: Easy") / 2, 144, "Scoreboard: Easy");
    else if (difficulty == difficulty_level::medium)
        output(XRES / 2 - glutBitmapLength(font, (const unsigned char*)"Scoreboard: Medium") / 2, 144, "Scoreboard: Medium");
    else if (difficulty == difficulty_level::hard)
        output(XRES / 2 - glutBitmapLength(font, (const unsigned char*)"Scoreboard: Hard") / 2, 144, "Scoreboard: Hard");

    glPopAttrib();

    glPushAttrib(GL_CURRENT_BIT);
    glColor3f(0.0, 0.0, 1.0);

    //output(FromNormalizedToPixel(back.tx, 1), FromNormalizedToPixel(back.ty, -1), back.text);
    for (int i = 0; i < n_score; i++) {
        if (difficulty == difficulty_level::easy) {
            str = std::to_string(score_easy[i].score);
            ts = FixMonth(score_easy[i].timestamp, -1);
            sprintf_s(buffer, "%04d/%02d/%02d %02d:%02d:%02d", ts.tm_year, ts.tm_mon, ts.tm_mday, ts.tm_hour, ts.tm_min, ts.tm_sec);

            output(XRES / 2 - glutBitmapLength(font, (const unsigned char*)str.c_str()) / 2, FromNormalizedToPixel(score_buttons[i].position.y, -1), str);
            str = buffer;
            output(XRES / 10, FromNormalizedToPixel(score_buttons[i].ty, -1), str);
        } else if (difficulty == difficulty_level::medium) {
            str = std::to_string(score_medium[i].score);
            ts = FixMonth(score_medium[i].timestamp, -1);
            sprintf_s(buffer, "%04d/%02d/%02d %02d:%02d:%02d", ts.tm_year, ts.tm_mon, ts.tm_mday, ts.tm_hour, ts.tm_min, ts.tm_sec);

            output(XRES / 2 - glutBitmapLength(font, (const unsigned char*)str.c_str()) / 2, FromNormalizedToPixel(score_buttons[i].ty, -1), str);
            str = buffer;
            output(XRES / 10, FromNormalizedToPixel(score_buttons[i].ty, -1), str);
        } else if (difficulty == difficulty_level::hard) {
            str = std::to_string(score_hard[i].score);
            ts = FixMonth(score_hard[i].timestamp, -1);
            sprintf_s(buffer, "%04d/%02d/%02d %02d:%02d:%02d", ts.tm_year, ts.tm_mon, ts.tm_mday, ts.tm_hour, ts.tm_min, ts.tm_sec);

            output(XRES / 2 - glutBitmapLength(font, (const unsigned char*)str.c_str()) / 2, FromNormalizedToPixel(score_buttons[i].position.y, -1), str);
            str = buffer;
            output(XRES / 10, FromNormalizedToPixel(score_buttons[i].ty, -1), str);
        }
    }
    glPopAttrib();
    */
}

bool init(string path) {
    ColorRGBA nullColor = ColorRGBA(0, 0, 0, 0);
    map<string, Material> allMats;
    map<string, Texture> allTextures;
    vector<Vector3> allVertices, allNormals, temp_vert, temp_norm;
    vector<Vector2> allUVs, temp_uv;
    vector<int> temp_face, temp_line;
    filebuf fb1, fb2;
    string newLine, name, mat, tex_path;
    bool first = true, has_Texture = false;
    float x, y, z;
    ColorRGBA a, d, s, e;
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
            } else first = false;
            name = "" + newLine.substr(7, newLine.length() - 7);
            std::cout << "LOG: new material '" << name << "'.\n";
            a = ColorRGBA::White();
            d = ColorRGBA::Magenta();
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
            s.setAlpha(1.0);
            e.setAlpha(x);
        } else if (stringMatch(newLine, "map_Kd ")) {
            has_Texture = true;
            tex_path = ".\\models\\" + newLine.substr(7, newLine.length() - 7);
            tex_path.replace(tex_path.find("\\\\"), 2, "\\");
            allTextures[name] = Texture(tex_path);
            if (!allTextures[name].isEmpty())
                std::cout << "LOG: new texture '(" << allTextures[name].getId() << ") " << allTextures[name].getPath() << "' found.\n";
        }
    } while (!mtlfile.eof());
    allMats[name] = Material(d, s, a, e);
    std::cout << "LOG: found all materials\n"; 
    
    first = true;
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
            name = "" + newLine.substr(2, newLine.length() - 2);
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
            if (allTextures.find(mat) != allTextures.end() && allGameObj[name].texture.isEmpty()) 
                allGameObj[name].addTexture(allTextures[mat]);
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
            std::cout << "LOG: Curve '" << name << "': point [" << (v1 - start) << "]( z = " << temp_vert[v1 - start].z << ") -> [" << (v2 - start) << "]( z = " << temp_vert[v2 - start].z << ")\n";
        } else if (stringMatch(newLine, "usemtl ")) {
            mat = "" + newLine.substr(7, newLine.length() - 7);
            std::cout << "LOG: '" << mat << "' material found.\n";
        }
    } while (!objfile.eof());
    if (read_status == read_state::read_faces) {
        allGameObj[name].addMesh(Mesh(temp_vert, temp_norm, temp_uv, temp_face));
    } else if (read_status == read_state::read_lines) {
        allGameObj[name].addCurve(Curve(temp_vert, temp_line));
    }
    fb1.close();
    fb2.close();

    return true;
}

void prepareScene() {
    const double pesci_bound[MAX_Pesci] = { 0.4, 0.4, 0.4, 0.25, 0.25, 0.25, 0.4, 0.4, 0.6 };
    const double SCALE = 1.0;

    game_state = GameState::GameState_TitleScreen;
    game_score = 0;
    background = Texture(".\\models\\textures\\render_sfondo.png");

    allGameObj[Splash_OBJ].hide();
    allGameObj[Pugno_OBJ].hide();
    allGameObj[Amo_OBJ].setBoundingBox2D(SCALE * 0.05, SCALE * 0.05);
    for (int i = 0; i < 9; i++) {
        allGameObj[pesci[i]].hide();
        allGameObj[pesci[i]].setBoundingBox2D(SCALE * pesci_bound[i], SCALE * pesci_bound[i]);
        allGameObj[pesci[i]].setState((int)pesce_state::pesce_inactive);
        allTimers[pesci[i]] = Timer();
    }
    //allGameObj[Lago_OBJ].setBoundingBox2D(0.8 * xBound, 0.8 * yBound);
    //allGameObj[Lago_OBJ].boundingBox.setOutside();

    setFloat4(LightAmbient, 0.5f, 0.5f, 0.5f, 1.0f);
    setFloat4(LightDiffuse, 1.0f, 1.0f, 1.0f, 1.0f);
    setFloat4(LightPosition, 0.0f, 0.0f, 15.0f, 1.0f);
    setFloat4(LightSpecular, 0.1f, 0.1f, 0.1f, 0.5f);

    allTimers["spawnFish"] = Timer(5.0);
    allTimers["gameTimer"] = Timer(1.0);
    allTimers["skillCheck"] = Timer(8.0);

    MainCamera.lookAt(Vector3(-5.0, 0.0, 4.0), Vector3(0.0, 0.0, 0.0), Vector3(0.5, 0.0, 0.5));
}

//void resetScene(){}

void prepareOpenGL() {    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Enables blending and alpha value

    glShadeModel(GL_SMOOTH);		 // Enables Smooth Shading
    glClearColor(0.3f, 0.6f, 0.8f, 1.0f);
    glClearDepth(1.0f);				// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);		// Enables Depth Testing
    glDepthFunc(GL_LEQUAL);			// The Type Of Depth Test To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculation
    /*
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);    // Uses default lighting parameters
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_LIGHT0);
    */
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecular);
    glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
    glEnable(GL_LIGHT1);
    
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE); 
}

void repaint(void) {
    /* Show skill check */
    if (check_sequence_generation == false) {
        srand(time(NULL));
        skill_check_sequence = genRandom(sequence_length);
        check_sequence_generation = true;
        cout << "LOG: Sequenza generata: " << skill_check_sequence << endl;
    }

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glPushAttrib(GL_CURRENT_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    // TODO: binding texture
    glVertex2f( 0.5,-0.5);
    glVertex2f( 0.5, 0.5);
    glVertex2f(-0.5, 0.5);
    glVertex2f(-0.5,-0.5);
    glEnd();
    glPopAttrib();
    
    gluOrtho2D(0, XRES, YRES, 0);
    glPushAttrib(GL_CURRENT_BIT);
    glColor3f(1, 0, 0);

    output(0, 50, skill_check_sequence);

    glPopAttrib();
    glPopMatrix();
}

void printScore() {
    //string score_str = "Score: " + std::to_string(game_score);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, XRES, YRES, 0);
    glPushAttrib(GL_CURRENT_BIT);
    glColor3f(1, 0, 0);

    output(0, 24, "Score: " + std::to_string(game_score));
    output(XRES - 100, 24, "Time: " + std::to_string(game_time));

    glPopAttrib(); // This sets the colour back to its original value
    glPopMatrix();
}

void updateScene(double deltaTime) {
    static const double pesci_depth[MAX_Pesci] = { -0.4, -0.4, -0.4, -0.2, -0.2, -0.2, -0.6, -0.6, -0.8 };
    static const double pesci_speed[MAX_Pesci] = { 0.3, 0.3, 0.3, 0.5, 0.5, 0.5, 0.3, 0.3, 0.1 };
    static const int pesci_punti[MAX_Pesci] = { 10, 10, 10, -5, -5, -5, 20, 20, 50 };
    static uint pesci_ord[MAX_Pesci] = { 0, 3, 6, 1, 8, 4, 2, 7, 5 };
    static Vector3 floater = Vector3();
    static uint n_pesci = 0;
    static uint pesci_attivi = 0;
    static double theta = 0.0;
    static bool pesce_interested = false; // indica se un pesce è attaccato alla lenza
    static const double posizioni[6][2] = {
        {-0.3 * xBound, 0.3 * yBound}, {0, 0.3 * yBound}, {0.3 * xBound, 0.3 * yBound},
        {-0.3 * xBound,-0.3 * yBound}, {0,-0.3 * yBound}, {0.3 * xBound,-0.3 * yBound}
    };
    static int n_bonus = 1; 
    static std::string pesce_bonus = "";
    double dist = allGameObj[Canna_OBJ].transform.position.distance(allGameObj[Amo_OBJ].transform.position);

    theta += deltaTime; // angolo da usare per vari calcoli
    floater.set(0, 0, (pesce_interested) ? 0.0 : 0.1 * sin(PI * theta)); // fa galleggiare il pesce
    if (dist > 0.4) {
        // se lontano dalla canna, l'amo si muove velocemente verso di essa
        allGameObj[Amo_OBJ].setVelocity(floater + (allGameObj[Canna_OBJ].transform.position.xy() - allGameObj[Amo_OBJ].transform.position.xy()).asVector3() * 0.4);
    } else if (dist < 0.05) {
        // se vicino rallenta
        allGameObj[Amo_OBJ].setVelocity(floater + (allGameObj[Canna_OBJ].transform.position.xy() - allGameObj[Amo_OBJ].transform.position.xy()).asVector3() * 0.01);
    } else allGameObj[Amo_OBJ].setVelocity(floater); // se si trova sotto la canna da pesca galleggia e basta
    allGameObj[Amo_OBJ].checkCollision(xBound, yBound, 1.0); // mantiene l'amo dentro il laghetto
    for (uint i = 0; i < MAX_Pesci; i++) {
        uint j = pesci_ord[i]; // l'ordine serve a fare in modo che il luccio, che ha il punteggio massimo, abbia meno probabilità di uscire
        pesce_state state = (pesce_state)allGameObj[pesci[j]].getState(); // per non dover fare il cast
        switch (state) {
        case pesce_state::pesce_active:
            // normale stato del pesce, esso si muove in giro a una velocità standard dipendente dalla specie
            if (allGameObj[Amo_OBJ].transform.velocity.xy().magnitude() < 0.1 && allGameObj[pesci[j]].checkCollision(allGameObj[Amo_OBJ])) {
                // se l'amo è quasi fermo e collide con un pesce, esso viene attirato dall'esca
                double facing_angle = 0.0;
                std::cout << "LOG: Collision(1_point): (" << Amo_OBJ << ", " << pesci[j] << ")\n";
                if (game_state == GameState::GameState_Play && !pesce_interested) {
                    playSound(".\\sounds\\poke_interest.wav");
                    facing_angle = rad2deg((allGameObj[Canna_OBJ].transform.position - allGameObj[pesci[j]].transform.position).normalize() * Vector3::Polar(1, allGameObj[Canna_OBJ].transform.rotation.z, 0));
                    allGameObj[pesci[j]].setState((int)pesce_state::pesce_interest); // cambia lo stato
                    allGameObj[pesci[j]].rotate(0, 0, facing_angle); // rivolge la bocca del pesce verso l'amo
                    allGameObj[pesci[j]].setVelocity(Vector3::Polar(1, deg2rad(allGameObj[pesci[j]].transform.rotation.z), 0.0));
                    if (allTimers[pesci[j]].isCounting()) allTimers[pesci[j]].stop();  // si assicura che il timer sia fermo
                    allTimers[pesci[j]].start(3.0); // fa partire un timer per vedere se l'amo sta fermo nella durata
                    pesce_interested = true;
                }
            }
            //if(allGameObj[Amo_OBJ].checkCollision(allGameObj[pesci[i]])) std::cout << "LOG: Collision(2_box): (" << Amo_OBJ << ", " << pesci[i] << ")\n";
            // se il pesce sbatte contro la parete della vasca
            //if(allGameObj[pesci[j]].checkCollision(allGameObj[Lago_OBJ])){
            if (allGameObj[pesci[j]].checkCollision(xBound - allGameObj[pesci[j]].boundingBox.dimensions.x, yBound - allGameObj[pesci[j]].boundingBox.dimensions.y, 4.0)) {
                allGameObj[pesci[j]].rotate(0, 0, 180 - rand() % 60); // ruota in modo da non toccare la parete
                allGameObj[pesci[j]].setVelocity(Vector3::Polar(pesci_speed[j], deg2rad(allGameObj[pesci[j]].transform.rotation.z), 0)); // ruota la velocità nella nuova direzione
            }
            break;
        case pesce_state::pesce_interest:
            if (allTimers[pesci[j]].isCounting()) {
                if (allGameObj[Amo_OBJ].transform.velocity.xy().magnitude() == 0.0) {
                    allGameObj[pesci[j]].setVelocity(allGameObj[pesci[j]].transform.velocity * sin(PI * theta));
                } else {
                    playSound(".\\sounds\\scared.wav");
                    allGameObj[pesci[j]].setState((int)pesce_state::pesce_scared); // il pesce si spaventa e comincia a scappare in giro per la vasca
                    allGameObj[pesci[j]].setVelocity(Vector3::Polar(4.0 * pesci_speed[j], deg2rad(allGameObj[pesci[j]].transform.rotation.z), 0)); // il pesce si muove più velocemente di prima
                    allTimers[pesci[j]].stop(); // viene stoppato il timer e riadibito all'utilizzo per 'pesce_scared'
                    allTimers[pesci[j]].start(5.0); // lo stato 'pesce_scared' ha una durata limitata
                    pesce_interested = false;
                }
            } else {
                if (game_state != GameState::GameState_SkillCheck && !allTimers["skillCheck"].isCounting()) {
                    allTimers["skillCheck"].start(); // avvia il timer dello skill check
                    playSound(".\\sounds\\poke_interest.wav");
                    game_state = GameState::GameState_SkillCheck; // cambia lo stato del gioco e avvia lo skill check
                    // Resetta le variabili relative allo skill check
                    std::cout << "LOG: Start skillcheck.\n";
                    skill_check_successful = false;
                    skill_check_failed = false;
                    check_sequence_generation = false;
                } else if (game_state == GameState::GameState_SkillCheck && skill_check_successful) { // se lo skillcheck viene superato
                    allTimers["skillCheck"].stop(); // ferma il timer dello skill check
                    if (pesci_speed[j] == 0.5) playSound(".\\sounds\\fish_catch_small.wav");
                    if (pesci_speed[j] == 0.3) playSound(".\\sounds\\fish_catch_medium.wav");
                    else playSound(".\\sounds\\fish_catch_big.wav");
                    // Resetta le variabili relative allo skill check
                    skill_check_successful = false;
                    check_sequence_generation = false;
                    allGameObj[pesci[j]].setState((int)pesce_state::pesce_hooked); // lo stato passa a 'pesce_hooked'
                    allGameObj[pesci[j]].rotate(0, -90, 0); // il pesce viene messo in verticale
                    allGameObj[Amo_OBJ].setAcceleration(Vector3::K() * 4.0); // l'amo viene alzato come se venisse riavvolta la lenza
                    allGameObj[Splash_OBJ].place(allGameObj[Amo_OBJ].transform.position.xy().asVector3());
                    allGameObj[Splash_OBJ].scale(0.3, 0.3, 1.0);
                    allGameObj[Splash_OBJ].show();
                    allTimers[pesci[j]].stop(); // viene stoppato il timer e riadibito all'utilizzo per 'pesce_hooked'
                    allTimers[pesci[j]].start(1.5);
                    allGameObj[pesci[j]].place(allGameObj[Amo_OBJ].transform.position);
                    game_state = GameState::GameState_Play;
                } else if (game_state == GameState::GameState_SkillCheck && (skill_check_failed || !allTimers["skillCheck"].isCounting())) {
                    allTimers["skillCheck"].stop();
                    playSound(".\\sounds\\scared.wav");
                    skill_check_failed = false;
                    check_sequence_generation = false;
                    allGameObj[pesci[j]].setState((int)pesce_state::pesce_scared); // il pesce si spaventa e comincia a scappare in giro per la vasca
                    allGameObj[pesci[j]].setVelocity(Vector3::Polar(4.0 * pesci_speed[j], deg2rad(allGameObj[pesci[j]].transform.rotation.z), 0)); // il pesce si muove più velocemente di prima
                    allTimers[pesci[j]].stop(); // viene stoppato il timer e riadibito all'utilizzo per 'pesce_scared'
                    allTimers[pesci[j]].start(5.0); // lo stato 'pesce_scared' ha una durata limitata
                    game_state = GameState::GameState_Play;
                    pesce_interested = false;
                }
            }
            break;
        case pesce_state::pesce_hooked:
            if (!allTimers[pesci[j]].isCounting() && allGameObj[Amo_OBJ].transform.acceleration.magnitude() > 0.0) {
                playSound(".\\sounds\\poke_catch.wav");
                allGameObj[Amo_OBJ].setAcceleration(Vector3::Origin()); // l'amo si ferma
                allGameObj[Amo_OBJ].setVelocity(Vector3::Origin()); // l'amo si ferma
                allGameObj[Splash_OBJ].hide();
                allGameObj[Splash_OBJ].resetTransform();
            } else allGameObj[Splash_OBJ].scale(1.1, 1.1, 0);
            // se l'amo si avvicina al secchio, che si trova al margine anteriore sinistro
            if (allGameObj[Amo_OBJ].transform.position.xy().distance(Vector2(-0.9 * xBound, 0.9 * yBound)) <= 0.3) {
                if (pesci_speed[j] == 0.5) playSound(".\\sounds\\clap_small.wav");
                else playSound(".\\sounds\\clap_big.wav");
                allGameObj[pesci[j]].setState((int)pesce_state::pesce_inactive); // il pesce viene settato inattivo
                allGameObj[pesci[j]].hide(); // la mesh non viene renderizzata
                allGameObj[pesci[j]].rotate(0, 90, 0); // la rotazione viene resettata
                allGameObj[Amo_OBJ].translate(0.0, 0.0, -allGameObj[Amo_OBJ].transform.position.z); // l'amo viene riposizionato a pelo d'acqua
                allGameObj[Amo_OBJ].setAcceleration(Vector3::Origin()); // l'amo si ferma
                allGameObj[Amo_OBJ].setVelocity(Vector3::Origin());
                // TODO: aggiorna punteggio
                pesci_attivi--;
                pesce_interested = false;
                game_score += pesci_punti[j] * ((stringMatch(pesci[j], pesce_bonus.c_str())) ? 2 : 1);
                std::cout << "LOG: Ha abboccato " << pesci[j] << ", punti guadagnati: (" << ((pesci_punti[j] > 0) ? "+" : "") << pesci_punti[j] << "), punteggio: " << game_score << "\n";
            } else allGameObj[pesci[j]].place(allGameObj[Amo_OBJ].transform.position); // il pesce si muove con l'amo
            break;
        case pesce_state::pesce_scared:
            // finché il timer continua il pesce è spaventato
            if (allTimers[pesci[i]].isCounting() && allGameObj[pesci[j]].checkCollision(xBound - allGameObj[pesci[j]].boundingBox.dimensions.x, yBound - allGameObj[pesci[j]].boundingBox.dimensions.y, 1.0 - pesci_depth[j])) {
                allGameObj[pesci[j]].rotate(0, 0, 180 - rand() % 90);
                allGameObj[pesci[j]].setVelocity(Vector3::Polar(4.0 * pesci_speed[j], deg2rad(allGameObj[pesci[j]].transform.rotation.z), 0));
            } else allGameObj[pesci[j]].setState((int)pesce_state::pesce_active); // appena scade il timer il pesce si calma
            break;
        case pesce_state::pesce_inactive: default: break; // se il pesce è inattivo, non fa nulla
        }
    }
    if (!allTimers["spawnFish"].isCounting()) {
        // se i pesci nel laghetto sono meno di 6
        if (pesci_attivi < 6) {
            // l'ordine permette di controllare il punteggio dato dai vari pesci
            int i = pesci_ord[n_pesci % MAX_Pesci];
            int p = rand() % 6; // la posizione viene scelta in maniera randomica

            allGameObj[pesci[i]].place(posizioni[p][0], posizioni[p][1], pesci_depth[i]);
            allGameObj[pesci[i]].rotate(0, 0, rand() % 180); // per aggiungere randomicità il pesce ha una rotazione iniziale casuale
            allGameObj[pesci[i]].setVelocity(Vector3::Polar(pesci_speed[i], deg2rad(allGameObj[pesci[i]].transform.rotation.z), 0)); // la velocità viene aggiustata di conseguenza
            allGameObj[pesci[i]].setState((int)pesce_state::pesce_active); // il pesce viene attivato
            allGameObj[pesci[i]].show(); // e compare sullo schermo
            n_pesci++;
            pesci_attivi++;
            std::cout << "LOG: Spawned " << allGameObj[pesci[i]].getName() << " in (" << posizioni[p][0] << ", " << posizioni[p][1] << ") [" << n_pesci << "].\n";
        }
        if (n_bonus % 2 == 0) {
            // imposta il pesce bonus
            int i = 0, b = pesci_ord[n_pesci % MAX_Pesci - pesci_attivi + rand() % pesci_attivi];

            for (; i < pesci[b].length() && isalpha(pesci[b].at(i)); i++);
            pesce_bonus = pesci[b].substr(0, i);
            std::cout << "Pesce Bonus: " + pesce_bonus + ".\n";
        }
        n_bonus++;
        allTimers["spawnFish"].start(); // il timer riparte
    }
}

void doMotion() {
    static Vector3 left = Vector3(0, -1, 0), a = Vector3(), b = Vector3();
    static int prev_time = 0;
    const float middle = 0.3;
    double deltaTime;
    int time = glutGet(GLUT_ELAPSED_TIME);

    deltaTime = ((double)time - (double)prev_time) * 0.001; // in seconds
    prev_time = time;
    MainCamera.lookAt(MainCamera.position, allGameObj[Canna_OBJ].transform.position * middle, MainCamera.forward % left);
    if (game_state == GameState::GameState_Play || game_state == GameState::GameState_SkillCheck) {
        updateScene(deltaTime);
        allGameObj[Amo_OBJ].move(deltaTime);
        for (int i = 0; i < MAX_Pesci; i++) {
            pesce_state ps = (pesce_state)allGameObj[pesci[i]].getState();
            switch (ps) {
            case pesce_state::pesce_active:
            case pesce_state::pesce_interest:
            case pesce_state::pesce_scared:
            case pesce_state::pesce_hooked:
                allTimers[pesci[i]].pass(deltaTime);
                allGameObj[pesci[i]].move(deltaTime);
                break;
            case pesce_state::pesce_inactive:
            default:
                break;
            }
        }
        allTimers["spawnFish"].pass(deltaTime);
        allTimers["gameTimer"].pass(deltaTime);
        allTimers["skillCheck"].pass(deltaTime);
    }

    glutPostRedisplay();
}

void renderScene() {
    static bool once = true;
    static vector<string> orderedObjs;
    static Vector2 dim = Vector2();
    int id = -1;

    if (once) {
        // ordina gli elementi in base al numero contenuto nel nome
        once = false; // l'ordinamento va fatto solo una volta,
        orderedObjs = vector<string>(allGameObj.size());
        for (auto elem : allGameObj) {
            bool underscore = false;
            id = -1;
            for (uint i = 0; i < elem.first.length(); i++) {
                char c = elem.first.at(i);
                if (underscore && c >= '0' && c <= '9') {
                    // una sola cifra => id < 10
                    if (id == -1) id = c - '0'; 
                    else {
                        // altrimenti id > 10, quindi la prima viene considerata per le decine, la seconda cifra per le unità
                        id *= 10;
                        id += c - '0';
                    }
                } else if (!underscore && c == '_') underscore = true;
            }
            orderedObjs[id] = "";
            // l'ordinamento viene eseguito solo sulle stringhe dei nomi, così da non duplicare i dati
            orderedObjs[id] += elem.first;
            std::cout << "log: rendering (" << id << ")->'" << elem.first << "'.\n"; // stampa per testing, penso di lasciarla
        } 
    }

    switch(game_state) {
    case GameState::GameState_TitleScreen:
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        switch (CurrentState) {
        case menu_state::menu:
            Menu();
            break;
        case menu_state::setDifficulty:
            SetDifficulty();
            break;
        case menu_state::play:
            game_state = GameState::GameState_Play;
            allTimers["gameTimer"].start(stopGameTimer);
            changeSize(XRES, YRES);
            break; // avvia il gioco
        case menu_state::scoreboard:
            ScoreBoard();
            break;
        default: break;
        }
        break;
    case GameState::GameState_SkillCheck:
    case GameState::GameState_Play:
    case GameState::GameState_End:
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        gluLookAt(MainCamera.position.x, MainCamera.position.y, MainCamera.position.z,
                    MainCamera.focus.x, MainCamera.focus.y, MainCamera.focus.z,
                    MainCamera.up.x, MainCamera.up.y, MainCamera.up.z);
        for (uint i = 0; i < allGameObj.size(); i++) {
            // gli oggetti vengono renderizzati nell'ordine stabilito dentro 'orderedObjs'
            allGameObj[orderedObjs[i]].renderOpenGL(); 
        }
        glPopMatrix();
        printScore();
        if (game_state == GameState::GameState_SkillCheck) {
            repaint();
        }
        break;
    default: break;
    }
    
    glutSwapBuffers();
}

void UpdateScoreboard(Scoreboard* records) {
    const time_t now = time(0);
    tm ltm;
    localtime_s(&ltm, &now);

    tm ts;
    ts.tm_year = 1900 + ltm.tm_year;
    ts.tm_mon = 1 + ltm.tm_mon;
    ts.tm_mday = ltm.tm_mday;
    ts.tm_hour = ltm.tm_hour;
    ts.tm_min = ltm.tm_min;
    ts.tm_sec = ltm.tm_sec;

    for (int i = 0; i < n_score; i++) {
        if (game_score >= records[i].score) {
            for (int j = n_score - 1; j >= i; j--)
                records[j] = records[j - 1];

            records[i].score = game_score;
            records[i].timestamp = ts;
            break;
        }

    }
}

void processNormalKeys(unsigned char key, int x, int y) {
    static unsigned int n = 0;
    static unsigned int screen_select = 0;

    switch (game_state) {
    case GameState::GameState_End:
        switch (key) {
        case ' ':
            game_state = GameState::GameState_TitleScreen;
            CurrentState = menu_state::menu;
            if (left_button) {
                glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
                left_button = false;
            }
            if (difficulty == difficulty_level::easy)
                UpdateScoreboard(score_easy);
            else if (difficulty == difficulty_level::medium)
                UpdateScoreboard(score_medium);
            else
                UpdateScoreboard(score_hard);
            game_score = 0;
            break;
        }
        break;
    case GameState::GameState_Play:
        break;
    case GameState::GameState_SkillCheck:
        switch (key) {
            // W A S D
        case 'w':
            if (skill_check_sequence[n] == 'w')
                n++;
            else
                n = 0;
            break;
        case 'a':
            if (skill_check_sequence[n] == 'a')
                n++;
            else
                n = 0;
            break;
        case 's':
            if (skill_check_sequence[n] == 's')
                n++;
            else
                n = 0;
            break;
        case 'd':
            if (skill_check_sequence[n] == 'd')
                n++;
            else
                n = 0;
            break;
        case ' ':
            cout << "Skill check interrupted!\n";
            n = 0;
            break;
        default:
            n = 0;
            break;
        }

        /* Skill check OK */
        if (n == sequence_length) {
            // Increase score
            // Rimuovi il pesce
            // Disabilita skill check
            check_sequence_generation = false;
            skill_check_successful = true;
            std::cout << "Skill check successful!" << endl;
            n = 0;
        } else if (n == 0) {
            // Skill check failed
            check_sequence_generation = false;
            skill_check_failed = true;
            std::cout << "Skill check failed!" << endl;
        }
    }

    // std::cout << "( " << n << " ) " << key << "\n";
}

void mouse(int button, int state, int x, int y) {
    switch (game_state) {
    case GameState::GameState_TitleScreen:
        myMouse(button, state, x, y);
        break;
    case GameState::GameState_Play:
        switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN) {
                playSound(".\\sounds\\grab_start.wav");
                left_button = true;
                glutSetCursor(GLUT_CURSOR_NONE);
                allGameObj[Mano_OBJ].hide();
                allGameObj[Pugno_OBJ].show();
            } else if (state == GLUT_UP) {
                playSound(".\\sounds\\grab_end.wav");
                left_button = false;
                allGameObj[Pugno_OBJ].hide();
                allGameObj[Mano_OBJ].show();
                glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
            }
            break;
        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_DOWN) {
                allGameObj[Lago_OBJ].hide();
            } else if (state == GLUT_UP) {
                allGameObj[Lago_OBJ].show();
            }
            break;
        case GLUT_MIDDLE_BUTTON:
            //if (state == GLUT_DOWN)
            //else if (state == GLUT_UP)
            break;
        default: break;
        }
    }
}

void drag_n_drop(int x, int y) {
    static Vector3 delta = Vector3(), on_screen = Vector3(), canna_xy = Vector3();
    bool drag = false;
    int xres = glutGet(GLUT_WINDOW_WIDTH);
    int yres = glutGet(GLUT_WINDOW_HEIGHT);

    if (game_state == GameState::GameState_Play) {
        if (x > xres) x = xres - 1;
        if (y > yres) y = yres - 1;
        if (x < 0) x = 1;
        if (y < 0) y = 1;
        // mappatura fra la posizione del puntatore del mouse sullo schermo e le coordinate sul piano xy
        on_screen.set((0.5 - (float)y / (float)yres) * 2 * (xBound - 0.1), (0.5 - (float)x / (float)xres) * 2 * (yBound - 0.1), 0.0);
        if (left_button) {
            // viene rilevata la posizione corrente in modo da poter utilizzare 'translate()' invece di 'place()' (performance migliori)
            canna_xy.set(allGameObj[Canna_OBJ].transform.position.x, allGameObj[Canna_OBJ].transform.position.y, 0);
            delta = on_screen - canna_xy;
            // canna, rocchetto e mano si muovono in maniera solidale
            allGameObj[Canna_OBJ].translate(delta);
            allGameObj[Rocchetto_OBJ].translate(delta);
            allGameObj[Pugno_OBJ].translate(delta);

            //std::cout << "Amo.vel(" << allGameObj[Amo_OBJ].transform.velocity.x << ", " << allGameObj[Amo_OBJ].transform.velocity.y << ")\n";
        }
    }
}

void changeSize(int w, int h) {
    const double aspectRatio = (float)w / h, fieldOfView = 45.0;

    XRES = w;
    YRES = h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    switch (game_state) {
    case GameState::GameState_TitleScreen:
        gluOrtho2D(0, w, h, 0);
        break;
    default:
        gluPerspective(fieldOfView, aspectRatio, 1.0, 15.0);  // Znear and Zfar 
        break;
    }
    glViewport(0, 0, w, h);

    glutPostRedisplay();
}

int main(int argc, char* argv[]) {
    // init GLUT and create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(XRES, YRES);
    glutCreateWindow("Fishing App");

    //soundEngine = irrklang::createIrrKlangDevice();
    //if (!soundEngine) std::cout << "ERROR: Could not startup sound engine.\n";
    
    // OpenGL init
    if (!init(".\\models\\laghetto.obj")) {
        std::cout << "Initialization failed\n";
        return -1;
    }
    prepareScene();
    prepareOpenGL();
    InitMenu();
    ReadScoreboard();
    // register callbacks
    glutDisplayFunc(renderScene);
    glutIdleFunc(doMotion);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(processNormalKeys);
    glutMouseFunc(mouse);
    glutMotionFunc(drag_n_drop);
    glutPassiveMotionFunc(PickButton);
    // enter GLUT event processing cycle
    glutGet(GLUT_ELAPSED_TIME);
    glutMainLoop();
    //soundEngine->drop();

    return 0;
}

