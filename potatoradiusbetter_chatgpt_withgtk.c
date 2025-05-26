#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define G 6.674e-11

typedef struct {
    char material[50];
    float ten_es;
    float densidade;
} Material;

Material materiais[10];
int n_materiais = 0;

GtkWidget *label_result;

void carregar_dados_csv(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Erro ao abrir o arquivo CSV");
        exit(1);
    }
    char linha[256];
    fgets(linha, sizeof(linha), f); // pula cabeçalho
    n_materiais = 0;

    while (fgets(linha, sizeof(linha), f) && n_materiais < 10) {
        char *token = strtok(linha, ",");
        if (!token) continue;
        strncpy(materiais[n_materiais].material, token, sizeof(materiais[n_materiais].material)-1);

        token = strtok(NULL, ",");
        materiais[n_materiais].ten_es = atof(token);

        token = strtok(NULL, ",");
        materiais[n_materiais].densidade = atof(token);

        n_materiais++;
    }
    fclose(f);
}

void on_button_calcular_clicked(GtkButton *button, gpointer user_data) {
    GtkComboBoxText *combo = GTK_COMBO_BOX_TEXT(user_data);
    int index = gtk_combo_box_get_active(GTK_COMBO_BOX(combo));
    if (index < 0 || index >= n_materiais) {
        gtk_label_set_text(GTK_LABEL(label_result), "Selecione um material válido.");
        return;
    }

    Material *m = &materiais[index];

    // calcula potato radius (em metros)
    float R = sqrt((3 * m->ten_es) / (4 * M_PI * G * m->densidade * m->densidade));
    char resultado[256];
    snprintf(resultado, sizeof(resultado),
             "Material: %s\nLimite de escoamento: %.2e Pa\nDensidade: %.0f kg/m³\nPotato Radius: %.2f km",
             m->material, m->ten_es, m->densidade, R / 1000.0);

    gtk_label_set_text(GTK_LABEL(label_result), resultado);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    carregar_dados_csv("potatoradios.csv");

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Calculadora Potato Radius");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget *combo = gtk_combo_box_text_new();

    for (int i = 0; i < n_materiais; i++) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), materiais[i].material);
    }
    gtk_box_pack_start(GTK_BOX(vbox), combo, FALSE, FALSE, 0);

    GtkWidget *button = gtk_button_new_with_label("Calcular");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);

    label_result = gtk_label_new("Escolha um material e clique em Calcular.");
    gtk_box_pack_start(GTK_BOX(vbox), label_result, TRUE, TRUE, 0);

    g_signal_connect(button, "clicked", G_CALLBACK(on_button_calcular_clicked), combo);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
