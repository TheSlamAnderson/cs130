#define STACK_SIZE 16

/*
  Create your Stack class here
*/

class Stack {
  public:
    Stack();
    void push(int p);
    int pop();
    int *sp();
    int *st();
  
  private:

    // A private integer array called stack that contains STACK_SIZE elements.
    int stack[STACK_SIZE];

    // A private integer pointer called stack_pointer.
    int *stack_pointer;

};


/*
  Add your Stack class' member functions here
*/

// A public constructor that sets the stack_pointer to the bottom of the stack.
Stack::Stack(){
  stack_pointer = &stack[STACK_SIZE];
}

// A public void function called push that takes one integer parameter, which is the value to be pushed on the stack.
// This function pushes the given value onto the stack.
void Stack::push(int p){
  stack_pointer--;
  *stack_pointer = p;
}

// A public function called pop that takes no parameters, but returns an integer.
// This function pops the value given by the stack pointer.
int Stack::pop(){
  stack_pointer++;
  return *(stack_pointer-1);
}

// A public function called sp that takes no parameters, but returns an integer pointer.
// This function returns the stack pointer variable.
int* Stack::sp(){
  return stack_pointer;
}

// A public function called st that takes no parameters, but returns an integer pointer.
// This function returns the TOP of the stack array.
int* Stack::st(){
  return &stack[0];
}


////////////////////////////////////////////////////////
//
// Do not modify anything below!
//
////////////////////////////////////////////////////////
#define BUTTON_1 A1
#define BUTTON_2 A2
#define BUTTON_3 A3

#define CLK_DIO 7
#define DATA_DIO 8
#define LATCH_DIO 4

const int SEGMENTS[] = {0b11110001, 0b11110010, 0b11110100, 0b11111000};
const int DIGITS[] = {0b11000000, 0b11111001, 0b10100100, 0b10110000, 0b10011001, 0b10010010, 0b10000010, 0b11111000, 0b10000000, 0b10011000,
                     /*a*/0b10001000,
                     /*b*/0b10000011,
                     /*c*/0b10100111,
                     /*d*/0b10100001,
                     /*e*/0b10000110,
                     /*f*/0b10001110,
                     /*-*/0b10111111,
};
bool BUTZ[] = {false, false, false};
bool disp_value = true;

Stack stack;
char buf[1024];

void SetSegment(int segment, int digit)
{
  digitalWrite(LATCH_DIO, LOW);

  shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, DIGITS[digit]);
  shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, SEGMENTS[segment]);
  
  digitalWrite(LATCH_DIO, HIGH);
  
}

void help()
{
  Serial.write("  push <value> - Push <value> onto the stack\n");
  Serial.write("  pop          - Pops a value off of the stack\n");
  Serial.write("  display      - Toggle 7-seg display from value or address mode\n");
  Serial.write("  dump         - Dump the entire stack (or press button 3)\n");
  Serial.write("  help         - This help\n");
}

void dump()
{
   sprintf(buf, "Dumping stack (asterisk = stack pointer):\n\n Address     Value\n");
   for (int i = 0;i < STACK_SIZE;i++) {
       sprintf(buf, "%s%c0x%08x  %d\n", buf, (&stack.st()[i] == stack.sp() ? '*' : ' '), &stack.st()[i], stack.st()[i]);
   }
   Serial.write(buf);
}


void setup() {
  // put your setup code here, to run once:
  pinMode(CLK_DIO, OUTPUT);
  pinMode(DATA_DIO, OUTPUT);
  pinMode(LATCH_DIO, OUTPUT);

  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_2, INPUT);
  pinMode(BUTTON_3, INPUT);

  SetSegment(0, 0);

  Serial.begin(115200);
  delay(1000);
  randomSeed(analogRead(0));
  help();
}


void loop() {
  bool but;
  String command, sval;
  int value;
  int values[4];
  int i;

  if (Serial.available() > 0) {
    command = Serial.readString();
    if (command.startsWith("push")) {
      if (stack.sp() == stack.st()) {
        sprintf(buf, "Push FAILED -- You're at the top of the stack!\n");
      }
      else {
        value = command.substring(5).toInt();
        stack.push(value);
        sprintf(buf, "Pushed %d, pointer is at 0x%08x\n", value, stack.sp());
      }
    }
    else if (command.startsWith("pop")) {
      if (stack.sp() >= stack.st() + STACK_SIZE) {
        sprintf(buf, "Pop FAILED -- There is nothing to pop off the stack!\n");
      }
      else {
        value = stack.pop();
        sprintf(buf, "Popped %d, pointer is at 0x%08x\n", value, stack.sp());
      }
    }
    else if (command.startsWith("display")) {
      disp_value = !disp_value;
      sprintf(buf, "Changing display to %s\n", disp_value ? "value" : "address");
    }
    else if (command.startsWith("dump")) {
      dump();
      buf[0] = '\0';
    }
    else if (command.startsWith("help")) {
      help();
      buf[0] = '\0';
    }
    else {
      sprintf(buf, "Unknown command '%s'\n", command.c_str());
      help();
    }
    Serial.write(buf);
  }

  but = digitalRead(BUTTON_1) ? false : true;
  if (BUTZ[0] != but) {
    //Button 1 pushes to stack
    if (but == true) {
      if (stack.sp() == stack.st()) {
        Serial.write("Button 1: push FAILED -- You're at the top of the stack!\n");
      }
      else {
        value = random(1, 9999);
        stack.push(value);
        sprintf(buf, "Button 1: pushed %d -- Current stack: Value = %d, SP = 0x%08x\n", value, *stack.sp(), stack.sp());
        Serial.write(buf);
      }
    }
    BUTZ[0] = but;
  }
  
  but = digitalRead(BUTTON_2) ? false : true;
  if (BUTZ[1] != but) {
    if (but == true) {
      if (stack.sp() >= stack.st() + STACK_SIZE) {
        Serial.write("Button 2: pop FAILED -- There is nothing to pop off the stack!\n");
      }
      else {
        value = stack.pop();
        sprintf(buf, "Button 2: popped %d -- Current stack: Value = %d, SP = 0x%08x\n", value, *stack.sp(), stack.sp());
        Serial.write(buf);
      }
    }
    BUTZ[1] = but;
  }
  
  but = digitalRead(BUTTON_3) ? false : true;
  if (BUTZ[2] != but) {
    if (but == true) {
      dump();
    }
    BUTZ[2] = but;
  }
  

  if (disp_value) {
    //Show the stack data
    if (stack.sp() >= stack.st() + STACK_SIZE) {
      for (i = 0;i < 4;i++)
        SetSegment(i, 16);
    }
    else {
      value = *stack.sp();
      values[0] = value / 1000;
      value -= values[0] * 1000;
      values[1] = value / 100;
      value -= values[1] * 100;
      values[2] = value / 10;
      value -= values[2] * 10;
      values[3] = value;
      for (i = 0;i < 4;i++) {
        SetSegment(i, values[i] % 10);
      }
    }
  } 
  else {
    value = (int)stack.sp() & 0xffff;
    SetSegment(0, value >> 12 & 0xf);
    SetSegment(1, value >> 8 & 0xf);
    SetSegment(2, value >> 4 & 0xf);
    SetSegment(3, value >> 0 & 0xf);
  }
  
}
