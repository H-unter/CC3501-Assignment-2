# Assuming the CSV has columns: voltage, moving_average_.01, moving_average_.05, moving_average_.10, moving_average_.20
readings <- read.csv("putty.log", header=TRUE)
pdf("smoothed_readings_plot.pdf", width = 14, height = 6)

# Plot the raw voltage data
plot(
  readings$voltage,
  type = "l",       # Line plot
  col = "blue",     # Color of raw data
  lwd = 2,          # Line width
  xlab = "Time", 
  ylab = "Voltage (V)",
  ylim = range(c(readings$voltage, readings$moving_average_.01, 
                 readings$moving_average_.05, readings$moving_average_.10, 
                 readings$moving_average_.20)), # Adjust y-axis to fit all series
  main = "Raw and Smoothed Voltages (Exponential Moving Average)"
)

# Add lines for each smoothed moving average level
lines(readings$moving_average_.01, col = "red", lwd = 2)
lines(readings$moving_average_.05, col = "green", lwd = 2)
lines(readings$moving_average_.10, col = "purple", lwd = 2)
lines(readings$moving_average_.20, col = "orange", lwd = 2)

# Add a legend to distinguish the series
legend(
  "topright", 
  legend = c("Raw Voltage", "EMA (alpha = 0.01)", "EMA (alpha = 0.05)", "EMA (alpha = 0.10)", "EMA (alpha = 0.20)"), 
  col = c("blue", "red", "green", "purple", "orange"), 
  lty = 1,  # Line type
  lwd = 2  # Line width
)
dev.off()
