// Ohm's Law Circuit Analysis Tool 

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

struct Circuit {
    double voltage = 0;
    double current = 0;
    double resistance = 0;
    double power = 0;
};

std::string readLine() {
    std::string line;
    if (!std::getline(std::cin, line)) {
        throw std::runtime_error("Input closed.");
    }
    return line;
}

int readChoice() {
    while (true) {
        std::cout << "Choice: ";
        std::istringstream input(readLine());
        int choice;
        std::string extra;
        if ((input >> choice) && !(input >> extra) && choice >= 0 && choice <= 6) {
            return choice;
        }
        std::cout << "Enter a whole number from 0 to 6.\n";
    }
}

// Accept a number and an optional case-sensitive unit example 2200 ohm or 2.2 kohm
double readQuantity(const std::string& name, const std::string& baseUnit,
                    const std::string& allowedUnits) {
    while (true) {
        std::cout << name << " (" << allowedUnits << "): ";
        std::istringstream input(readLine());
        double value;
        std::string unit, extra;
        if (!(input >> value) || !std::isfinite(value) || value <= 0) {
            std::cout << "Enter a finite number greater than zero.\n";
            continue;
        }
        if (!(input >> unit)) unit = baseUnit;
        if (input >> extra) {
            std::cout << "Enter one number and an optional unit.\n";
            continue;
        }

        double multiplier = 0;
        if (unit == baseUnit) multiplier = 1;
        else if (unit == "u" + baseUnit) multiplier = 1e-6;
        else if (unit == "m" + baseUnit) multiplier = 1e-3;
        else if (unit == "k" + baseUnit) multiplier = 1e3;
        else if (unit == "M" + baseUnit) multiplier = 1e6;
        else {
            std::cout << "Invalid unit. Units are case-sensitive; use "
                      << allowedUnits << ".\n";
            continue;
        }
        value *= multiplier;
        if (!std::isfinite(value) || value <= 0) {
            std::cout << "Value is outside the supported numeric range.\n";
            continue;
        }
        return value;
    }
}

double voltage() { return readQuantity("Voltage", "V", "uV, mV, V, kV, MV"); }
double current() { return readQuantity("Current", "A", "uA, mA, A, kA, MA"); }
double resistance() { return readQuantity("Resistance", "ohm", "uohm, mohm, ohm, kohm, Mohm"); }
double power() { return readQuantity("Power", "W", "uW, mW, W, kW, MW"); }

// Calculate the remaining quantities from each of the six possible pairs
Circuit calculate(int choice) {
    Circuit c;
    switch (choice) {
        case 1:
            c.voltage = voltage(); c.current = current();
            c.resistance = c.voltage / c.current;
            c.power = c.voltage * c.current;
            break;
        case 2:
            c.voltage = voltage(); c.resistance = resistance();
            c.current = c.voltage / c.resistance;
            c.power = c.voltage * c.current;
            break;
        case 3:
            c.current = current(); c.resistance = resistance();
            c.voltage = c.current * c.resistance;
            c.power = c.voltage * c.current;
            break;
        case 4:
            c.voltage = voltage(); c.power = power();
            c.current = c.power / c.voltage;
            c.resistance = c.voltage / c.current;
            break;
        case 5:
            c.current = current(); c.power = power();
            c.voltage = c.power / c.current;
            c.resistance = c.voltage / c.current;
            break;
        case 6:
            c.resistance = resistance(); c.power = power();
            // Split square roots avoid overflow in the intermediate P * R.
            c.voltage = std::sqrt(c.power) * std::sqrt(c.resistance);
            c.current = std::sqrt(c.power) / std::sqrt(c.resistance);
            break;
    }
    return c;
}

void printResults(const Circuit& c) {
    for (double value : {c.voltage, c.current, c.resistance, c.power}) {
        if (!std::isfinite(value) || value <= 0) {
            std::cout << "Calculation exceeds the supported numeric range. Try other values.\n";
            return;
        }
    }
    std::cout << std::setprecision(8)
              << "\n--- Results (SI units) ---\n"
              << "Voltage:    " << c.voltage << " V\n"
              << "Current:    " << c.current << " A\n"
              << "Resistance: " << c.resistance << " ohm\n"
              << "Power:      " << c.power << " W\n"
              << "Formulas: V = I * R; P = V * I\n";
}

int main() {
    std::cout << "OHM'S LAW CIRCUIT ANALYSIS TOOL\n"
              << "Ideal DC resistor; enter positive magnitudes only.\n"
              << "Examples: 12 V, 20 mA, 2.2 kohm. Bare numbers use SI units.\n";
    try {
        while (true) {
            std::cout << "\nChoose your two known quantities:\n"
                      << "1. Voltage and current\n"
                      << "2. Voltage and resistance\n"
                      << "3. Current and resistance\n"
                      << "4. Voltage and power\n"
                      << "5. Current and power\n"
                      << "6. Resistance and power\n"
                      << "0. Exit\n";
            int choice = readChoice();
            if (choice == 0) break;
            printResults(calculate(choice));
        }
    } catch (const std::runtime_error& error) {
        std::cout << '\n' << error.what() << '\n';
    }
    return 0;
}
