#include <LiquidCrystal.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

float accelXOffset = 0, accelYOffset = 0, accelZOffset = 0;
float gyroXOffset = 0, gyroYOffset = 0, gyroZOffset = 0;

const float accelThreshold = 1.0;

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int size = 5;
int pos_x = 0;
int pos_y = 0;
bool win = false;
char dir;

int** grid = (int** )malloc(size*sizeof(int* ));

String getDirection(float accelX, float accelY) {
  if (accelY < -accelThreshold) {
    return "up";
  } else if (accelY > accelThreshold) {
    return "down";
  } else if (accelX < -accelThreshold) {
    return "left";
  } else if (accelX > accelThreshold) {
    return "right";
  }
  return "stationary";
}

void calibrateMPU6050() {
  const int numSamples = 2000; // Number of samples for averaging

  float accelXSum = 0, accelYSum = 0, accelZSum = 0;
  float gyroXSum = 0, gyroYSum = 0, gyroZSum = 0;

  for (int i = 0; i < numSamples; i++) {
    sensors_event_t accel, gyro, temp;
    mpu.getEvent(&accel, &gyro, &temp);

    accelXSum += accel.acceleration.x;
    accelYSum += accel.acceleration.y;
    accelZSum += accel.acceleration.z;

    gyroXSum += gyro.gyro.x;
    gyroYSum += gyro.gyro.y;
    gyroZSum += gyro.gyro.z;

    delay(10); // Short delay between samples
  }

  // Calculate average offsets
  accelXOffset = accelXSum / numSamples;
  accelYOffset = accelYSum / numSamples;
  accelZOffset = accelZSum / numSamples - 1.0; // Adjust for gravity on Z-axis

  gyroXOffset = gyroXSum / numSamples;
  gyroYOffset = gyroYSum / numSamples;
  gyroZOffset = gyroZSum / numSamples;
}


void move_up(int* pos_x, int* pos_y, int** grid, int size){
  if(*pos_x==0){
    lcd.clear();
    lcd.print("INVALID!");
    return;
  }
  if(grid[*pos_x-1][*pos_y]!=1){
    lcd.clear();
    lcd.print("MOVED UP!");
    grid[*pos_x][*pos_y] = 0;
    *pos_x -= 1;
    grid[*pos_x][*pos_y] = 2;
    return;
  }
  else{
    lcd.clear();
    lcd.print("INVALID!");
    return;
  }
}

void move_down(int* pos_x, int* pos_y, int** grid, int size){
  if(*pos_x+1==size){
    lcd.clear();
    lcd.print("INVALID!");
    return;
  }
  if(grid[*pos_x+1][*pos_y]!=1){
    lcd.clear();
    lcd.print("MOVED DOWN!");
    grid[*pos_x][*pos_y] = 0;
    *pos_x += 1;
    grid[*pos_x][*pos_y] = 2;
    return;
  }
  else{
    lcd.clear();
    lcd.print("INVALID!");
    return;
  }
}

void move_left(int* pos_x, int* pos_y, int** grid, int size){
  if(*pos_y-1==-1){
    lcd.clear();
    lcd.print("INVALID!");
    return;
  }
  if(grid[*pos_x][*pos_y-1]!=1){
    lcd.clear();
    lcd.print("MOVED LEFT!");
    grid[*pos_x][*pos_y] = 0;
    *pos_y -= 1;
    grid[*pos_x][*pos_y] = 2;
    return;
  }
  else{
    lcd.clear();
    lcd.print("INVALID!");
    return;
  }
}

void move_right(int* pos_x, int* pos_y, int** grid, int size){
  if(*pos_y+1==size){
    lcd.clear();
    lcd.print("INVALID!");
    return;
  }
  if(grid[*pos_x][*pos_y+1]!=1){
    lcd.clear();
    lcd.print("MOVED RIGHT!");
    grid[*pos_x][*pos_y] = 0;
    *pos_y += 1;
    grid[*pos_x][*pos_y] = 2;
    return;
  }
  else{
    lcd.clear();
    lcd.print("INVALID!");
    return;
  }
}

void create_maze(int** grid, int size){
  int randomizer = 4;
  int* move = (int* ) malloc(((size-1)*2+4*randomizer)*sizeof(int));
  for (int i=0; i<size-1; ++i){
    move[i]=0;
  }
  for (int i=size-1; i<2*(size-1); ++i){
    move[i]=2;
  }
  for(int i=2*(size-1); i<((size-1)*2+4*randomizer); i=i+2){
    int to_add = random(0,2);
    if(to_add==1){
      move[i]=0;
      move[i+1]=1;
    }else{
      move[i]=2;
      move[i+1]=3;
    }
  }
  for (int i = 0; i < ((size-1)*2+4*randomizer); i++){
      int j = random(0, ((size-1)*2+4*randomizer) - i);

      int t = move[i];
      move[i] = move[j];
      move[j] = t;
  }

  int x = 0;
  int y = 0;
  int i=0;
  bool run = true;
  while(run){
    if(i==((size-1)*2+4*randomizer)){
      i=0;
    }
    if(move[i]==-1){
      i+=1;
      continue;
    }
    if(move[i]==0&&y!=size-1){
      y+=1;
      grid[x][y]=0;
      move[i]=-1;
    }else if(move[i]==1&&y!=0){
      y-=1;
      grid[x][y]=0;
      move[i]=-1;
    }else if(move[i]==2&&x!=size-1){
      x+=1;
      grid[x][y]=0;
      move[i]=-1;
    }else if(move[i]==3&&x!=0){
      x-=1;
      grid[x][y]=0;
      move[i]=-1;
    }
    ++i;
    run = false;
    for(int j=0; j<((size-1)*2+4*randomizer); ++j){
      if(move[j]!=-1){
        run=true;
        break;
      }
    }
  }
}

void print(int** grid, int size){
  for(int i=0; i<size; i++){
    Serial.print(" -");
  }
  Serial.println();
  for(int i=0; i<size; ++i){
    for(int j=0; j<size; ++j){
      Serial.print('|');
      if(grid[i][j]==0){
        Serial.print(' ');
      }
      if(grid[i][j]==1){
        Serial.print('#');
      }
      if(grid[i][j]==2){
        Serial.print('O');
      }
    }
    Serial.print('|');
    Serial.println();
  }
  
  for(int i=0; i<size; i++){
    Serial.print(" -");
  }
  Serial.println();
}

void new_maze(int** grid, int size, int* pos_x, int* pos_y){
  for(int i=0; i<size; i++){
    for(int j=0; j<size; j++){
      grid[i][j] = 1;
    }
  }

  print(grid, size);

  create_maze(grid, size);

  grid[*pos_x][*pos_y] = 2;


  print(grid, size);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  lcd.begin(16, 2);

  randomSeed(analogRead(0));

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);

  Serial.println("Place the MPU6050 on a flat surface and keep it still...");
  delay(5000);

  
  calibrateMPU6050();
  Serial.println("Calibration Complete!");

  Serial.print("Accel Offsets (g): X=");
  Serial.print(accelXOffset, 6);
  Serial.print(", Y=");
  Serial.print(accelYOffset, 6);
  Serial.print(", Z=");
  Serial.println(accelZOffset, 6);

  Serial.print("Gyro Offsets (°/s): X=");
  Serial.print(gyroXOffset, 6);
  Serial.print(", Y=");
  Serial.print(gyroYOffset, 6);
  Serial.print(", Z=");
  Serial.println(gyroZOffset, 6);

  Serial.println("Start moving the sensor to detect directions...");
  //dummy grid

  for(int i=0; i<size; i++){
    grid[i] = (int* ) malloc(size*sizeof(int));
  }

  new_maze(grid, size, &pos_x, &pos_y);

  
}

void loop() {
  // put your main code here, to run repeatedly:
  
  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  // Apply offsets to raw data
  float accelX = accel.acceleration.x - accelXOffset;
  float accelY = accel.acceleration.y - accelYOffset;
  float accelZ = accel.acceleration.z - accelZOffset;

  String direction = getDirection(accelX, accelY);

  if (direction != "Stationary") {
    if(direction=="left"){
      move_left(&pos_x, &pos_y, grid, size);
      print(grid, size);
    }

    if(direction=="right"){
      move_right(&pos_x, &pos_y, grid, size);
      print(grid, size);
    }

    if(direction=="up"){
      move_up(&pos_x, &pos_y, grid, size);
      print(grid, size);
    }

    if(direction=="down"){
      move_down(&pos_x, &pos_y, grid, size);
      print(grid, size);
    }

    
  }

  if(pos_x==size-1&&pos_y==size-1){
      lcd.clear();
      lcd.print("OMG VICTORY!");
      win = true;
    }

  delay(100);



  if(win){
    win=false;
    pos_x=0;
    pos_y=0;
    new_maze(grid, size, &pos_x, &pos_y);
  }  

}

