#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif


double sqrt_custom(double S) {
    if (S < 0) return -1;
    double x = S;
    double eps = 1e-6;

    while (1) {
        double nx = 0.5 * (x + S / x);
        double diff = x - nx;
        if (diff < eps && diff > -eps)
            return nx;
        x = nx;
    }
}


double potatoRadiusFormula(double yieldStrength, double density){
    double G = 6.674e-11;
    double pi = 3.141592653589793;

    double N = 2.0 * yieldStrength;
    double D = pi * G * density * density;

    return sqrt_custom(N / D);
}


void clear_screen() {
    system(CLEAR);
}

void header() {
    printf("\033[1;36m");
    printf("==============================================\n");
    printf("               POTATO RADIUS TOOL             \n");
    printf("==============================================\n");
    printf("\033[0m");
}

int main() {
    double ys, rho;

    while (1) {
        clear_screen();
        header();

        printf("Insira o yield strength (Pa): ");
        if (scanf("%lf", &ys) != 1) break;

        printf("Insira a densidade (kg/m^3): ");
        if (scanf("%lf", &rho) != 1) break;

        double R = potatoRadiusFormula(ys, rho);

        printf("\n----------------------------------------------\n");
        printf("Potato Radius (m): %.6f\n", R);
        printf("----------------------------------------------\n\n");

        printf("Calcular novamente? (s/n): ");
        getchar();

        char c = getchar();
        if (c == 'n' || c == 'N')
            break;
    }

    return 0;
}

