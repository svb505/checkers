#pragma once
#include <map>
#include <GLFW/glfw3.h>
#include "world.h"
#include <cmath>

#define step 20

struct Cell {
    int row;
    int col;
};

class Checkers {
private:
    std::map<int, Cell> positionsBlue;
    std::map<int, Cell> positionsRed;

    int selectedBlue = 0;
    int selectedRed = 0;

    bool redStart = false;
    int scoreRed = 0;
    int scoreBlue = 0;

    // ===== utils =====
    void cellToWorld(const Cell& c, float& x, float& y) {
        x = minX + c.col * step + step / 2.0f;
        y = maxY - c.row * step - step / 2.0f;
    }

    bool hasChecker(int row, int col, std::map<int, Cell>& list) {
        for (auto& p : list) {
            if (p.second.row == row && p.second.col == col)
                return true;
        }
        return false;
    }

    int getCheckerAt(int row, int col, std::map<int, Cell>& list) {
        for (auto& p : list) {
            if (p.second.row == row && p.second.col == col)
                return p.first;
        }
        return -1;
    }

public:
    Checkers() {
        int index = 0;

        // blue (top)
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 8; col++) {
                if ((row + col) % 2 == 1) {
                    positionsBlue[index++] = { row, col };
                }
            }
        }

        index = 0;

        // red (bottom)
        for (int row = 5; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                if ((row + col) % 2 == 1) {
                    positionsRed[index++] = { row, col };
                }
            }
        }
    }

    // ===== drawing =====
    void drawSheet() {
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {

                if ((row + col) % 2 == 0) glColor3f(1, 1, 1);
                else glColor3f(0.2f, 0.2f, 0.2f);

                float x = minX + col * step;
                float y = maxY - row * step;

                glBegin(GL_QUADS);
                glVertex2f(x, y);
                glVertex2f(x + step, y);
                glVertex2f(x + step, y - step);
                glVertex2f(x, y - step);
                glEnd();
            }
        }
    }

    void drawChecker() {
        const int segments = 40;
        const float radius = 7.0f;

        for (auto& blue : positionsBlue) {
            float cx, cy;
            cellToWorld(blue.second, cx, cy);

            if (blue.first == selectedBlue)
                glColor3f(0.0f, 0.0f, 0.6f);
            else
                glColor3f(0.0f, 0.0f, 1.0f);

            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(cx, cy);
            for (int i = 0; i <= segments; i++) {
                float a = 2.0f * 3.1415926f * i / segments;
                glVertex2f(cx + cos(a) * radius, cy + sin(a) * radius);
            }
            glEnd();
        }

        for (auto& red : positionsRed) {
            float cx, cy;
            cellToWorld(red.second, cx, cy);

            if (red.first == selectedRed)
                glColor3f(0.6f, 0.0f, 0.0f);
            else
                glColor3f(1.0f, 0.0f, 0.0f);

            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(cx, cy);
            for (int i = 0; i <= segments; i++) {
                float a = 2.0f * 3.1415926f * i / segments;
                glVertex2f(cx + cos(a) * radius, cy + sin(a) * radius);
            }
            glEnd();
        }
    }

    // ===== input & logic =====
    void processInput(GLFWwindow* window) {
        static bool aWas = false, dWas = false, lWas = false, rWas = false;

        bool a = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
        bool d = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
        bool l = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
        bool r = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;

        if (a && !aWas) {
            if (!redStart && selectedBlue > 0) selectedBlue--;
            if (redStart && selectedRed > 0) selectedRed--;
        }

        if (d && !dWas) {
            if (!redStart && selectedBlue < 11) selectedBlue++;
            if (redStart && selectedRed < 11) selectedRed++;
        }

        // ===== MOVE LEFT =====
        if (l && !lWas) {
            auto& positions = redStart ? positionsRed : positionsBlue;
            auto& enemy = redStart ? positionsBlue : positionsRed;
            int& selected = redStart ? selectedRed : selectedBlue;

            int idx = selected;
            Cell& c = positions[idx];

            int dir = redStart ? -1 : 1;
            int newRow = c.row + dir;
            int newCol = c.col - 1;

            int enemyIdx = getCheckerAt(newRow, newCol, enemy);

            if (enemyIdx != -1) {
                int jumpRow = newRow + dir;
                int jumpCol = newCol - 1;

                if (jumpRow >= 0 && jumpRow < 8 &&
                    jumpCol >= 0 && jumpCol < 8 &&
                    !hasChecker(jumpRow, jumpCol, positions) &&
                    !hasChecker(jumpRow, jumpCol, enemy)) {

                    enemy.erase(enemyIdx);
                    c = { jumpRow, jumpCol };
                    redStart = !redStart;
                }
            }
            else if (newRow >= 0 && newRow < 8 &&
                newCol >= 0 && newCol < 8 &&
                !hasChecker(newRow, newCol, positions)) {

                c = { newRow, newCol };
                redStart = !redStart;
            }
        }

        // ===== MOVE RIGHT =====
        if (r && !rWas) {
            auto& positions = redStart ? positionsRed : positionsBlue;
            auto& enemy = redStart ? positionsBlue : positionsRed;
            int& selected = redStart ? selectedRed : selectedBlue;

            int idx = selected;
            Cell& c = positions[idx];

            int dir = redStart ? -1 : 1;
            int newRow = c.row + dir;
            int newCol = c.col + 1;

            int enemyIdx = getCheckerAt(newRow, newCol, enemy);

            if (enemyIdx != -1) {
                int jumpRow = newRow + dir;
                int jumpCol = newCol + 1;

                if (jumpRow >= 0 && jumpRow < 8 &&
                    jumpCol >= 0 && jumpCol < 8 &&
                    !hasChecker(jumpRow, jumpCol, positions) &&
                    !hasChecker(jumpRow, jumpCol, enemy)) {

                    enemy.erase(enemyIdx);
                    c = { jumpRow, jumpCol };
                    redStart = !redStart;
                }
            }
            else if (newRow >= 0 && newRow < 8 &&
                newCol >= 0 && newCol < 8 &&
                !hasChecker(newRow, newCol, positions)) {

                c = { newRow, newCol };
                redStart = !redStart;
            }
        }

        aWas = a;
        dWas = d;
        lWas = l;
        rWas = r;
    }
};
