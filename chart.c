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

    return TRUE;
}



int
main (int argc, char *argv[])
{
    GtkWidget *window, *content_area, *action_area, *button;
    GOGraphWidget *graph_widget;
    GogChart *chart;
    GogPlot *plot;
    GogSeries *series;

    gtk_init(&argc, &argv);
    libgoffice_init();
    go_plugins_init(NULL, NULL, NULL, NULL, TRUE, GO_TYPE_PLUGIN_LOADER_MODULE);

    window = gtk_dialog_new_with_buttons("Charting demo", NULL,
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_STOCK_OK, GTK_RESPONSE_NONE,
                                         NULL);
    gtk_window_resize(GTK_WINDOW(window), 640, 480);
    g_signal_connect_swapped(window, "response",
                             G_CALLBACK(gtk_widget_destroy), window);
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(window));
    action_area = gtk_dialog_get_action_area(GTK_DIALOG(window));

    button = g_object_new(GTK_TYPE_TOGGLE_BUTTON, "label", GTK_STOCK_GO_FORWARD,
                          "use-stock", TRUE, "use-underline", TRUE, NULL);
    g_signal_connect(button, "toggled", G_CALLBACK(gop_switch), NULL);
    gtk_container_add(GTK_CONTAINER(action_area), button);

    graph_widget = (GOGraphWidget *) go_graph_widget_new(NULL);
    gop_graph_widget_prepare(graph_widget);
    gtk_container_add(GTK_CONTAINER(content_area), GTK_WIDGET(graph_widget));

    plot = (GogPlot *) gog_plot_new_by_name("GogXYPlot");
    chart = go_graph_widget_get_chart(graph_widget);
    gog_object_add_by_name(GOG_OBJECT(chart), "Plot", GOG_OBJECT(plot));

    series = gog_plot_new_series(plot);
    gop_series_prepare(series);

    gtk_widget_show_all(window);
    g_timeout_add(10, (GSourceFunc) gop_producer, series);
    gtk_dialog_run(GTK_DIALOG(window));

    libgoffice_shutdown();

    return 0;
}
