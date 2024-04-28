int storage[2][5][4]; //need to figure out how to wait to initialize this when we get size input...
float size_apart = 0.5; //temporary, will be input

struct nutrients{
  int x;
  int y;
  int m;
  int n;
  int p;
  int k;
};

nutrients store(){
  //simulate being received from sensor team
  //will no longer have () parameters, will be values, skip this chunk
  int x = random(3); //this is distance in meters, i made it 2 because max is /0.5 = 4
  int y = random(2); //this y is ALREADY the row number so no adjustments
  Serial.print(x);
  Serial.println(y);
  int m = random(1000);
  int n = random(1000);
  int p = random(1000);
  int k = random(1000);

  //fixing values for proper use
  int x_new = x / size_apart;
  //adjusment formulas for all nutrient to be understandable
  m = adaptMoisture(m);

  nutrients input = {x_new, y, m, n, p, k};
  return input;
}

void setup() {
  // put your setup code here, to run once:
	
	Serial.begin(9600);
  randomSeed(analogRead(0));
  //call a function to create the array
  //make random variables of x, y, num rows, x dist
  //actually all i need for the array is x length, y length, x dist

}

void loop() {
  // put your main code here, to run repeatedly:

  //if (data_ready = true)
  nutrients new_input = store();
  //because only store new values sent by data, need some indicator


  for (int i=0;i<2;i++){
    for(int j=0;j<5;j++){
      for(int k=0;k<4;k++){
        Serial.print(storage[i][j][k]);
        Serial.print(" ");
      }
      Serial.print("          ");
    }     
    Serial.println(" ");
  }
  Serial.println("--------------------------------------------------------------------------------------------");

  place_coord(new_input);

  delay(6000); //10 seconds between loops for my sanity

}






int adaptMoisture(int moisture){
  //some sort of function to adapt moisture to usable values
  int new_moisture = moisture + 1;
  return new_moisture;
}

void place_coord(nutrients input){
  int y = input.y;
  int x = input.x;
  storage[y][x][0] = input.m;
  storage[y][x][1] = input.n;
  storage[y][x][2] = input.p;
  storage[y][x][3] = input.k;
  return;
}

