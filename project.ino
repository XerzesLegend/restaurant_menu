/*
   Program File Name: EE112_VIRTUAL_MENU.ino

   Group  : Emilio Tamez & Yousif Yosif
   Date   : 4/16/2021 - 5/5/2021

   Purpose: THE VIRTUAL MENU. Take in orders from multiple customers
            and compute the time and cost of that order. Orders will
            be 'made'/sorted by the least amount of time needed to
            make that order. Calculations will be based on the items
            size, quantity, and cost. Nearly all inputs are entered
            on the EDU shield; 5 inputs are from the serial monitor:
            customers name, and what to do after an item is selected
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
void  instructions();                                 // [1]  display instructions of how to use program
void  binary();                                       // [2]  display a binary guide
int   menu();                                         // [3]  read input combinations (binary inputs from push buttons)
void  item_name(int order[], int i, char item_size);  // [4]  display item name and it's size
int   ordertime(int order[], int i);                  // [5]  get time it takes to prepare individual item
int   orderprice(int order[], int i, char item_size); // [6]  get cost of individual item
char  item_sizes();                                   // [7]  calculate item size using the potentiometer, returns size cost
void  get_imin();                                     // [8]  get i min and min values to then sort orders later
void  sort_order();                                   // [9]  sort orders from least amount of time it takes to prepare them
void  song();                                         // [10] play song when program is done running : Code credit to 'AnonymousAlly'...
                                                      //      ... https://github.com/AnonymousAlly/Arduino-Music-Codes/blob/master/Bloody_Stream_JojoBA.ino
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//GLOBAL VARIABLES
#define SPEAKER  5          //speaker

#define POTENT   1          //potentiometer
#define LOW_VAL  0          //low mapped value for potentiometer
#define HIGH_VAL 5          //high mapped value for potentiometer

#define sw1 digitalRead(12) //puch button #0
#define sw2 digitalRead(8)  //puch button #1
#define sw3 digitalRead(7)  //puch button #2
#define sw4 digitalRead(4)  //puch button #3

#define MAX 10              //max value of items per order
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  delay(1000);

  //DECLARE VARIABLES
  int    item_number[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  int    order[MAX];          //number of items in order
  int    i;                   //counter
  int    order_size = 0;      //amount of items in an order
  float  order_time[i];       //list of time preperation needed to make an item
  char   cond = 'n';          //condition variable, set to 'y' to enter loop
  int    totalprice = 0;      //total cost of an order
  float  tax = 0.05125;       //sales tax percentage (in New Mexico)
  float  taxed_total = 0;     //total price with tax
  int    totaltime = 0;       //total amount of time it takes to make an order
  char   sizes[MAX];          //all different sizes per item in an order
  int    customer = 0;        //amount of order numbers
  int    add = 0;             //counter for number of customers
  int    customers[MAX];      //array to hold the amount of cutomers index
  int    ending = 0;          //control variable to stop program
  int    order_number[MAX];   //individual order value that stores an orders time
  String names[MAX];          //array to store customers names
  String cust_name;           //individual customer names

  pinMode(POTENT, INPUT);     //declaring potentiometer
  pinMode(12, INPUT_PULLUP);  //declaring push-button #0
  pinMode(8,  INPUT_PULLUP);  //declaring push-button #1
  pinMode(7,  INPUT_PULLUP);  //declaring push-button #2
  pinMode(4,  INPUT_PULLUP);  //declaring push-button #3
  ///////////////////////////////////////////////////////////////////////////////

  //DISPLAY INSTRUCTIONS
  instructions();

  //DISPLAY MENU
  Serial.println("MENU:\n 1) Classic Burger  $5\t  6) Dynamite Fries $3\t  11) Slushie   $2\n 2) Cheese Burger   $5\t  7) Nachos         $1\t  12) Coffee    $2\n 3) Mushroom Burger $6\t  8) Onion Rings    $2\t  13) Beer      $3\n 4) Sausage Burger  $4\t  9) Soda Drink     $1\t  14) Apple Pie $4\n 5) Normal Fries    $1\t 10) Milkshake      $3\t  15) Muffin    $4");

  //MAIN PROGRAM
  while (ending != 1) {/////////////////////////////////////RUN WHILE END IS FLASE


    while (cond == 'n') {///////////////////////////////////NEW ORDER WHILE CONDITION IS 'NEW'
      customer++; add++;                                  //number of customers (set at 1) | counter (set at 0)
      customers[add] = customer;                     //store number of customers in array

      //ASK FOR USER NAME
      Serial.print("\nPlease enter your name> ");         //context
      while (Serial.available() == 0);                    //wait
      cust_name = Serial.readString();                    //store
      Serial.println(cust_name);                          //echo
      Serial.println(String("Welcome ") + cust_name + String("! What would You like to order?"));
      names[customer] = cust_name;                        //store all customer names in array

      cond = 'y';                                         //change condition to 'y' to enter loop

      while (cond == 'y') {/////////////////////////////////NEW ITEM ADDED WHILE CONDITION IS 'YES'

        //ADD ITEMS TO AN ORDER
        Serial.print("\n  Item number >");                //context
        order[order_size] = menu();                       //get item inputs
        Serial.println(order[order_size]);                //display item number
        sizes[order_size] = item_sizes();                 //select item size
        item_name(order, order_size, sizes[order_size]);  //display item name and item size

        //ASK USER WHAT TO DO [CONTINUE ADDING ITEMS | NEW ORDER | END]
        while (1) {
          delay(500);
          Serial.print("\nTo add another item type (y) | To make a new order type (n) | To end program type (e) >");
          while (Serial.available() == 0);                //wait
          cond = Serial.read();                           //store
          Serial.println(cond);                           //echo

          if ((cond == 'y') || (cond == 'n')) {           //if condition is 'yes' or 'new'...
            ending = 0;                                   //...program still runs
            break;
          }

          else if (cond == 'e') {                         //if condition is 'end'...
            ending = 1;                                   //...make END TRUE to end program
            break;
          }
        }
        order_size++;                                     //increade order size everytime an item is added
      }//y

      //RESET VARIABLES FOR NEW ORDERS
      totaltime = 0;                                      //reset variables for every new order
      totalprice = 0;                                     //reset variables for every new order

      //ADD ITEMS TO GET COST AND PRICE OF AN INDIVIDUAL ORDER
      for (i = 0; i < order_size ; i++) {
        totalprice += orderprice(order, i, sizes[i]);     //comupte cost of order by summation of items costs
        totaltime += ordertime(order, i);                 //compute time of order by summation of items times
      }

      //STORE TOTAL TIMES INTO AN ARRAY
      order_number[customer] = totaltime;  //total time is given to the respective customer
      //customer++;

      //DISPLAY COSTS AND PRICE
      Serial.print("  Base Cost : "); Serial.print(float(totalprice)); Serial.println('$');
      taxed_total = (tax * totalprice) + totalprice;
      Serial.print("  Taxed Cost: "); Serial.print(taxed_total); Serial.println('$');
      Serial.print("  Total time: "); Serial.print(totaltime); Serial.println(" mins");
      Serial.print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
      delay(700);

      order_size = 0;
    }//n
  }//e

  //SORT ORDERS FROM LEAST AMOUNT OF TIME IT TAKES TO PREPARE THEM
  Serial.print("\nFor time efficiency, orders will be made in this sequence. (Customers Name, Time to Prepare)\n");
  sort_order(customer, order_number, names);

  //PLAY A SONG WHEN THE PROGRAM IS DONE RUNNING
  Serial.print("\n! THANK YOU FOR ORDERING !");
  delay(2000);
  song();
}
void loop() {}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void instructions() {
  char bin;

  Serial.println(F("WELCOME TO THE VIRTUAL MENU !\n\n-To order an item, look at the item number and enter the item number with the\n push-buttons in a binary configuration (press and hold until the green LED\n lights up)\n\nex: item #5 would be 0101 (reading from right to left), where 1 = pushed button\n    value 1 : Push-button #0 --> 1 => 1\n    value 2 : Push-button #1 --> 0 => 0\n    value 4 : Push-button #2 --> 1 => 4\n    value 8 : Push-button #3 --> 0 => 0\n\t\t\t        =  5\n\n-To see a binary input guide type the charcter 'B', else type any character>\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"));

  while (Serial.available() == 0);  //wait
  bin = Serial.read();              //store

  //DISPLAY BINARY GUIDE
  if (bin == 'B' || bin == 'b') {   //if charcter entered is 'B' or 'b' then...
    delay(1000);
    binary();                       //display binary table
  }
  delay(500);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void binary() {
  Serial.println(F("BINARY TABLE\n\n  READ FROM RIGHT TO LEFT\n  Button # : 3  2  1  0\n             ^  ^  ^  ^\n  No.Value : 8  4  2  1\n\n[01] 0001\t[05] 0101\t[09] 1001\t [13] 1101\n[02] 0010\t[06] 0110\t[10] 1010\t [14] 1110\n[03] 0011\t[07] 0111\t[11] 1011\t [15] 1111\n[04] 0100\t[08] 1000\t[12] 1100\t\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"));
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void item_name(int order[], int i, char item_size) {
  String names[] = {String("Classic Burger"), String("Cheeseburger"), String("Mushroom Burger"), String("Sausage Burger"), String("Normal Fries"), String("Dynamite Fries")
                    , String("Nachos"), String("Onion Rings"), String("Soda Drink"), String("Milkshake"), String("Slushie"), String("Coffee"), String("Beer"), String("Applie Pie"), String("Muffin")
                   };
  String str = names[order[i] - 1];
  Serial.print("  "); Serial.print(str); Serial.print(" ("); Serial.print(item_size); Serial.print(')'); Serial.println("! has been added to your order!");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ordertime(int order[], int i) {
  int times[] = {6, 6, 6, 5, 2, 4, 2, 3, 1, 3, 2, 2, 2, 1, 1};

  return (times[order[i] - 1]);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int orderprice(int order[], int i, char item_size) {
  int price;
  int prices[] = {5, 5, 6, 4, 1, 3, 1, 2, 1, 3, 2, 2, 3, 4, 4};

  price = prices[order[i] - 1];

  if (item_size == 'L')
    price += 4;

  else if (item_size == 'M')
    price += 2;

  return (price);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sort_order(int n, int arr[], String str[]) {
  int imin;
  int len;
  int temp;
  int i;
  String temp2;

  for (len = n; len >= 2 ; len--) {
    imin = get_imin(arr, len);

    if (imin != (len - 1)) {
      temp = arr[imin];
      arr[imin] = arr[len - 1];
      arr[len - 1] = temp;
    }
    if (imin != (len - 1)) {
      temp2 = str[imin];
      str[imin] = str[len - 1];
      str[len - 1] = temp2;
    }
    for (i = 0 ; i < n ; i++) {            //for loop conditions
      Serial.print("  ");
      Serial.print(str[i]);                //display name
      Serial.print(" : ");
      Serial.print(arr[i]);                //display time it takes to prepare
      Serial.println(" minutes");
    }
  }
}
// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
int get_imin(int arr[], int n) {
  int imin;
  int Min;
  int i;

  for (i = 0 ; i < n ; i++) {
    if (i == 0 || arr[i] > Min) {
      Min = arr[i];
      imin = i;
    }
  }
  return (imin);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int menu() {
  int answer;
  int bin = 0;
  while (analogRead(1) == 1023) {
    if (sw1 == 0) {
      bin += 1;
    }
    if (sw2 == 0) {
      bin += 2;
    }
    if (sw3 == 0) {
      bin += 4;
    }
    if (sw4 == 0) {
      bin += 8;
    }
    delay(1000);
  }

  return bin;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char item_sizes() {
  char  item_size;      //inital of item size selected [S/M/L]
  int   pot;            //potentiometer reading
  float val;            //potentiometer value mapped from the range of [0-5]
  int   cont = 1;       //continue, set at 1 to enter while loop
  /**********************************************************************************************************************/
  delay(1000);

  //TAKE ITEM SIZE WHILE LOOP IS INITIATED
  while (cont == 1) {

    Serial.println("\nTo select an item size [S/M/L], set the potentiometer to the size you want, then\npress push-button #0 (Rotated all the way to the left is SMALL, middle is MEDIUM,\nand right is LARGE)\n");

    //CHECK BUTTONS CONDITION
    while (sw1 == HIGH) {}           //while push-button #0 is not pressed, do nothing


    //DETERMINE ITEM SIZE                         //when push-button #0 is pressed...
    pot = analogRead(POTENT);                     //take reading from potentiometer
    val = map(pot, 0, 1023, LOW_VAL, HIGH_VAL);   //map porentiometer values from [0-1023] to [0-5]
    /*****************************************************************************************************/
    if (val < 1.67) {                             //if read potentiometer value is less than 1.67...
      item_size = 'L';                            //...then item size is Large
      break;
    }
    /*****************************************************************************************************/
    else if (val >= 1.67 && val <= 3.33) {        //if read potentiometer value is between 1.67 and 3.33...
      item_size = 'M';                            //...then item size is Medium
      break;
    }
    /*****************************************************************************************************/
    else {                                        //else...
      item_size = 'S';                            //...item size is Small
      break;
    }
    /*****************************************************************************************************/

  }
  return (item_size);                             //return final price of item size
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void song() {
  pinMode(SPEAKER, OUTPUT);

  tone(SPEAKER, 261, 114);  delay(114);  //C4(middle)
  tone(SPEAKER, 311, 114);  delay(114);  //Eb/D#4
  tone(SPEAKER, 392, 227);  delay(227);  //G4
  tone(SPEAKER, 370, 341);  delay(341);  //Gb/F#4
  tone(SPEAKER, 349, 227);  delay(227);  //F4
  tone(SPEAKER, 349, 227);  delay(227);  //F4
  tone(SPEAKER, 261, 114);  delay(114);  //C4(middle)
  tone(SPEAKER, 311, 227);  delay(227);  //Eb/D#4
  tone(SPEAKER, 349, 114);  delay(227);  //F4
  tone(SPEAKER, 261, 114);  delay(114);  //C4(middle)
  tone(SPEAKER, 311, 114);  delay(114);  //Eb/D#4
  tone(SPEAKER, 392, 227);  delay(227);  //G4
  tone(SPEAKER, 370, 341);  delay(341);  //Gb/F#4
  tone(SPEAKER, 349, 227);  delay(227);  //F4
  tone(SPEAKER, 349, 227);  delay(227);  //F4
  tone(SPEAKER, 261, 114);  delay(114);  //C4(middle)
  tone(SPEAKER, 311, 227);  delay(227);  //Eb/D#4
  tone(SPEAKER, 349, 114);  delay(227);  //F4
  tone(SPEAKER, 261, 114);  delay(114);  //C4(middle)
  tone(SPEAKER, 311, 114);  delay(114);  //Eb/D#4
  tone(SPEAKER, 392, 227);  delay(227);  //G4
  tone(SPEAKER, 370, 341);  delay(341);  //Gb/F#4
  tone(SPEAKER, 349, 227);  delay(227);  //F4
  tone(SPEAKER, 349, 227);  delay(227);  //F4
  tone(SPEAKER, 261, 114);  delay(114);  //C4(middle)
  tone(SPEAKER, 311, 227);  delay(227);  //Eb/D#4
  tone(SPEAKER, 466, 910);               //Bb/A#4
}
