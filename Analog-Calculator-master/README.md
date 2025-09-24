# Analog Computer

The Analog Computer allows users to input complex mathematical expressions, processed by analog circuitry to provide accurate results. This innovative device bridges the gap between digital and analog worlds, offering a unique computing experience.

![WhatsApp Image 2024-06-26 at 20 06 45_1152d6e3](https://github.com/HardikJainGit/Analog-Calculator/assets/133627261/92d54893-f969-4589-b7af-ac9e66e79cc1)

## Features

- **Arithmetic Operations**: Perform addition, subtraction, multiplication, and nth root calculations for numbers ranging from -12 to 12.
- **Parentheses for Grouping**: Support for complex expressions with grouping using parentheses.
- **Wireless Input**: Inputs can be given wirelessly over Wi-Fi connection by phone or laptops handled by NodeMCU unit.
- **Arduino Integration**: Input expressions are parsed and converted for Arduino using the Arduino Stack Library.
- **Inputs via PWM**: PWM square wave is averaged to its DC value by a RC low-pass filter and given to the analog circuit.
- **Display Output**: Calculated results are displayed on the user's personal computer screen.

## How It Works

1. **User Input**: Enter a mathematical expression on a personal computer.
2. **Expression Parsing**: The computer parses the expression into numbers, operators, and parentheses.
3. **Arduino Interface**: Arduino translates the parsed expression for analog circuitry.
4. **Analog Circuitry**: Analog circuitry performs basic arithmetic operations.
5. **Result Display**: The computed result is sent back to the personal computer screen for display.
