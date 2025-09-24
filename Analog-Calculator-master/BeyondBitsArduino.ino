#include <SoftwareSerial.h>
#include <Arduino.h>
#include <StackArray.h> // Include Arduino Stack library for stack implementation
#include <Wire.h> // I2C is a two-wire serial communication protocol 

const int bufferSize = 40; // Adjust the size based on your requirements
char tempBuffer[bufferSize]; // Temporary non-volatile buffer
volatile char buffer[bufferSize];
volatile boolean receiveFlag = false;

void receiveEvent(int howMany) { // data received from I2C
  if (howMany <= bufferSize) {
    Wire.readBytes(tempBuffer, howMany); // read and NULL terminate
    tempBuffer[howMany] = '\0'; // Null-terminate the temporary buffer
    
    // Copy the data into the volatile buffer
    for (int i = 0; i <= howMany; ++i) {
      buffer[i] = tempBuffer[i];
    }
    
    receiveFlag = true;
  } 
  // else {
  //   // Handle the case where the received data exceeds the buffer size
  //   // This could be an error condition or a mechanism to handle larger data
  //   // You may need to implement your specific logic here
  // }
}



// Function to find precedence of operators.
int precedence(char op){
  if(op == '+' || op == '-')
    return 1;
  if(op == '*')
    return 2;
  if(op == '^') return 3;
  return 0;
}

// Function to perform arithmetic operations.
// Modified to Perform Arithmetic Operations and Read Analog Value 10 times for Averaging
float applyOp(float a, float b, char op) {

  //int a = (5/24) * (a + 12);
  //int b = (5/24) * (b + 12);

  int pwm1 = 255 * a / 5; // Convert input a to PWM value
  int pwm2 = 255 * b / 5; // Convert input b to PWM value
  int pin;

  if (op=='^')
  {
    pwm2 = 51;
    if (b==0.5)
      pin = A3;
    else if (b==0.3)
      pin = A4;
    else if (b==0.2)
      pin = A5;
  }

  if(op=='*')
  {
    analogWrite(5,51); // 14 digital and 6 analog pins
  }
    
  // Write PWM values to the corresponding pins
  analogWrite(6, pwm1);
  analogWrite(9, pwm2);

  // Introduce a delay to allow the circuit to stabilize
  delay(5000); // Example delay, adjust based on operation

  float sumAnalogValue = 0.0; // Initialize variable to accumulate analog read values
  int readCount = 10; // Number of readings to take

  // Loop to read analog value 10 times
  for(int i = 0; i < readCount; i++) {
    delay(30); 
    // Read and accumulate analog value from the appropriate pin based on operation
    switch (op) {
      // Perform DAC from 10-bit digital to analog
      case '+': 
        sumAnalogValue += ((float)analogRead(A0))*5/1023; // Read analog value from pin A0
        break;
      case '-': 
        sumAnalogValue += ((float)analogRead(A1))*5/1023; // Read analog value from pin A1
        break;
      case '*': 
        sumAnalogValue += ((float)analogRead(A2))*5/1023; // Read analog value from pin A2
        break;
      case '^': 
        sumAnalogValue += ((float)analogRead(pin))*5/1023; // Read analog value from the designated pin based on 'b' value
        break;
    }
  }

  // Calculate the average of the analog values
  float averageAnalogValue = sumAnalogValue / readCount;

  // Return the averaged analog value
  return averageAnalogValue;
}



//float version of isdigit
bool isFloat(String value) {
  bool containsDecimal = false;
  
  // Check if the string is empty
  if (value.length() == 0) {
    return false;
  }
  
  // Iterate through each character
  for (int i = 0; i < value.length(); i++) {
    // Check for an operation character
    if (value[i] == '+' || value[i] == '-' || value[i] == '*' || value[i] == '/') {
      return false;
    }
    // Check for a decimal point
    if (value[i] == '.') {
      // If already found a decimal point, it's not a valid float
      if (containsDecimal) {
        return false;
      }
      containsDecimal = true;
    }
    // Check for a digit or negative sign
    else if (!isdigit(value[i]) && value[i] != '-') {
      return false;
    }
  }
  
  return true;
}

// Function that returns value of expression after evaluation.
float evaluate(String tokens){
  int i;
  
  // stack to store integer values.
  StackArray<float> values;
  
  // stack to store operators.
  StackArray<char> ops;
  
  for(i = 0; i < tokens.length(); i++){
    
    // Current token is a whitespace, skip it.
    if(tokens[i] == ' ')
      continue;
    
    // Current token is an opening brace, push it to 'ops'
    else if(tokens[i] == '('){
      ops.push(tokens[i]);
    }
    
    // Current token is a number, push it to stack for numbers.
    else if(isFloat(String(tokens[i]))){
      float val = 0;
      // There may be more than one digit in number.
      String x="";
      while(i < tokens.length() && isFloat(String(tokens[i]))){
        // Serial.println(i);
        x+=tokens[i];
        // val = (val * 10) + (tokens[i] - '0');
        i++;
      }
        val=x.toFloat();

      values.push(val);
      
      // right now the i points to the character next to the digit,
      // since the for loop also increases the i, we would skip one 
      // token position; we need to decrease the value of i by 1 to
      // correct the offset.
      i--;
    }
    
    // Closing brace encountered, solve entire brace.
    else if(tokens[i] == ')'){
      while(!ops.isEmpty() && ops.peek() != '('){
        float val2 = values.pop();
        float val1 = values.pop();
        char op = ops.pop();
        
        values.push(applyOp(val1, val2, op));
      }
      
      // pop opening brace.
      if(!ops.isEmpty())
        ops.pop();
    }
    
    // Current token is an operator.
    else {
      // While top of 'ops' has same or greater 
      // precedence to the current token, which
      // is an operator. Apply operator on top 
      // of 'ops' to top two elements in the values stack.
      while(!ops.isEmpty() && precedence(ops.peek()) >= precedence(tokens[i])){
        float val2 = values.pop();
        float val1 = values.pop();
        char op = ops.pop();
        
        values.push(applyOp(val1, val2, op));
      }
      
      // Push the current token to 'ops'.
      ops.push(tokens[i]);
    }
  }
  
  while(!ops.isEmpty()){
    float val2 = values.pop();
    float val1 = values.pop();
    char op = ops.pop();
    
    values.push(applyOp(val1, val2, op));
  }
  
  // Top of 'values' contains result, return it.
  //int ans = values.peek();
  //ans = (ans)*(24/5) - 12;
  //return ans;
  
  return values.peek();
}

void setup() {
  Serial.begin(9600); // Initialize the hardware serial 9600 baud

  // The address 9 is an arbitrary 7-bit value 
  // (from 0 to 127) that identifies this Arduino on the Inter-Integrated Circuit bus.
  Wire.begin(9); // Address for the Arduino Uno
  Wire.onReceive(receiveEvent); // Register the receive event

  pinMode(6, OUTPUT); // PWM pin for input 1
  pinMode(9, OUTPUT); // PWM pin for input 2

  // inputs to the arduino from the circuits

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
}

void loop() {

    if (receiveFlag) {
    Serial.print("Received String: ");
    Serial.println(tempBuffer);
    String receivedExpression = tempBuffer; // Print the temporary buffer
    
    float ans;
    ans = evaluate(receivedExpression);
    
    Serial.println(evaluate(receivedExpression));

    // Reset the buffer and flag for the next message
    memset(buffer, 0, sizeof(buffer));
    receiveFlag = false;
  }

  delay(100); // Add a delay to avoid reading too quickly
  
}

// I2C

// multi-master, multi-slave, serial bus
// connect multiple integrated circuits (ICs) or microcontrollers 
// SDA (Serial Data Line): Used for data transmission.
// SCL (Serial Clock Line): Used to synchronize the data transmission.
// Arduino is the slave with address 9