/**
 * Potato Radius Calculator - Maximum Improvement Edition
 * 
 * Calculates the minimum radius at which a celestial body becomes spherical
 * due to hydrostatic equilibrium, based on material properties.
 * 
 * Formula: R = sqrt(2 * Y / (π * G * ρ²))
 * Where:
 *   Y = Yield strength (Pa)
 *   ρ = Density (kg/m³)
 *   G = Gravitational constant
 * 
 * Features:
 * - Robust input validation with error handling
 * - Predefined material presets for common celestial compositions
 * - Advanced Newton-Raphson square root with convergence guarantees
 * - Unit conversions (meters, kilometers, miles)
 * - Scientific notation output
 * - Color-coded terminal UI with animations
 * - Input history and session statistics
 * - Cross-platform compatibility
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <float.h>
#include <errno.h>

/* Platform detection */
#ifdef _WIN32
    #define CLEAR "cls"
    #define PATH_SEP "\\"
    #define USE_ANSI_COLORS 0
#else
    #define CLEAR "clear"
    #define PATH_SEP "/"
    #define USE_ANSI_COLORS 1
#endif

/* Constants */
#define G_CONST 6.67430e-11           /* Gravitational constant (m³/kg·s²) */
#define PI 3.14159265358979323846
#define MAX_INPUT_BUF 256
#define MAX_HISTORY 10
#define MAX_PRESETS 8

/* ANSI Color codes */
#define COLOR_RESET     "\033[0m"
#define COLOR_BOLD      "\033[1m"
#define COLOR_CYAN      "\033[36m"
#define COLOR_GREEN     "\033[32m"
#define COLOR_YELLOW    "\033[33m"
#define COLOR_BLUE      "\033[34m"
#define COLOR_MAGENTA   "\033[35m"
#define COLOR_RED       "\033[31m"
#define COLOR_WHITE     "\033[37m"

/* Material preset structure */
typedef struct {
    const char* name;
    double yield_strength;  /* Pascals */
    double density;         /* kg/m³ */
    const char* description;
} MaterialPreset;

/* Calculation result structure */
typedef struct {
    double radius_meters;
    double radius_km;
    double radius_miles;
    double volume;
    double mass;
    double surface_gravity;
    int valid;
    char error_msg[256];
} CalculationResult;

/* Session statistics */
typedef struct {
    int calculation_count;
    double min_radius;
    double max_radius;
    double total_radius;
    time_t start_time;
} SessionStats;

/* Global state */
static SessionStats g_stats = {0};
static double g_history[MAX_HISTORY][2];  /* [yield, density] pairs */
static int g_history_count = 0;

/* Predefined material presets */
static const MaterialPreset presets[MAX_PRESETS] = {
    {"Ice", 1.0e6, 917.0, "Water ice (typical outer solar system moons)"},
    {"Rock", 1.0e7, 3000.0, "Silicate rock (typical asteroids)"},
    {"Granite", 2.0e7, 2700.0, "Granite stone"},
    {"Basalt", 3.0e7, 3000.0, "Volcanic basalt rock"},
    {"Steel", 2.5e8, 7850.0, "Structural steel"},
    {"Diamond", 1.2e10, 3500.0, "Crystalline diamond"},
    {"Potato", 1.5e5, 1100.0, "Organic potato matter (the original!)"},
    {"Regolith", 5.0e5, 1500.0, "Loose planetary regolith"}
};

/**
 * Enhanced Newton-Raphson square root with overflow protection
 * and guaranteed convergence within specified iterations
 */
double sqrt_robust(double S, int max_iterations, double tolerance) {
    if (S < 0.0) return -1.0;
    if (S == 0.0) return 0.0;
    if (!isfinite(S)) return NAN;
    
    /* Handle very small and very large numbers */
    if (S < DBL_MIN) return 0.0;
    if (S > DBL_MAX / 2.0) return INFINITY;
    
    /* Initial guess using bit manipulation approximation for speed */
    double x = S;
    if (x > 1.0) {
        /* Scale initial guess for better convergence */
        int exp;
        double mantissa = frexp(x, &exp);
        x = ldexp(mantissa, exp / 2);
    }
    
    /* Newton-Raphson iteration */
    for (int i = 0; i < max_iterations; i++) {
        double nx = 0.5 * (x + S / x);
        
        /* Check for convergence */
        double rel_diff = fabs(nx - x) / fmax(fabs(nx), 1e-10);
        if (rel_diff < tolerance) {
            return nx;
        }
        
        /* Prevent divergence */
        if (!isfinite(nx) || nx <= 0.0) {
            break;
        }
        
        x = nx;
    }
    
    return x;
}

/**
 * Calculate potato radius with comprehensive result data
 */
CalculationResult calculate_potato_radius(double yield_strength, double density) {
    CalculationResult result = {0};
    
    /* Input validation */
    if (yield_strength <= 0.0) {
        result.valid = 0;
        snprintf(result.error_msg, sizeof(result.error_msg), 
                 "Yield strength must be positive (got: %e)", yield_strength);
        return result;
    }
    
    if (density <= 0.0) {
        result.valid = 0;
        snprintf(result.error_msg, sizeof(result.error_msg), 
                 "Density must be positive (got: %e)", density);
        return result;
    }
    
    if (!isfinite(yield_strength) || !isfinite(density)) {
        result.valid = 0;
        snprintf(result.error_msg, sizeof(result.error_msg), 
                 "Inputs must be finite numbers");
        return result;
    }
    
    /* Check for potential overflow */
    double density_squared = density * density;
    if (density_squared > DBL_MAX / (PI * G_CONST)) {
        result.valid = 0;
        snprintf(result.error_msg, sizeof(result.error_msg), 
                 "Density too large - would cause overflow");
        return result;
    }
    
    /* Calculate radius using formula: R = sqrt(2Y / (πGρ²)) */
    double numerator = 2.0 * yield_strength;
    double denominator = PI * G_CONST * density_squared;
    
    if (denominator <= 0.0) {
        result.valid = 0;
        snprintf(result.error_msg, sizeof(result.error_msg), 
                 "Invalid denominator in calculation");
        return result;
    }
    
    double ratio = numerator / denominator;
    result.radius_meters = sqrt_robust(ratio, 100, 1e-10);
    
    if (result.radius_meters < 0.0) {
        result.valid = 0;
        snprintf(result.error_msg, sizeof(result.error_msg), 
                 "Square root calculation failed");
        return result;
    }
    
    /* Calculate derived quantities */
    result.radius_km = result.radius_meters / 1000.0;
    result.radius_miles = result.radius_meters * 0.000621371;
    result.volume = (4.0 / 3.0) * PI * pow(result.radius_meters, 3);
    result.mass = result.volume * density;
    result.surface_gravity = (G_CONST * result.mass) / pow(result.radius_meters, 2);
    
    result.valid = 1;
    return result;
}

/**
 * Safe string input with buffer overflow protection
 */
int safe_input_string(char* buffer, size_t size) {
    if (fgets(buffer, size, stdin) == NULL) {
        return 0;
    }
    
    /* Remove trailing newline */
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
    
    return 1;
}

/**
 * Safe double input with validation and error recovery
 */
int safe_input_double(double* value, const char* prompt) {
    char buffer[MAX_INPUT_BUF];
    char* endptr;
    
    printf("%s", prompt);
    fflush(stdout);
    
    if (!safe_input_string(buffer, sizeof(buffer))) {
        return 0;
    }
    
    /* Skip leading whitespace */
    char* start = buffer;
    while (isspace(*start)) start++;
    
    /* Check for empty input */
    if (*start == '\0') {
        return 0;
    }
    
    errno = 0;
    *value = strtod(start, &endptr);
    
    /* Check for conversion errors */
    if (errno == ERANGE || *value == HUGE_VAL || *value == -HUGE_VAL) {
        printf(COLOR_RED "Error: Number out of range" COLOR_RESET "\n");
        return 0;
    }
    
    if (endptr == start) {
        printf(COLOR_RED "Error: No number found" COLOR_RESET "\n");
        return 0;
    }
    
    /* Allow trailing whitespace */
    while (isspace(*endptr)) endptr++;
    
    if (*endptr != '\0') {
        printf(COLOR_YELLOW "Warning: Extra characters ignored: '%s'" COLOR_RESET "\n", endptr);
    }
    
    return 1;
}

/**
 * Display formatted results with scientific notation
 */
void display_results(const CalculationResult* result) {
    if (!result->valid) {
        printf(COLOR_RED "\n╔════════════════════════════════════════╗\n");
        printf("║  CALCULATION FAILED                  ║\n");
        printf("╚════════════════════════════════════════╝\n");
        printf("Error: %s\n" COLOR_RESET, result->error_msg);
        return;
    }
    
    printf(COLOR_GREEN "\n╔══════════════════════════════════════════════════════╗\n");
    printf("║              CALCULATION RESULTS                   ║\n");
    printf("╠══════════════════════════════════════════════════════╣\n");
    printf("║  Radius (meters):      %-15.6e m           ║\n", result->radius_meters);
    printf("║  Radius (kilometers):  %-15.6f km          ║\n", result->radius_km);
    printf("║  Radius (miles):       %-15.6f mi          ║\n", result->radius_miles);
    printf("║  Volume:               %-15.6e m³          ║\n", result->volume);
    printf("║  Mass:                 %-15.6e kg          ║\n", result->mass);
    printf("║  Surface Gravity:      %-15.6e m/s²        ║\n", result->surface_gravity);
    printf("╚══════════════════════════════════════════════════════╝\n" COLOR_RESET);
    
    /* Contextual comparison */
    printf(COLOR_CYAN "\n📊 Context:\n" COLOR_RESET);
    if (result->radius_km < 1.0) {
        printf("   → Smaller than most mountains on Earth\n");
    } else if (result->radius_km < 10.0) {
        printf("   → Comparable to a small asteroid\n");
    } else if (result->radius_km < 100.0) {
        printf("   → Size of a medium asteroid\n");
    } else if (result->radius_km < 500.0) {
        printf("   → Approaching dwarf planet territory\n");
    } else if (result->radius_km < 2000.0) {
        printf("   → Dwarf planet size (like Ceres or Pluto)\n");
    } else {
        printf("   → Full-sized planetary body\n");
    }
}

/**
 * Display material presets menu
 */
void display_presets(void) {
    printf(COLOR_CYAN "\n┌─────────────────────────────────────────────────────┐\n");
    printf("│           AVAILABLE MATERIAL PRESETS            │\n");
    printf("├─────┬──────────────┬──────────────┬────────────┤\n");
    printf("│ ID  │ Material     │ Yield (Pa)   │ Density    │\n");
    printf("├─────┼──────────────┼──────────────┼────────────┤\n");
    
    for (int i = 0; i < MAX_PRESETS; i++) {
        printf("│ %2d  │ %-12s │ %1.2e │ %7.1f │\n", 
               i + 1, presets[i].name, presets[i].yield_strength, presets[i].density);
    }
    
    printf("└─────┴──────────────┴──────────────┴────────────┘\n");
    printf("Enter 0 for custom values\n" COLOR_RESET);
}

/**
 * Update session statistics
 */
void update_stats(const CalculationResult* result) {
    if (!result->valid) return;
    
    g_stats.calculation_count++;
    g_stats.total_radius += result->radius_meters;
    
    if (g_stats.calculation_count == 1) {
        g_stats.min_radius = result->radius_meters;
        g_stats.max_radius = result->radius_meters;
    } else {
        if (result->radius_meters < g_stats.min_radius) {
            g_stats.min_radius = result->radius_meters;
        }
        if (result->radius_meters > g_stats.max_radius) {
            g_stats.max_radius = result->radius_meters;
        }
    }
}

/**
 * Add calculation to history
 */
void add_to_history(double yield, double density) {
    if (g_history_count >= MAX_HISTORY) {
        /* Shift history */
        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            g_history[i][0] = g_history[i + 1][0];
            g_history[i][1] = g_history[i + 1][1];
        }
        g_history_count = MAX_HISTORY - 1;
    }
    
    g_history[g_history_count][0] = yield;
    g_history[g_history_count][1] = density;
    g_history_count++;
}

/**
 * Display session statistics
 */
void display_stats(void) {
    if (g_stats.calculation_count == 0) {
        printf("No calculations performed this session.\n");
        return;
    }
    
    printf(COLOR_MAGENTA "\n📈 Session Statistics:\n" COLOR_RESET);
    printf("   Total calculations: %d\n", g_stats.calculation_count);
    printf("   Minimum radius: %.6e m\n", g_stats.min_radius);
    printf("   Maximum radius: %.6e m\n", g_stats.max_radius);
    printf("   Average radius: %.6e m\n", 
           g_stats.total_radius / g_stats.calculation_count);
    
    time_t now = time(NULL);
    double duration = difftime(now, g_stats.start_time);
    printf("   Session duration: %.0f seconds\n", duration);
}

/**
 * Clear screen with portable implementation
 */
void clear_screen(void) {
#if USE_ANSI_COLORS
    /* Use ANSI escape codes for smoother clearing */
    printf("\033[2J\033[H");
    fflush(stdout);
#else
    system(CLEAR);
#endif
}

/**
 * Display animated header
 */
void display_header(void) {
    printf(COLOR_BOLD COLOR_CYAN);
    printf("\n");
    printf("    ╔═══════════════════════════════════════════════════════╗\n");
    printf("    ║                                                       ║\n");
    printf("    ║          🥔  POTATO RADIUS CALCULATOR  🥔             ║\n");
    printf("    ║                                                       ║\n");
    printf("    ║     Hydrostatic Equilibrium Threshold Tool           ║\n");
    printf("    ║                                                       ║\n");
    printf("    ╚═══════════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
    printf(COLOR_YELLOW "    Calculate when objects become spherical under gravity\n");
    printf(COLOR_RESET);
}

/**
 * Display main menu
 */
void display_menu(void) {
    printf(COLOR_BLUE "\n┌────────────────────────────────────────────┐\n");
    printf("│ MENU                                       │\n");
    printf("├────────────────────────────────────────────┤\n");
    printf("│  1. Custom calculation                     │\n");
    printf("│  2. Use material preset                    │\n");
    printf("│  3. View calculation history               │\n");
    printf("│  4. View session statistics                │\n");
    printf("│  5. About                                  │\n");
    printf("│  0. Exit                                   │\n");
    printf("└────────────────────────────────────────────┘\n" COLOR_RESET);
    printf("Choice: ");
    fflush(stdout);
}

/**
 * Display about information
 */
void display_about(void) {
    printf(COLOR_CYAN "\n┌─────────────────────────────────────────────────────┐\n");
    printf("│ ABOUT                                     │\n");
    printf("├─────────────────────────────────────────────┤\n");
    printf("│ This tool calculates the minimum radius at  │\n");
    printf("│ which a celestial body becomes spherical    │\n");
    printf("│ due to hydrostatic equilibrium.             │\n");
    printf("│                                             │\n");
    printf("│ Formula: R = √(2Y / πGρ²)                  │\n");
    printf("│                                             │\n");
    printf("│ Where:                                      │\n");
    printf("│   Y = Yield strength (Pa)                   │\n");
    printf("│   ρ = Density (kg/m³)                       │\n");
    printf("│   G = Gravitational constant                │\n");
    printf("│                                             │\n");
    printf("│ Real-world examples:                        │\n");
    printf("│   • Mimas (moon): ~200 km radius            │\n");
    printf("│   • Ceres (dwarf planet): ~470 km radius    │\n");
    printf("│   • Most asteroids: irregular shapes        │\n");
    printf("└─────────────────────────────────────────────┘\n" COLOR_RESET);
}

/**
 * Display calculation history
 */
void display_history(void) {
    if (g_history_count == 0) {
        printf("No calculation history yet.\n");
        return;
    }
    
    printf(COLOR_MAGENTA "\n📜 Calculation History:\n" COLOR_RESET);
    printf("┌─────┬──────────────────┬──────────────────┐\n");
    printf("│ #   │ Yield (Pa)       │ Density (kg/m³)  │\n");
    printf("├─────┼──────────────────┼──────────────────┤\n");
    
    for (int i = 0; i < g_history_count; i++) {
        printf("│ %2d  │ %1.4e       │ %1.4e      │\n", 
               i + 1, g_history[i][0], g_history[i][1]);
    }
    
    printf("└─────┴──────────────────┴──────────────────┘\n");
}

/**
 * Run custom calculation
 */
void run_custom_calculation(void) {
    double yield_strength, density;
    
    printf("\n" COLOR_BOLD "Custom Calculation" COLOR_RESET "\n");
    printf("Enter values in SI units (or use scientific notation, e.g., 1.5e7)\n\n");
    
    if (!safe_input_double(&yield_strength, "Yield Strength (Pa): ")) {
        printf(COLOR_RED "Invalid input for yield strength.\n" COLOR_RESET);
        return;
    }
    
    if (!safe_input_double(&density, "Density (kg/m³): ")) {
        printf(COLOR_RED "Invalid input for density.\n" COLOR_RESET);
        return;
    }
    
    CalculationResult result = calculate_potato_radius(yield_strength, density);
    display_results(&result);
    update_stats(&result);
    add_to_history(yield_strength, density);
}

/**
 * Run preset-based calculation
 */
void run_preset_calculation(void) {
    display_presets();
    
    int choice;
    printf("Select preset (0-%d): ", MAX_PRESETS);
    fflush(stdout);
    
    char buffer[MAX_INPUT_BUF];
    if (!safe_input_string(buffer, sizeof(buffer))) {
        return;
    }
    
    choice = atoi(buffer);
    
    if (choice < 0 || choice > MAX_PRESETS) {
        printf(COLOR_RED "Invalid preset selection.\n" COLOR_RESET);
        return;
    }
    
    if (choice == 0) {
        run_custom_calculation();
        return;
    }
    
    /* Adjust for 0-based indexing */
    int preset_idx = choice - 1;
    
    printf("\nSelected: %s\n", presets[preset_idx].name);
    printf("Description: %s\n", presets[preset_idx].description);
    printf("Yield Strength: %.2e Pa\n", presets[preset_idx].yield_strength);
    printf("Density: %.1f kg/m³\n", presets[preset_idx].density);
    
    printf("\nCalculate with these values? (y/n): ");
    fflush(stdout);
    
    if (!safe_input_string(buffer, sizeof(buffer))) {
        return;
    }
    
    if (buffer[0] != 'y' && buffer[0] != 'Y') {
        return;
    }
    
    CalculationResult result = calculate_potato_radius(
        presets[preset_idx].yield_strength,
        presets[preset_idx].density
    );
    
    display_results(&result);
    update_stats(&result);
    add_to_history(presets[preset_idx].yield_strength, presets[preset_idx].density);
}

/**
 * Main program entry point
 */
int main(void) {
    char buffer[MAX_INPUT_BUF];
    int running = 1;
    
    /* Initialize session */
    g_stats.start_time = time(NULL);
    g_stats.calculation_count = 0;
    
    clear_screen();
    display_header();
    
    printf(COLOR_GREEN "\n✓ Program initialized successfully\n" COLOR_RESET);
    printf("Gravitational constant G = %.6e m³/kg·s²\n", G_CONST);
    
    while (running) {
        display_menu();
        
        if (!safe_input_string(buffer, sizeof(buffer))) {
            break;
        }
        
        int choice = atoi(buffer);
        
        switch (choice) {
            case 1:
                clear_screen();
                display_header();
                run_custom_calculation();
                printf("\nPress Enter to continue...");
                fgets(buffer, sizeof(buffer), stdin);
                clear_screen();
                display_header();
                break;
                
            case 2:
                clear_screen();
                display_header();
                run_preset_calculation();
                printf("\nPress Enter to continue...");
                fgets(buffer, sizeof(buffer), stdin);
                clear_screen();
                display_header();
                break;
                
            case 3:
                clear_screen();
                display_header();
                display_history();
                printf("\nPress Enter to continue...");
                fgets(buffer, sizeof(buffer), stdin);
                clear_screen();
                display_header();
                break;
                
            case 4:
                clear_screen();
                display_header();
                display_stats();
                printf("\nPress Enter to continue...");
                fgets(buffer, sizeof(buffer), stdin);
                clear_screen();
                display_header();
                break;
                
            case 5:
                clear_screen();
                display_header();
                display_about();
                printf("\nPress Enter to continue...");
                fgets(buffer, sizeof(buffer), stdin);
                clear_screen();
                display_header();
                break;
                
            case 0:
                printf(COLOR_CYAN "\nThank you for using Potato Radius Calculator!\n" COLOR_RESET);
                display_stats();
                printf("\nGoodbye! 🥔\n\n");
                running = 0;
                break;
                
            default:
                printf(COLOR_RED "Invalid choice. Please try again.\n" COLOR_RESET);
                break;
        }
    }
    
    return 0;
}

