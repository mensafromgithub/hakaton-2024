//Подключаем заголовочные файлы gtk
#include <gtk/gtk.h>

#define CLAMP(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

int width, height; // Размеры окна
gchar *filename; // Название файла фото

GdkPixbuf *globalpixbuf;
GdkPixbuf *globalpixbuf_save;

double kontrastnistnost;
double S_dveri;
double shirina_fakela;
double za_granichi_fakela;
double stoimost;


// Слот выхода из программы
G_MODULE_EXPORT void onExit(GtkWidget * w, gpointer data) {
    gtk_main_quit();
}

void otrisovka(GdkPixbuf *scaled_pixbuf, gpointer image) {
    gtk_image_set_from_pixbuf(GTK_IMAGE(image), scaled_pixbuf);
    g_object_unref(scaled_pixbuf);
}

GdkPixbuf* adjust_contrast(GdkPixbuf* pixbuf, float factor) {
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

    GdkPixbuf* new_pixbuf = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(pixbuf),
                                            gdk_pixbuf_get_has_alpha(pixbuf),
                                            gdk_pixbuf_get_bits_per_sample(pixbuf),
                                            width, height);
    guchar *new_pixels = gdk_pixbuf_get_pixels(new_pixbuf);
    int new_rowstride = gdk_pixbuf_get_rowstride(new_pixbuf);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int offset = y * rowstride + x * n_channels;
            int new_offset = y * new_rowstride + x * n_channels;

            for (int c = 0; c < n_channels; c++) {
                if (n_channels == 4 && c == 3) {
                    new_pixels[new_offset + c] = pixels[offset + c];
                    continue;
                }

                int value = pixels[offset + c];
                // Изменяем контрастность
                int new_value = (int)(((float)value - 128.0f) * factor + 128.0f);
                new_value = CLAMP(new_value, 0, 255); // Ограничиваем значение от 0 до 255
                new_pixels[new_offset + c] = (guchar)new_value;
            }
        }
    }

    return new_pixbuf;
}


G_MODULE_EXPORT void get_kontrastnost(GtkWidget *widget, gpointer adjustment) {
    gdouble value = gtk_adjustment_get_value(adjustment);
    kontrastnistnost = (double)value;
    //g_print("%f \n", kontrastnistnost);
}

G_MODULE_EXPORT void get_S_dveri(GtkWidget *widget, gpointer adjustment) {
    gdouble value = gtk_adjustment_get_value(adjustment);
    S_dveri = (double)value;
    //g_print("%f \n", S_dveri);
}

G_MODULE_EXPORT void get_shirina_fakela(GtkWidget *widget, gpointer adjustment) {
    gdouble value = gtk_adjustment_get_value(adjustment);
    shirina_fakela = (double)value;
    //g_print("%f \n", shirina_fakela);
}

G_MODULE_EXPORT void get_za_granichi_fakela(GtkWidget *widget, gpointer adjustment) {
    gdouble value = gtk_adjustment_get_value(adjustment);
    za_granichi_fakela = (double)value;
    //g_print("%f \n", za_granichi_fakela);
}

G_MODULE_EXPORT void get_stoimost(GtkWidget *widget, gpointer adjustment) {
    gdouble value = gtk_adjustment_get_value(adjustment);
    stoimost = (double)value;
    //g_print("%f \n", stoimost);
}

G_MODULE_EXPORT void set_kontrastnost(GtkWidget *widget, gpointer image) {
    float contrast_factor = kontrastnistnost/50; // Увеличиваем контрастность
    GdkPixbuf *new_pixbuf = adjust_contrast(globalpixbuf, contrast_factor);
    globalpixbuf_save = new_pixbuf;
    otrisovka(new_pixbuf, image);
}

G_MODULE_EXPORT void get_window_size(GtkWidget *widget, gpointer window) {
    gtk_window_get_size(window, &width, &height);
}

// Функция для обработки выбора пункта меню "О программе"
G_MODULE_EXPORT void on_about_menu_item_activate(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(data),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "UI Ferma [Prime]\nВерсия 0.2.1");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

G_MODULE_EXPORT void on_open_image_menu_item_activate(GtkWidget *widget, gpointer image) {
    GtkWidget *dialog;
    GtkFileChooser *file_chooser;

    //globalimage = image; // Костыль PRIME

    dialog = gtk_file_chooser_dialog_new("Открыть изображение",
                                         GTK_WINDOW(widget),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Отмена", GTK_RESPONSE_CANCEL,
                                         "_Открыть", GTK_RESPONSE_ACCEPT, 
                                         NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        file_chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(file_chooser);

        // Установка изображения в виджет
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
        if (pixbuf) {
            // Изменение размера изображения, чтобы оно вписывалось в АААААА
            GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);
            globalpixbuf = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR); // ХЗ Правильно ли (ДА !!)
            globalpixbuf_save = globalpixbuf;
            otrisovka(scaled_pixbuf, image);
            g_object_unref(pixbuf);
        } 
        else {
            g_print("Не удалось загрузить изображение: %s\n", filename);
        }
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}



G_MODULE_EXPORT void OBRABOTKA_PRIME(GtkWidget *widget, gpointer window) {
    GError *error = NULL;
    if (!gdk_pixbuf_save(globalpixbuf_save, "output_image.png", "png", &error, NULL)) {
        g_print("Ошибка при сохранении изображения: %s\n", error->message);
        g_error_free(error);
        g_object_unref(globalpixbuf_save);
    }
    g_print("Изображение успешно сохраненно.\n");
    
    GtkWidget* dialog = gtk_message_dialog_new(
        NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_NONE,
        "Обработка изображения... Подождите...");
    gtk_dialog_run(GTK_DIALOG(dialog));

    gtk_widget_destroy(dialog);

    //gtk_label_set_text(label, text);
}

int main(int argc, char * argv[]) {
    //Инициализация gtk
    gtk_init(&argc, &argv);

    // Этот объект будет считывать данные из формы
    // и создавать интерфейс на их основе
    GtkBuilder * ui_builder;
    // Если будут какие-либо ошибки, то они появятся здесь
    GError * err = NULL;

    // Инициализируем GtkBuilder
    ui_builder = gtk_builder_new();
    //Загрузка файла с UI в GtkBuilder
    if(!gtk_builder_add_from_file(ui_builder, "./main.glade", &err)) {
        g_critical("Не вышло загрузить файл с UI : %s", err->message);
        g_error_free(err);
    }

    GtkWidget * window = GTK_WIDGET(gtk_builder_get_object(ui_builder, "main_window"));

    gtk_builder_connect_signals(ui_builder, NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}