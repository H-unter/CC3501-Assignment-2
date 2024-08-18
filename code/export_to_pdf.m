function export_to_pdf(fig, file_name)
    % Get figure position
    fig_position = get(fig, 'Position');
    % Set paper properties
    set(fig, 'PaperPositionMode', 'auto');
    set(fig, 'PaperUnits', 'inches');
    set(fig, 'PaperSize', [fig_position(3), fig_position(4)]/96);  % Convert from pixels to inches assuming 96 DPI
    set(fig, 'PaperPosition', [0 0 fig_position(3), fig_position(4)]/96);
    
    % Define output file path
    subfolder_name = 'exported_figures';
    base_dir = pwd; % Current working directory where the MATLAB script is running
    full_subfolder_path = fullfile(base_dir, subfolder_name);
    output_file_name = fullfile(full_subfolder_path, append(file_name, '.pdf'));

    % Print the plot to a PDF file
    print(fig, output_file_name, '-dpdf', '-painters');
end
