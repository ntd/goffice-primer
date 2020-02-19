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


static gboolean gop_trigger = FALSE;


static void
gop_switch(GtkToggleButton *toggle_button)
{
    gop_trigger = gtk_toggle_button_get_active(toggle_button);
}

static void
gop_graph_widget_prepare(GOGraphWidget *graph_widget)
{
    GogGraph *graph;
    GogObject *label;
    GOStyle *style;
    PangoFontDescription *desc;
    GOData *data;

    graph = go_graph_widget_get_graph(graph_widget);
    label = g_object_new(GOG_TYPE_LABEL, NULL);
    gog_object_add_by_name(GOG_OBJECT(graph), "Title", label);

    style = go_styled_object_get_style(GO_STYLED_OBJECT(label));
    desc = pango_font_description_from_string("Sand bold 16");
    go_style_set_font_desc(style, desc);
    go_styled_object_style_changed(GO_STYLED_OBJECT(label));

    data = go_data_scalar_str_new("Testing libgoffice viability...", FALSE);
    gog_dataset_set_dim(GOG_DATASET(label), 0, data, NULL);
}

static void
gop_series_prepare(GogSeries *series)
{
    GOData *data;
    GOStyle *style;
    GOMarker *marker;

    data = go_data_vector_val_new(NULL, 0, NULL);
    gog_series_set_dim(series, 1, data, NULL);

    style = go_styled_object_get_style(GO_STYLED_OBJECT(series));
    marker = go_marker_new();
    go_marker_set_shape(marker, GO_MARKER_NONE);
    go_style_set_marker(style, marker);
    style->marker.auto_shape = FALSE;
    style->marker.auto_outline_color = FALSE;
    style->marker.auto_fill_color = FALSE;
    go_styled_object_style_changed(GO_STYLED_OBJECT(series));
}

static gboolean
gop_producer(GogSeries *series)
{
    if (gop_trigger && GOG_IS_DATASET(series)) {
        gdouble *old_values, *values;
        gint old_len, len;
        GOData *old_data, *data;

        old_data = gog_dataset_get_dim(GOG_DATASET(series), 1);
        g_assert(old_data != NULL);
        old_len = go_data_vector_get_len(GO_DATA_VECTOR(old_data));
        g_assert(old_len >= 0);
        old_values = go_data_vector_get_values(GO_DATA_VECTOR(old_data));

        len = old_len + 1;
        values = g_new(gdouble, len);
        if (old_values != NULL)
            memcpy(values, old_values, sizeof(gdouble)*old_len);
        values[old_len] = g_random_double_range(0, 80);
        data = go_data_vector_val_new(values, len, NULL);

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
    GogChart *chart;
    GogPlot *plot;
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

    widget = go_graph_widget_new(NULL);
    gop_graph_widget_prepare(GO_GRAPH_WIDGET(widget));
    gtk_box_pack_start(GTK_BOX(box), widget, TRUE, TRUE, 3);

    plot = (GogPlot *) gog_plot_new_by_name("GogXYPlot");
    chart = go_graph_widget_get_chart(GO_GRAPH_WIDGET(widget));
    gog_object_add_by_name(GOG_OBJECT(chart), "Plot", GOG_OBJECT(plot));

    series = gog_plot_new_series(plot);
    gop_series_prepare(series);
    g_timeout_add(50, (GSourceFunc) gop_producer, series);

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
