#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif


double sqrt_custom(double S) {
    if (S < 0) return -1;
    if (S == 0) return 0;
    if (S != S || S > DBL_MAX) return S; // NaN e infinito: Newton nunca convergiria

    // Começando acima da raiz, Newton decresce até parar; um eps absoluto
    // nunca é atingido para S grande (o ulp passa de 1e-6).
    double x = S > 1 ? S : 1;
    while (1) {
        double nx = 0.5 * (x + S / x);
        if (nx >= x)
            return x;
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

        printf("\n----------------------------------------------\n");
        if (!(ys > 0) || !(rho > 0)) {
            printf("Erro: yield strength e densidade devem ser maiores que zero.\n");
        } else {
            double R = potatoRadiusFormula(ys, rho);
            if (R > DBL_MAX)
                printf("Erro: densidade pequena demais, o raio estoura o double.\n");
            else
                printf("Potato Radius (m): %.6f\n", R);
        }
        printf("----------------------------------------------\n\n");

        printf("Calcular novamente? (s/n): ");
        getchar();

        char c = getchar();
        if (c == 'n' || c == 'N')
            break;
    }

    return 0;
}

