#define _DEFAULT_SOURCE
#include <gtk/gtk.h>
#include <vte/vte.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char font_val[128] = "Monospace 11";
static char bg_val[16]    = "#0D0F18";
static char fg_val[16]    = "#FFFFFF";
static char shell_val[64] = ""; // Boş bırakırsak sisteminkini alır
static float width_val    = 1.0;

// Kısayollar (Ctrl+Shift+C/V)
gboolean on_key_press(GtkWidget *terminal, GdkEventKey *event, gpointer user_data) {
    if ((event->state & GDK_CONTROL_MASK) && (event->state & GDK_SHIFT_MASK)) {
        if (event->keyval == GDK_KEY_C || event->keyval == GDK_KEY_c) {
            vte_terminal_copy_clipboard_format(VTE_TERMINAL(terminal), VTE_FORMAT_TEXT);
            return TRUE;
        }
        if (event->keyval == GDK_KEY_V || event->keyval == GDK_KEY_v) {
            vte_terminal_paste_clipboard(VTE_TERMINAL(terminal));
            return TRUE;
        }
    }
    return FALSE;
}

void try_load(const char *path) {
    FILE *file = fopen(path, "r");
    if (!file) return;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "font_name=", 10) == 0) sscanf(line, "font_name=%[^\n]", font_val);
        else if (strncmp(line, "bg_color=", 9) == 0) sscanf(line, "bg_color=%s", bg_val);
        else if (strncmp(line, "fg_color=", 9) == 0) sscanf(line, "fg_color=%s", fg_val);
        else if (strncmp(line, "shell=", 6) == 0) sscanf(line, "shell=%s", shell_val);
        else if (strncmp(line, "cell_width=", 11) == 0) sscanf(line, "cell_width=%f", &width_val);
    }
    fclose(file);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    
    // Varsayılan shell'i sistemden al
    char *sys_shell = getenv("SHELL");
    if (sys_shell) strcpy(shell_val, sys_shell);
    else strcpy(shell_val, "/bin/bash");

    // Config Yükle
    char user_path[256];
    snprintf(user_path, sizeof(user_path), "%s/.config/s-console/s-console.conf", getenv("HOME"));
    if (access(user_path, F_OK) == 0) try_load(user_path);
    else try_load("/etc/s-console/s-console.conf");

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "S-Console");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 600);
    gtk_window_set_icon_name(GTK_WINDOW(window), "s-console");

    GtkWidget *terminal = vte_terminal_new();
    vte_terminal_set_scrollback_lines(VTE_TERMINAL(terminal), 5000);
    g_signal_connect(terminal, "key-press-event", G_CALLBACK(on_key_press), NULL);

    PangoFontDescription *font_desc = pango_font_description_from_string(font_val);
    vte_terminal_set_font(VTE_TERMINAL(terminal), font_desc);
    vte_terminal_set_cell_width_scale(VTE_TERMINAL(terminal), width_val);
    pango_font_description_free(font_desc);

    GdkRGBA bg, fg;
    gdk_rgba_parse(&bg, bg_val); bg.alpha = 1.0;
    gdk_rgba_parse(&fg, fg_val);
    vte_terminal_set_colors(VTE_TERMINAL(terminal), &fg, &bg, NULL, 0);

    // Terminali başlat
    gchar **command = (gchar *[]){shell_val, "-c", "fastfetch || neofetch || echo 'S-Console'; exec $SHELL", NULL};
    vte_terminal_spawn_async(VTE_TERMINAL(terminal), VTE_PTY_DEFAULT, NULL, command, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, -1, NULL, NULL, NULL);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_container_add(GTK_CONTAINER(window), terminal);
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
