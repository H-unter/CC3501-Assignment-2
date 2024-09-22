# Create vectors for the data
#m <- c(2, 2.2, 2.4, 2.6, 2.8, 3, 3.2, 3.4, 3.6, 3.8, 4, 4.2, 4.4, 4.6, 4.8, 5)
#v <- c(0.824, 0.89, 0.962, 0.994, 1.02, 1.05, 1.09, 1.14, 1.18, 1.22, 1.24, 1.29, 1.33, 1.37, 1.39, 1.44)


# Define R_G as a variable
R_G <- 670  # Example value, you can change this

# Compute the expression
G <- 1 + (49.4 * 10^3 / R_G)
m = c(0, 0.2, 0.4, 0.6, 1.8, 2, 2.2, 2.4, 2.6, 2.8, 3, 3.2, 3.4, 3.6, 3.8, 4, 4.2, 4.4, 4.6, 4.8, 5)
v_out = c(0.76, 0.76, 0.76, 0.76, 0.76, 0.824, 0.89, 0.962, 0.994, 1.02, 1.05, 1.09, 1.14, 1.18, 1.22, 1.24, 1.29, 1.33, 1.37, 1.39, 1.44)
deadzone_indeces <- which(v_out == v_out[1])
v_raw = v_out / G


# Create linear models
linear_model_with_deadzone <- lm(m ~ v_raw)  # With deadzone
linear_model_without_deadzone <- lm(m ~ v_raw, subset = -deadzone_indeces)  # Exclude deadzone

# Plot the data
plot(v_raw, m, main = "Linear Model of Given Data", xlab = "Raw Voltage (V)", ylab = "Mass (kg)", pch = 16, col = "blue")

# Add linear regression lines
abline(linear_model_with_deadzone, col = "blue", lty = 2)  # With deadzone (dashed)
abline(linear_model_without_deadzone, col = "red")  # Without deadzone (solid)

# Add legend
legend("topleft", legend = c("With Deadzone", "Without Deadzone"), col = c("blue", "red"), lty = c(2, 1), pch = 16)

