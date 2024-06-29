#include <stdio.h>
#include <math.h>
#include <unistd.h>

#define SCREEN_WIDTH 40
#define SCREEN_HEIGHT 40
#define THETA_SPACING 0.07
#define PHI_SPACING 0.02
#define R1 1
#define R2 2
#define K2 5
#define PI 3.141592
#define K1 SCREEN_WIDTH * K2 * 3 / (8 * (R1 + R2))

void render_frame(float A, float B) {
    float cosA = cos(A), sinA = sin(A);
    float cosB = cos(B), sinB = sin(B);
    char lumi[12] = ".,-~:;=!*#$@";

    char output[SCREEN_WIDTH][SCREEN_HEIGHT];
    float zbuffer[SCREEN_WIDTH][SCREEN_HEIGHT];

    for (int i = 0; i < SCREEN_WIDTH; i++) {
        for (int j = 0; j < SCREEN_HEIGHT; j++) {
            output[i][j] = ' ';
            zbuffer[i][j] = 0;
        }
    }

    for (float theta = 0; theta < 2 * PI; theta += THETA_SPACING) {
        float costheta = cos(theta), sintheta = sin(theta);

        for (float phi = 0; phi < 2 * PI; phi += PHI_SPACING) {
            float cosphi = cos(phi), sinphi = sin(phi);

            float circlex = R2 + R1 * costheta;
            float circley = R1 * sintheta;

            // all transformations are precalculated
            float x = circlex * (cosB * cosphi + sinA * sinB * sinphi) - circley * cosA * sinB;
            float y = circlex * (sinB * cosphi - sinA * cosB * sinphi) + circley * cosA * cosB;
            float z = K2 + cosA * circlex * sinphi + circley * sinA;

            float ooz = 1 / z; // one over z

            int xp = (int)(SCREEN_WIDTH / 2 + (K1 * ooz * x));
            int yp = (int)(SCREEN_HEIGHT / 2 - (K1 * ooz * y));

            float L = cosphi * costheta * sinB - cosA * costheta * sinphi - sinA * sintheta + cosB * (cosA * sintheta - costheta * sinA * sinphi);

            if (L > 0) {
                if (xp >= 0 && xp < SCREEN_WIDTH && yp >= 0 && yp < SCREEN_HEIGHT) {
                    if (ooz > zbuffer[xp][yp]) {
                        zbuffer[xp][yp] = ooz;
                        int luminance_index = (int)(L * 8);
                        if (luminance_index < 0) luminance_index = 0;
                        if (luminance_index > 11) luminance_index = 11;
                        output[xp][yp] = lumi[luminance_index];
                    }
                }
            }
        }
    }

    // Clear the screen
    printf("\033[H");

    for (int j = 0; j < SCREEN_HEIGHT; j++) {
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            putchar(output[i][j]);
        }
        putchar('\n');
    }
}

int main() {
    float A = 0;
    float B = 0;

    while (1) {
        render_frame(A, B);
        A += 0.02;
        B += 0.01;
        usleep(30000); 
    }

    return 0;
}

