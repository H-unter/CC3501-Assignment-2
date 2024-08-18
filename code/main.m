
font_size = 20;
fig = figure;
hold on;
    x = linspace(0, 2*pi, 1000);
    y = sin(x);
    y_noisy = y + 0.2*randn(size(x));
    plot(x, y_noisy, 'DisplayName', 'Noisy Signal');
    plot(x, y, 'DisplayName', 'Clean Signal', LineWidth=3);
    xlabel('$$x$$', 'Interpreter', 'latex', 'FontSize', font_size);
    ylabel('$$y=\sin(x)$$', 'Interpreter', 'latex', 'FontSize', font_size, Rotation=0);
    fig_ax = gca;
    fig_ax.TickLabelInterpreter = 'latex';
    fig_ax.FontSize = font_size;
    legend('show', 'Interpreter', 'latex', 'FontSize', font_size, 'Box', 'off');
hold off;
export_to_pdf(fig, 'my_plot');


