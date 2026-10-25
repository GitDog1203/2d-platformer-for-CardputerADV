#include <M5Cardputer.h>

enum GameState {
  STATE_MENU,
  STATE_PLAYING,
  STATE_SPEEDRUN_WIN
};

GameState currentState = STATE_MENU;
int currentlevel = 1;
int maxUnlockedLevel = 1;
int deathCount = 0;
int unlockedLevels = 1;
const int MAX_LEVELS = 10;
bool isSpeedrun = false;
unsigned long speedrunStartTime = 0;
unsigned long speedrunFinalTime = 0;

struct Enemy {
  float x, y;
  float vx; 
  bool active;    
};

#define MAX_ENEMIES 10
Enemy enemies[MAX_ENEMIES];

float frogX = 50.0f;
float frogY = 50.0f;
float spawnX = 50.0f;
float spawnY = 20.0f;
float velocityY = 0.0f;
bool isGrounded = false;
float cameraX = 0.0f;

const int MAP_ROWS = 11;
const int MAP_COLS = 40;
const int TILE_SIZE = 12;

const char levelMaps[10][MAP_ROWS][MAP_COLS + 1] = {    
  // Level 1
  {
    "........................................",
    "........................................",
    "........................................",
    "........................................",
    "....................................P...",
    "...................................####.",
    "..........................###...........",
    "........................................",
    ".......##...........##..................",
    "..F....###...SS.....##.....E.#..........",
    "##############################.........."
  }, 
  // Level 2 logan
  {
    "........................................",
    "........................................",
    ".......................................P",
    ".F....................................##",
    ".###.............###.............###....",
    ".........###.............###............",
    "........................................",
    "........................................",
    "SSSS....SSSS....SSSS....SSSS....SSSS....",
    "####SSSS####SSSS####SSSS####SSSS####SSSS",
    "########################################"
  },
  // Level 3
  {
    "........................................",
    "........................................",
    "........................................",
    "...........................#..........P.",
    "..........................##..........P.",
    ".........................###..........P.",
    "........................####..........P.",
    ".......#...#...........#####..........P.",
    "......##...##.........######..........P.",
    ".F...###SSS###..EE...#######..........P.",
    "########################################"
  }, 
  // Level 4
  {
    "........................................",
    "........................................",
    "........................................",
    "........................................",
    "........................................",
    "........................................",
    "........................................",
    "........................................",
    "......#.E.#.E#.E#.E.#.E.#.E#.E.#........",
    ".F....#SSS#SS#SS#SSS#SSS#SS#SSS#...P....",
    "########################################"
  },
  // Level 5
  {
    ".........############################.##",
    "..F......#..............................",
    "#######..#..............................",
    ".........#..............................",
    ".........#.............##.....#....#....",
    ".......###..............#....##SSSS#....",
    "........................#....#######....",
    ".......................##....#..........",
    "........................#....#..........",
    "...####SSS##ESSSSE#.....#....#P......SS.",
    "#########################....###########"
  },
  // Level 6
  {
    "........................................",
    "........................................",
    "........................................",
    "........................................",
    "........................................",
    "........................................",
    "........................................",
    "........................................",
    ".F......................................",
    "###...E...E....###E..E..E...........P..",
    "###............###................######"
  },
  // Level 7
  {
    "..........#.............................",
    ".....###..#.............................",
    ".....#....#....#........................",
    "....#.....#....#.....SS..S..............",
    ".....#....#...#############.............",
    ".....#....#....#................###.....",
    "....#..........#P.......................",
    "....#.........##P.......................",
    "F...#..........#.....#####..............",
    "##SS###SSS##...#................###.....",
    "############...#........................"
  },
   { // level 8 CORBIN!
    "......#.................................",
    "......#.................................",
    "..F...#.................................",
    "#####.#.................................",
    "......#SSSS#................##..SS..##..",
    "......######...............###########..",
    ".......................#...S............",
    ".................##....##..#............",
    "......##...##....##........#PP.......#..",
    "SSSSSS##SSS##SSSSSSSSSSSSSS###########SS",
    "########################################"
   },
  { //lvl 9
    "#................###...............#SSSS",
    "#...###...#..#.................###.###..",
    "##....#..##..#.....#..##.##.###.........",
    "#.....#......#...#.###..S..S.....####...",
    "#........#######.#............#..#......",
    "##......S................###..#..#......",
    "....S.#.#######....###........#..#....SS",
    ".####.#..........#.........E..#..#...S..",
    ".#....##########.#.####.......#..#......",
    "F#...............#...............#P.....",
    "########################################"
  },
  // Level 10
  {
    ".######........#........................",
    "......#........#.....#..................",
    "..###.#P#.....##.....#..#...............",
    "..#...###............#..#..#............",
    ".##...#........##....#..#..#..#.........",
    "..#...#SSSS...###SSSS#........#..#......",
    "..#..S#############################.....",
    ".##...S#...................#.....##.#...",
    "..#..E.#.............#.....#..#...#.#...",
    "F.####....#E..E....###S.......##.......#",
    "###########........#####################"      
  }
};

uint32_t SKY = M5Cardputer.Display.color888(96, 156, 248);
uint32_t FROG_BODY = M5Cardputer.Display.color888(0, 255, 145);

M5Canvas canvas(&M5Cardputer.Display);

void drawFrog(int x, int y) {
  canvas.fillRect(x, y + 2, 12, 10, FROG_BODY);
  canvas.fillRect(x + 2, y, 3, 3, YELLOW); 
  canvas.fillRect(x + 7, y, 3, 3, YELLOW);  
}

void spawnEnemiesFromMap(int levelIndex) {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    enemies[i].active = false;
  }

  int count = 0;
  for (int row = 0; row < MAP_ROWS; row++) {
    for (int col = 0; col < MAP_COLS; col++) {
      if (levelMaps[levelIndex][row][col] == 'E') {
        if (count < MAX_ENEMIES) {
          enemies[count].x = col * TILE_SIZE;
          enemies[count].y = row * TILE_SIZE;
          enemies[count].vx = 0.5f;
          enemies[count].active = true;
          count++;
        }
      }
    }
  }
}

void updateAndDrawEnemies() {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (!enemies[i].active) continue;

    enemies[i].x += enemies[i].vx;

    int checkCol = (enemies[i].vx > 0) ? (enemies[i].x + 12) / TILE_SIZE : enemies[i].x / TILE_SIZE;
    int checkRow = enemies[i].y / TILE_SIZE;

    if (checkCol >= 0 && checkCol < MAP_COLS && checkRow >= 0 && checkRow < MAP_ROWS) {
      if (levelMaps[currentlevel - 1][checkRow][checkCol] == '#') {
        enemies[i].vx *= -1; 
      }
    }

    int screenX = (int)(enemies[i].x - cameraX);
    int screenY = (int)enemies[i].y;

    if (screenX >= -12 && screenX <= 240) {
      canvas.fillRoundRect(screenX + 1, screenY, 10, 8, 2, RED);    
      canvas.fillRect(screenX + 2, screenY + 8, 8, 4, DARKGREY);   
      canvas.fillRect(screenX + 3, screenY + 2, 2, 3, WHITE);      
      canvas.fillRect(screenX + 7, screenY + 2, 2, 3, WHITE);      
    }

    if (frogX < enemies[i].x + 10 && frogX + 12 > enemies[i].x &&
        frogY < enemies[i].y + 12 && frogY + 12 > enemies[i].y) {
      
      if (velocityY > 0 && (frogY + 12 - velocityY) <= enemies[i].y + 4) {
        enemies[i].active = false; 
        velocityY = -3.5f;         
      } else {
        deathCount++;
        frogX = spawnX;
        frogY = spawnY;
        velocityY = 0;
        cameraX = spawnX - 114;
        if (cameraX < 0) cameraX = 0;
        spawnEnemiesFromMap(currentlevel - 1);
      }
    }
  }
}

void setup() {
  auto cfg = M5.config();       
  M5Cardputer.begin(cfg, true);
  M5Cardputer.Display.setRotation(1); 
  canvas.createSprite(240, 135);
}

void loop() {
  M5Cardputer.update();
  Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

  canvas.fillScreen(SKY);

  switch (currentState) {
    case STATE_MENU: {
      canvas.setTextSize(2);
      canvas.setTextColor(WHITE);
      canvas.drawString("Fredrick's Feat", 30, 12);
      canvas.setTextColor(YELLOW);
      canvas.drawString("< Level:" + String(currentlevel) + " >", 54, 50); 
      canvas.setTextSize(1);
      canvas.setTextColor(CYAN);
      canvas.drawString("[ENTER]: Play Level", 65, 95);
      canvas.setTextSize(1);
      canvas.setTextColor(DARKGREY);
      canvas.drawString("W: Jump | </>: Move | Esc: Menu", 15, 118);

      if (M5Cardputer.Keyboard.isKeyPressed('s')) {
        isSpeedrun = true;
        currentlevel = 1;
        spawnEnemiesFromMap(currentlevel - 1);
        frogX = spawnX;
        frogY = spawnY;
        velocityY = 0;
        deathCount = 0;
        speedrunStartTime = millis();
        currentState = STATE_PLAYING;
        delay(200);
      }

      if (M5Cardputer.Keyboard.isKeyPressed(',') || M5Cardputer.Keyboard.isKeyPressed(';')) {
        if (currentlevel > 1) currentlevel--;
        delay(150);
      }
      if (M5Cardputer.Keyboard.isKeyPressed('.') || M5Cardputer.Keyboard.isKeyPressed('/')) {
        if (currentlevel < MAX_LEVELS) {
           currentlevel++;
        }
        delay(150);
      }
      if (status.enter) {
        int activeMapIndex = currentlevel - 1;
        spawnEnemiesFromMap(activeMapIndex);
        bool foundStart = false;
        for(int row = 0; row < MAP_ROWS; row++) {
          for(int col = 0; col < MAP_COLS; col++){
            if (levelMaps[activeMapIndex][row][col] == 'F') {
              frogX = col * TILE_SIZE;
              frogY = (row * TILE_SIZE) - 12;
              foundStart = true;
              break;
            }
          }
          if (foundStart) break;
        }
        if (!foundStart) {
          frogX = 30;
          frogY = 20;
        }
        spawnX = frogX;
        spawnY = frogY;
        velocityY = 0;
        cameraX = frogX - 114;
        if (cameraX < 0) cameraX = 0;
        isGrounded = false;
        currentState = STATE_PLAYING;
        delay(200);
      }
      break;
    }

    case STATE_PLAYING: {
      canvas.setTextColor(WHITE);
      canvas.drawString("Level:" + String(currentlevel) + "| Deaths:" + String(deathCount), 10, 10);
      
      if (isSpeedrun) {
        unsigned long currentTime = (millis() - speedrunStartTime) / 1000;
        canvas.setTextColor(YELLOW);
        canvas.setCursor(160, 25);
        canvas.printf("Time: %lus", currentTime);
      }

      float moveX = 0.0f;
      if (M5Cardputer.Keyboard.isKeyPressed(',')) {
        moveX = -2.5f;
      }
      if (M5Cardputer.Keyboard.isKeyPressed('/')) {
        moveX = 2.5f;
      }

      frogX += moveX;

      int activeMapIndex = currentlevel - 1;
      for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
          char tile = levelMaps[activeMapIndex][row][col];

          if (tile == '#') {
            int tileX = col * TILE_SIZE;
            int tileY = row * TILE_SIZE;
      
            if (frogY + 12 > tileY && frogY < tileY + TILE_SIZE) {
              if (moveX > 0 && frogX + 12 > tileX && frogX < tileX + 6) {
                frogX = tileX - 12;
              }
              else if (moveX < 0 && frogX < tileX + TILE_SIZE && frogX + 12 > tileX + TILE_SIZE - 6) {
                frogX = tileX + TILE_SIZE;
              }
            }
          }
        }
      }
      
      int totalLevelWidth = MAP_COLS * TILE_SIZE;
      if (frogX < 0) frogX = 0;
      if (frogX > totalLevelWidth - 12) frogX = totalLevelWidth - 12;

      cameraX = frogX - 114;
      if (cameraX < 0) {
        cameraX = 0;
      }
      if (cameraX > totalLevelWidth - 240) {
        cameraX = totalLevelWidth - 240;
      }

      if (M5Cardputer.Keyboard.isKeyPressed('w') && isGrounded) {
        velocityY = -5.5f;
        isGrounded = false;
      }

      velocityY += 0.35f;
      if (velocityY > 6.0f ) {
        velocityY = 6.0f;
      } 
      frogY += velocityY; 
      isGrounded = false;
      
      if (frogY > 135) {
        deathCount++;
        frogX = spawnX;
        frogY = spawnY;
        velocityY = 0;
        cameraX = spawnX - 114;
        if (cameraX < 0) cameraX = 0;
        spawnEnemiesFromMap(currentlevel - 1);
      }

      activeMapIndex = currentlevel - 1;
      for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
          char tile = levelMaps[activeMapIndex][row][col];

         if (tile == '#'){
            int tileX = col * TILE_SIZE;
            int tileY = row * TILE_SIZE;
            if (frogX + 12 > tileX && frogX < tileX + TILE_SIZE) {
              if (frogY + 12 >= tileY && frogY + 12 <= tileY + 6 && velocityY > 0) {
                frogY = tileY - 12;
                velocityY = 0;
                isGrounded = true;
              }
              else if (frogY <= tileY + TILE_SIZE && frogY >= tileY + TILE_SIZE - 6 && velocityY < 0) {
                frogY = tileY + TILE_SIZE; 
                velocityY = 0;             
              }
            }
          }
          else if (tile == 'S') {
            int tileX = col * TILE_SIZE;
            int tileY = row * TILE_SIZE;
            if (frogX + 12 > tileX && frogX < tileX + TILE_SIZE &&
                frogY + 12 > tileY && frogY < tileY + TILE_SIZE){
                  deathCount++;
                  frogX = spawnX;
                  frogY = spawnY;
                  velocityY = 0;
                  cameraX = spawnX - 114;
                  if (cameraX < 0) cameraX = 0;
                  spawnEnemiesFromMap(currentlevel - 1);
                }
          }
          else if (tile == 'P') {
            int tileX = col * TILE_SIZE;
            int tileY = row * TILE_SIZE;
            if (frogX + 12 > tileX && frogX < tileX + TILE_SIZE && 
                frogY + 12 > tileY && frogY < tileY + TILE_SIZE) {
                
                if (isSpeedrun) {
                  if (currentlevel < MAX_LEVELS) {
                    currentlevel++;
                    activeMapIndex = currentlevel - 1;
                    spawnEnemiesFromMap(activeMapIndex);
                    
                    bool foundStart = false;
                    for(int r = 0; r < MAP_ROWS; r++) {
                      for(int c = 0; c < MAP_COLS; c++){
                        if (levelMaps[activeMapIndex][r][c] == 'F') {
                          frogX = c * TILE_SIZE;
                          frogY = (r * TILE_SIZE) - 12;
                          foundStart = true;
                          break;
                        }
                      }
                      if (foundStart) break;
                    }
                    if (!foundStart) {
                      frogX = 30;
                      frogY = 20;
                    }
                    spawnX = frogX;
                    spawnY = frogY;
                    velocityY = 0;
                    cameraX = spawnX - 114;
                    if (cameraX < 0) cameraX = 0;

                  } else {
                    speedrunFinalTime = (millis() - speedrunStartTime) / 1000;
                    isSpeedrun = false;
                    currentState = STATE_SPEEDRUN_WIN;
                  }
                } else {
                  currentlevel++;
                  if (currentlevel > MAX_LEVELS) {
                    currentlevel = 1;
                  }
                  currentState = STATE_MENU;
                  delay(300);
                }
            }
          }
        }
      }

      for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
          char tile = levelMaps[activeMapIndex][row][col];
          int screenX = (col * TILE_SIZE) - (int)cameraX;
          int screenY = row * TILE_SIZE;
          if (screenX >= -TILE_SIZE && screenX <= 240) {
            if (tile == '#') {
              int tileHeight = (row == MAP_ROWS - 1) ? 15 : TILE_SIZE;
              canvas.fillRect(screenX, screenY, TILE_SIZE, tileHeight, DARKGREY);
            } else if (tile == 'F') {
              canvas.fillRect(screenX, screenY, TILE_SIZE, TILE_SIZE, ORANGE);
            } 
            else if (tile == 'S'){
              int spikeW = 4;
              int spikeH = 6;
              for (int i = 0; i < 3; i ++) {
                int x1 = screenX + (i * spikeW);
                int y1 = screenY + TILE_SIZE;
                int x2 = screenX + (i * spikeW) + (spikeW / 2);
                int y2 = screenY + TILE_SIZE - spikeH;
                int x3 = screenX + ((i+1) * spikeW);
                int y3 = screenY + TILE_SIZE;
                
                canvas.fillTriangle(x1, y1, x2, y2, x3, y3, RED);
              }
            } else if (tile == 'P') {
              canvas.fillRect(screenX, screenY, TILE_SIZE, TILE_SIZE, CYAN);
            }
          }
        }
      }
      
      updateAndDrawEnemies(); 
      drawFrog((int)(frogX - cameraX), (int)frogY);

      canvas.setTextColor(WHITE);
      canvas.drawString("Esc: Menu", 185, 10);

      if (M5Cardputer.Keyboard.isKeyPressed('`')) {
        isSpeedrun = false;
        currentState = STATE_MENU;
        delay(200);
      }
      break;
    }

    case STATE_SPEEDRUN_WIN: {
      canvas.fillScreen(BLACK);
      canvas.setTextColor(GREEN);
      canvas.setTextSize(2);
      canvas.drawString("SPEEDRUN CLEAR!", 20, 30);
      
      canvas.setTextColor(WHITE);
      canvas.setTextSize(1);
      canvas.setCursor(50, 70);
      canvas.printf("Final Time: %lu seconds", speedrunFinalTime);
      
      canvas.setCursor(40, 100);
      canvas.drawString("Press ENTER for Menu", 40, 100);

      if (status.enter) {
        currentlevel = 1; // <--- Reset level back to 1 here!
        currentState = STATE_MENU;
        delay(200);
      }
      break;
    }
  }

  canvas.pushSprite(0, 0);
  delay(30);
}