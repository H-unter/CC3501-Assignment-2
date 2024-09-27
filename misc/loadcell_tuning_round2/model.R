data = read_excel(
  "mass_to_voltage.xlsx"
)
spline_model <- smooth.spline(
  data$voltage_v, 
  data$mass_kg, 
  df = nrow(data)
)

linear_model = lm(
  mass_kg ~ voltage_v,
  data = data
)
polynomial_model = lm(
  mass_kg ~ poly(voltage_v, 4, raw = TRUE),
  data = data
)
linear_model
polynomial_model
spline_model

plot(
  data$voltage_v,
  data$mass_kg,
  xlab = "Voltage (V)",
  ylab = "Mass (kg)",
  main = "Mass vs Voltage"
)
abline(linear_model, col = "red")
abline(polynomial_model, col = "darkgreen")
lines(spline_model, col = "blue", lwd = 2)

