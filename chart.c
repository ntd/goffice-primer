/* chart - Experiments with libgoffice-0.10
 * Copyright (C) 2020  Nicola Fontana <ntd at entidi.it>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <goffice/goffice.h>
#include <string.h>

#define CHART(w)    go_graph_widget_get_chart(GO_GRAPH_WIDGET(w))
#define GRAPH(w)    go_graph_widget_get_graph(GO_GRAPH_WIDGET(w))


static gboolean gop_trigger = FALSE;


static void
gop_switch(GtkToggleButton *toggle_button)
{
    gop_trigger = gtk_toggle_button_get_active(toggle_button);
}

static GogObject *
gop_label_new(const gchar *text)
{
    GogObject *label;
    GOData *data;

    label = g_object_new(GOG_TYPE_LABEL, "allow-markup", TRUE, NULL);
    data  = go_data_scalar_str_new(text, FALSE);
    gog_dataset_set_dim(GOG_DATASET(label), 0, data, NULL);

    return label;
}

static void
gop_customize_axis(GogAxis *axis)
{
    GOStyle *style = go_styled_object_get_style(GO_STYLED_OBJECT(axis));
    style->font.color = GO_COLOR_FROM_RGB(0, 0, 255);
    style->line.color = GO_COLOR_FROM_RGB(0, 0, 255);
    go_styled_object_style_changed(GO_STYLED_OBJECT(axis));
}

static void
gop_customize_axis_label(GogObject *label)
{
    GOStyle *style = go_styled_object_get_style(GO_STYLED_OBJECT(label));
    style->font.color = GO_COLOR_FROM_RGB(0, 0, 255);
    go_style_set_font(style, go_font_new_by_name("Sans 18"));
    go_styled_object_style_changed(GO_STYLED_OBJECT(label));
}

static void
gop_customize_plot(GogPlot *plot)
{
    GogObject *label;
    GogAxis *axis;

    /* Customize X axis */
    axis = gog_plot_get_axis(plot, GOG_AXIS_X);
    gop_customize_axis(axis);
    label = gop_label_new("<b>X</b> axis");
    gog_object_add_by_name(GOG_OBJECT(axis), "Label", label);
    gop_customize_axis_label(label);

    /* Customize Y axis */
    axis = gog_plot_get_axis(plot, GOG_AXIS_Y);
    gop_customize_axis(axis);
    label = gop_label_new("<b>Y</b> axis");
    gog_object_add_by_name(GOG_OBJECT(axis), "Label", label);
    gop_customize_axis_label(label);
}

static void
gop_customize_series(GogSeries *series)
{
    GOStyle *style = go_styled_object_get_style(GO_STYLED_OBJECT(series));
    go_style_clear_auto(style);
    style->font.color = GO_COLOR_FROM_RGB(0, 255, 0);
    style->line.color = GO_COLOR_FROM_RGB(255, 0, 0);
    style->line.width = 1.5;
    go_styled_object_style_changed(GO_STYLED_OBJECT(series));
}

static GtkWidget *
gop_graph_widget_new(void)
{
    GtkWidget *widget;
    GogObject *label;

    widget = go_graph_widget_new(NULL);
    label  = gop_label_new("Testing libgoffice viability...");
    gog_object_add_by_name(GOG_OBJECT(GRAPH(widget)), "Title", label);

    return widget;
}

static GogSeries *
gop_series_new(GtkWidget *widget)
{
    GogPlot *plot;
    GogSeries *series;

    plot = gog_plot_new_by_name("GogXYPlot");
    gog_object_add_by_name(GOG_OBJECT(CHART(widget)), "Plot", GOG_OBJECT(plot));
    gop_customize_plot(plot);

    series = gog_plot_new_series(plot);
    gop_customize_series(series);

    return series;
}

static gboolean
gop_producer(GogSeries *series)
{
    static GArray *x = NULL;
    static GArray *y = NULL;

    if (gop_trigger) {
        GOData *data;
        gdouble value;

        /* Set value to the X of the last element */
        if (x == NULL) {
            x = g_array_sized_new(FALSE, FALSE, sizeof(gdouble), 500);
            y = g_array_sized_new(FALSE, FALSE, sizeof(gdouble), 500);
            value = 0;
        } else {
            value = g_array_index(x, gdouble, x->len - 1);
        }

        /* X values */
        value += g_random_double_range(-1, 2);
        x = g_array_append_val(x, value);
        data  = go_data_vector_val_new((gdouble *) x->data, x->len, NULL);
        gog_series_set_dim(series, 0, data, NULL);

        /* Y values */
        value = g_random_double_range(0, 80);
        y = g_array_append_val(y, value);
        data  = go_data_vector_val_new((gdouble *) y->data, y->len, NULL);
        gog_series_set_dim(series, 1, data, NULL);
    }

    return G_SOURCE_CONTINUE;
}

static void
on_startup(GtkApplication *app)
{
    libgoffice_init();
    go_plugins_init(NULL, NULL, NULL, NULL, TRUE, GO_TYPE_PLUGIN_LOADER_MODULE);
}

static void
on_activate(GtkApplication *app)
{
    GtkWidget *widget, *box;
    GogSeries *series;

    widget = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(widget), 640, 480);
    gtk_window_set_title(GTK_WINDOW(widget), "GOffice primer");
    gtk_container_set_border_width(GTK_CONTAINER(widget), 6);
    gtk_widget_show(widget);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(widget), box);

    widget = gtk_toggle_button_new_with_label("Trigger");
    g_signal_connect(widget, "toggled", G_CALLBACK(gop_switch), NULL);
    gtk_box_pack_end(GTK_BOX(box), widget, FALSE, TRUE, 3);

    widget = gop_graph_widget_new();
    gtk_box_pack_start(GTK_BOX(box), widget, TRUE, TRUE, 3);

    series = gop_series_new(widget);
    g_timeout_add(10, (GSourceFunc) gop_producer, series);

    gtk_widget_show_all(box);
}

static void
on_shutdown(GtkApplication *app)
{
    libgoffice_shutdown();
}


int
main(int argc, char *argv[])
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.entidi.goffice-primer", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "startup", G_CALLBACK(on_startup), NULL);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    g_signal_connect(app, "shutdown", G_CALLBACK(on_shutdown), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
