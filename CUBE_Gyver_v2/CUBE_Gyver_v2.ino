/*
   Светодиодный куб 8x8x8 на Arduino NANO и сдвиговых регистрах 74HC595
   Версия 2:
   - Добавлен режим игры "Змейка"
   - Добавлена бета версии игры "Танки"
   - Добавлена игра "Туннель"
   - Поправлена динамическая индикация
   - Добавлена инверсия текста

   Разработал VECT, модифицировал AlexGyver
   http://AlexGyver.ru/LEDcube/
   https://github.com/AlexGyver/GyverLibs
   2018 AlexGyver
*/

#define INVERT_Y 1    // инвертировать по вертикали (если дождь идёт вверх)
#define INVERT_X 0    // инвертировать по горизонтали (если текст не читается)
#define MIRROR_FONT 1 // зеркалить текст

#define POT_X 1       // джойстик Х
#define POT_Y 0       // джойстик У

//#define POT_X2 6       // джойстик Х2
//#define POT_Y2 7       // джойстик У2

#define BTN_UP A2     // отдельаня кнопка (вверх)
#define BTN_DWN A3    // кнопка джойстика

#define SNAKE_START 4 // начальная длина змейки

// текст для режима текста
String disp_text = "AlexGyver SHOW  SUBSCRIBE!";

#define XAXIS 0
#define YAXIS 1
#define ZAXIS 2

#define POS_X 0
#define NEG_X 1
#define POS_Z 2
#define NEG_Z 3
#define POS_Y 4
#define NEG_Y 5

#define BUTTON1 18
#define BUTTON2 19

#define RED_LED 5
#define GREEN_LED 7

#define TOTAL_EFFECTS 14  // количество эффектов

/*
   Эффекты по порядку:
   - Дождь
   - Плоскости
   - Лифты
   - Куб масштаб центр
   - Куб масштаб угол
   - Случайное заполнение
   - Бегущий текст
   - Ночник
   - Бегущая волна заполненная
   - Бегущая волна тонкая
   - Кубик рикошеты
   - Змейка
*/

//  некоторые названия режимов
#define RAIN 0
#define PLANE_BOING 1
#define SEND_VOXELS 2
#define WOOP_WOOP 3
#define CUBE_JUMP 4
#define GLOW 5
#define TEXT 6
#define LIT 7
#define SNAKE 11
#define SHOOTER 12
#define TUNNEL 13

// таймеры режимов
#define RAIN_TIME 260
#define PLANE_BOING_TIME 220
#define SEND_VOXELS_TIME 140
#define WOOP_WOOP_TIME 350
#define CUBE_JUMP_TIME 200
#define GLOW_TIME 8
#define TEXT_TIME 200
#define CLOCK_TIME 500
#define WALKING_TIME 100
#define SNAKE_TIME 700
#define SHOOTER_TIME 400
#define TUNNEL_TIME 300

#include <SPI.h>
#include "GyverButton.h"    // либа для кнопок
#include "GyverHacks.h"     // либа для быстрйо записи в порт
#include "GyverTimer.h"     // либа для таймеров
#include "fonts.h"          // шрифт

GButton butt1(BUTTON1);
GButton butt2(BUTTON2);
GButton butt_up(BTN_UP);
GButton butt_dwn(BTN_DWN);
GTimer_ms tankTimer(100);

// переменные
boolean showResult;
byte lastMode;
uint32_t millTimer;
uint8_t cube[8][8];
int8_t currentEffect;
uint16_t timer;
uint16_t modeTimer;
bool loading;
int8_t pos;
int8_t vector[3];
int16_t coord[3];
byte score;

void setup() {
  Serial.begin(9600);

  SPI.begin();
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  //pinMode(RED_LED, OUTPUT);
  //pinMode(GREEN_LED, OUTPUT);
  randomSeed(analogRead(0));
  //digitalWrite(GREEN_LED, HIGH);

  butt1.setStepTimeout(100);    // настрйока интервала инкремента (по умолчанию 800 мс)
  butt2.setStepTimeout(100);    // настрйока интервала инкремента (по умолчанию 800 мс)

  loading = true;
  currentEffect = TUNNEL;
  changeMode();
}

void loop() {
  butt1.tick();
  butt2.tick();

  if (butt1.isClick()) {
    currentEffect++;
    if (currentEffect >= TOTAL_EFFECTS) currentEffect = 0;
    changeMode();
  }
  if (butt2.isClick()) {
    currentEffect--;
    if (currentEffect < 0) currentEffect = TOTAL_EFFECTS - 1;
    changeMode();
  }

  if (butt1.isStep()) {                                 // если кнопка была удержана (это для инкремента)
    modeTimer += 10;
  }
  if (butt2.isStep()) {                                 // если кнопка была удержана (это для инкремента)
    modeTimer -= 10;
  }

  switch (currentEffect) {
    case RAIN: rain(); break;
    case PLANE_BOING: planeBoing(); break;
    case SEND_VOXELS: sendVoxels(); break;
    case WOOP_WOOP: woopWoop(); break;
    case CUBE_JUMP: cubeJump(); break;
    case GLOW: glow(); break;
    case TEXT:
      if (showResult) {
        String result = String(score);
        text(result, result.length());
      } else {
        text(disp_text, disp_text.length());
      }
      break;
    case LIT: lit(); break;
    case 8: sinusFill(); break;
    case 9: sinusThin(); break;
    case 10: walkingCube(); break;
    case 11: snake(); break;
    case 12: shooter(); break;
    case 13: tunnel(); break;
  }
  renderCube();
}

void changeMode() {
  clearCube();
  loading = true;
  timer = 0;
  randomSeed(millis());
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  delay(500);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);

  switch (currentEffect) {
    case RAIN: modeTimer = RAIN_TIME; break;
    case PLANE_BOING: modeTimer = PLANE_BOING_TIME; break;
    case SEND_VOXELS: modeTimer = SEND_VOXELS_TIME; break;
    case WOOP_WOOP: modeTimer = WOOP_WOOP_TIME; break;
    case CUBE_JUMP: modeTimer = CUBE_JUMP_TIME; break;
    case GLOW: modeTimer = GLOW_TIME; break;
    case TEXT: modeTimer = TEXT_TIME; break;
    case LIT: modeTimer = CLOCK_TIME; break;
    case 8: modeTimer = RAIN_TIME; break;
    case 9: modeTimer = RAIN_TIME; break;
    case 10: modeTimer = WALKING_TIME; break;
    case 11: modeTimer = SNAKE_TIME; break;
    case 12: modeTimer = SHOOTER_TIME; break;
    case 13: modeTimer = TUNNEL_TIME; break;
  }
}

void renderCube() {
  for (uint8_t i = 0; i < 8; i++) {
    setPin(SS, LOW);
    if (INVERT_Y) SPI.transfer(0x01 << (7 - i));
    else SPI.transfer(0x01 << i);

    for (uint8_t j = 0; j < 8; j++) {
      if (INVERT_X) SPI.transfer(cube[7 - i][j]);
      else SPI.transfer(cube[i][j]);
    }
    setPin(SS, HIGH);
    delayMicroseconds(50);
  }

  // вырубаем всё
  setPin(SS, LOW);
  SPI.transfer(0x00);
  setPin(SS, HIGH);
}

int8_t tank1[2], tank2[2];
boolean bullet1_f, bullet2_f;
int8_t bullet1[3], bullet2[3];
int8_t planePos;
byte planeType, prevType;
boolean planeFlag;

void tunnel() {
  if (loading) {
    clearCube();
    loading = false;
    tank1[0] = 0;
    tank1[1] = 0;
    score = 0;
    planePos = 7;
    planeFlag = false;
  }
  timer++;


  if (tankTimer.isReady()) {
    clearCube();
    if (timer > modeTimer) {
      timer = 0;

      planePos--;

      if (planePos == 1) {  // проверка на столкновение с кубом
        boolean gameOver = false;
        switch (planeType) {
          case 0: gameOver = checkPlane(0, 4, 1, 7);
            break;
          case 1: gameOver = checkPlane(3, 7, 1, 7);
            break;
          case 2: gameOver = checkPlane(2, 5, 1, 7);
            break;
          case 3: gameOver = checkPlane(0, 7, 0, 4);
            break;
          case 4: gameOver = checkPlane(0, 7, 3, 7);
            break;
          case 5: gameOver = checkPlane(0, 7, 2, 5);
            break;
          case 6:
            // проверка сложной фигуры, флаг gameOver не превысит 1
            gameOver += checkPlane(0, 1, 0, 7);
            gameOver += checkPlane(6, 7, 0, 7);
            gameOver += checkPlane(2, 5, 0, 2);
            gameOver += checkPlane(2, 5, 6, 7);
            break;
          case 7:
            // проверка сложной фигуры, флаг gameOver не превысит 1
            gameOver += checkPlane(0, 3, 0, 7);
            gameOver += checkPlane(3, 7, 0, 3);
            break;
          case 8:
            // проверка сложной фигуры, флаг gameOver не превысит 1
            gameOver += checkPlane(5, 7, 0, 7);
            gameOver += checkPlane(0, 4, 4, 7);
            break;
        }
        if (gameOver) {
          showResult = true;
          lastMode = TUNNEL;
          currentEffect = TEXT;
          changeMode();                             // всё по новой
          return;                                   // выходим из функции snake()
        } else {
          score++;
        }
      }

      if (planePos < 0) {
        planeFlag = false;
      }

      if (!planeFlag) {
        planeFlag = true;
        while (planeType == prevType) {   // генерируем, пока не будет отличаться от прежнего
          planeType = random(0, 9);
        }
        prevType = planeType;
        planePos = 7;
      }
    }

    if (planeFlag) {
      switch (planeType) {
        case 0: generatePlane(0, 4, 1, 7, planePos);
          break;
        case 1: generatePlane(3, 7, 1, 7, planePos);
          break;
        case 2: generatePlane(2, 5, 1, 7, planePos);
          break;
        case 3: generatePlane(0, 7, 0, 4, planePos);
          break;
        case 4: generatePlane(0, 7, 3, 7, planePos);
          break;
        case 5: generatePlane(0, 7, 2, 5, planePos);
          break;
        case 6: generatePlane(0, 1, 0, 7, planePos);
          generatePlane(6, 7, 0, 7, planePos);
          generatePlane(2, 5, 0, 2, planePos);
          generatePlane(2, 5, 6, 7, planePos);
          break;
        case 7: generatePlane(0, 3, 0, 7, planePos);
          generatePlane(3, 7, 0, 3, planePos);
          break;
        case 8: generatePlane(5, 7, 0, 7, planePos);
          generatePlane(0, 4, 4, 7, planePos);
          break;
      }
    }

    if (analogRead(POT_Y) > 800) tank1[0] -= 1;
    if (analogRead(POT_Y) < 300) tank1[0] += 1;
    if (analogRead(POT_X) > 800) tank1[1] -= 1;
    if (analogRead(POT_X) < 300) tank1[1] += 1;

    for (byte i = 0; i < 2; i++) {
      // выход за 0
      if (tank1[i] < 0) tank1[i] = 0;

      // выход за 7 (кубик 2х2х2 поэтому до 6)
      if (tank1[i] > 6) tank1[i] = 6;
    }

    // рисуем кубик
    for (byte i = 0; i < 2; i++)
      for (byte j = 0; j < 2; j++)
        for (byte k = 0; k < 2; k++)
          setVoxel(tank1[0] + i, tank1[1] + j, 0 + k);
  }
}

// рисует пластинку по 4 углам и глубина k
void generatePlane(byte fromX, byte toX, byte fromY, byte toY, byte k) {
  for (byte i = fromX; i <= toX; i++)
    for (byte j = fromY; j <= toY; j++)
      setVoxel(i, j, k);
}

boolean checkPlane(byte fromX, byte toX, byte fromY, byte toY) {
  for (byte i = fromX; i <= toX; i++)
    for (byte j = fromY; j <= toY; j++) {
      for (byte x = 0; x < 2; x++)
        for (byte y = 0; y < 2; y++)
          if (i == (tank1[0] + x) && j == (tank1[1] + y)) return true;
    }
  return false;
}

void shooter() {
  if (loading) {
    clearCube();
    loading = false;
    tank1[0] = 0;
    tank1[1] = 0;
    tank2[0] = 0;
    tank2[1] = 0;
  }

  timer++;
  if (timer > modeTimer) {
    timer = 0;
    if (bullet1_f) {
      bullet1[2] += 1;
      if (bullet1[2] > 7) {
        bullet1_f = false;
      } else {
        setVoxel(bullet1[0], bullet1[1], bullet1[2]);
      }
    }
  }

  if (tankTimer.isReady()) {
    clearCube();
    if (analogRead(POT_Y) > 800) tank1[0] -= 1;
    if (analogRead(POT_Y) < 300) tank1[0] += 1;
    if (analogRead(POT_X) > 800) tank1[1] -= 1;
    if (analogRead(POT_X) < 300) tank1[1] += 1;

    if (!digitalRead(BTN_UP) && !bullet1_f) {
      bullet1_f = true;
      bullet1[0] = tank1[0] + 1;
      bullet1[1] = tank1[1] + 1;
      bullet1[2] = 1;
    }

    for (byte i = 0; i < 2; i++) {
      if (tank1[i] < 0) tank1[i] = 0;
      if (tank2[i] < 0) tank2[i] = 0;

      if (tank1[i] > 5) tank1[i] = 5;
      if (tank2[i] > 5) tank2[i] = 5;
    }

    for (byte i = 0; i < 3; i++) {
      for (byte j = 0; j < 3; j++) {
        setVoxel(tank1[0] + i, tank1[1] + j, 0);
      }
    }
    setVoxel(tank1[0] + 1, tank1[1] + 1, 1);
  }
}

boolean appleGen = true;
boolean appleEaten = false;
byte snakeArray[100][3];
byte snakeLength;
int8_t head[3];
int8_t butt[3];
byte apple[3];
enum direction {UP, DOWN, LEFT, RIGHT, FORWARD, BACKWARD, STOP} dir;

void snake() {
  if (loading) {
    loading = false;
    millTimer = millis();
    appleGen = true;
    appleEaten = false;
    dir = FORWARD;
    head[0] = 0;
    head[1] = 7;
    head[2] = SNAKE_START - 1;
    snakeLength = SNAKE_START;
    butt[0] = 0;
    butt[1] = 7;
    butt[2] = 0;
    for (byte i = 0; i < SNAKE_START; i++) {
      snakeArray[i][0] = 0;
      snakeArray[i][1] = 7;
      snakeArray[i][2] = i;
      setVoxel(0, 7, i);
    }
  }
  butt_up.tick();
  butt_dwn.tick();

  // опрос кнопок и выбор направления
  // с учётом запрета двигаться назад (в змею)
  if (butt_up.isPress())
    if (dir != DOWN)
      dir = UP;

  if (butt_dwn.isPress())
    if (dir != UP)
      dir = DOWN;

  if (analogRead(POT_Y) > 800)
    if (dir != RIGHT)
      dir = LEFT;

  if (analogRead(POT_Y) < 300)
    if (dir != LEFT)
      dir = RIGHT;

  if (analogRead(POT_X) > 800)
    if (dir != FORWARD)
      dir = BACKWARD;

  if (analogRead(POT_X) < 300)
    if (dir != BACKWARD)
      dir = FORWARD;

  // таймер движения
  if (millis() - millTimer >= modeTimer) {
    millTimer = millis();

    // движение головы змеи в трёх плоскостях, изменяем координату
    switch (dir) {
      case UP: head[1] -= 1;
        break;
      case DOWN: head[1] += 1;
        break;
      case LEFT: head[0] -= 1;
        break;
      case RIGHT: head[0] += 1;
        break;
      case FORWARD: head[2] += 1;
        break;
      case BACKWARD: head[2] -= 1;
        break;
    }

    if (appleGen) {
      appleGen = false;

      byte success = false;
      while (!success) {      // бесконечный цикл генерации яблока
        // генерируем случайные координаты X Y Z
        apple[0] = random(0, 8);
        apple[1] = random(0, 8);
        apple[2] = random(0, 8);

        // проверяем на совпадение со змеёй
        success = true;
        for (byte i = 0; i < snakeLength; i++) {
          for (byte j = 0; j < 3; j++) {          // для трёх координат
            if (snakeArray[i][j] == apple[j]) {   // проверяем на совпадение с координатами тела змеи
              success = false;
              break;
            }
          }
        }
      }
      setVoxel(apple[0], apple[1], apple[2]);     // рисуем яблоко
    }

    // проверка на столкновение с собой
    for (byte i = 0; i < snakeLength; i++) {
      if (snakeArray[i][0] == head[0] &&
          snakeArray[i][1] == head[1] &&
          snakeArray[i][2] == head[2]) {
        showResult = true;
        lastMode = SNAKE;
        score = snakeLength - SNAKE_START;
        currentEffect = TEXT;
        changeMode();                             // всё по новой
        appleEaten = false;
        return;                                   // выходим из функции snake()
      }
    }

    appleEaten = true;                            // делаем предположение, что координата головы совпала с яблоком
    for (byte i = 0; i < 3; i++) {                // для всех трёх осей поочереди
      if (head[i] != apple[i])                    // если хоть одна координата головы не совпадает с яблоком
        appleEaten = false;                       // предположение неверно
      if (head[i] < 0 || head[i] > 7) {           // если удар головой в стену
        showResult = true;
        lastMode = SNAKE;
        score = snakeLength - SNAKE_START;
        currentEffect = TEXT;
        changeMode();                             // всё по новой
        appleEaten = false;
        return;                                   // выходим из функции snake()
      }

      butt[i] = snakeArray[0][i];                 // хвост принимает координаты из массива змеи (из ячейки 0)
      snakeArray[snakeLength][i] = head[i];       // массив змеи принимает координаты головы змеи в ячейку с длиной змеи
    }

    if (!appleEaten) {                              // если яблоко съедено
      for (byte i = 0; i < snakeLength; i++) {      // для всей длины змеи
        for (byte j = 0; j < 3; j++) {              // для трёх координат
          snakeArray[i][j] = snakeArray[i + 1][j];  // смещаем массив змеи на один шаг влево
        }
      }
      clearVoxel(butt[0], butt[1], butt[2]);        // стираем хвост
    } else {
      snakeLength++;                                // длина змеи увеличилась
      appleEaten = false;                           // опустить флаг
      appleGen = true;                              // разрешаем генерацию нового яблока
    }

    setVoxel(head[0], head[1], head[2]);            // рисуем новое положение головы змеи
  }
}

void walkingCube() {
  if (loading) {
    clearCube();
    loading = false;
    for (byte i = 0; i < 3; i++) {
      // координата от о до 700!
      coord[i] = 300;
      vector[i] = random(3, 8) * 15;
    }
  }
  timer++;
  if (timer > modeTimer) {
    timer = 0;
    clearCube();
    for (byte i = 0; i < 3; i++) {
      coord[i] += vector[i];
      if (coord[i] < 1) {
        coord[i] = 1;
        vector[i] = -vector[i];
        vector[i] += random(0, 6) - 3;
      }
      if (coord[i] > 700 - 100) {
        coord[i] = 700 - 100;
        vector[i] = -vector[i];
        vector[i] += random(0, 6) - 3;
      }
    }

    int8_t thisX = coord[0] / 100;
    int8_t thisY = coord[1] / 100;
    int8_t thisZ = coord[2] / 100;

    setVoxel(thisX, thisY, thisZ);
    setVoxel(thisX + 1, thisY, thisZ);
    setVoxel(thisX, thisY + 1, thisZ);
    setVoxel(thisX, thisY, thisZ + 1);
    setVoxel(thisX + 1, thisY + 1, thisZ);
    setVoxel(thisX, thisY + 1, thisZ + 1);
    setVoxel(thisX + 1, thisY, thisZ + 1);
    setVoxel(thisX + 1, thisY + 1, thisZ + 1);
  }
}

void sinusFill() {
  if (loading) {
    clearCube();
    loading = false;
  }
  timer++;
  if (timer > modeTimer) {
    timer = 0;
    clearCube();
    if (++pos > 10) pos = 0;
    for (uint8_t i = 0; i < 8; i++) {
      for (uint8_t j = 0; j < 8; j++) {
        int8_t sinZ = 4 + ((float)sin((float)(i + pos) / 2) * 2);
        for (uint8_t y = 0; y < sinZ; y++) {
          setVoxel(i, y, j);
        }
      }
    }
  }
}

void sinusThin() {
  if (loading) {
    clearCube();
    loading = false;
  }
  timer++;
  if (timer > modeTimer) {
    timer = 0;
    clearCube();
    if (++pos > 10) pos = 0;
    for (uint8_t i = 0; i < 8; i++) {
      for (uint8_t j = 0; j < 8; j++) {
        int8_t sinZ = 4 + ((float)sin((float)(i + pos) / 2) * 2);
        setVoxel(i, sinZ, j);
      }
    }
  }
}

void rain() {
  if (loading) {
    clearCube();
    loading = false;
  }
  timer++;
  if (timer > modeTimer) {
    timer = 0;
    shift(NEG_Y);
    uint8_t numDrops = random(0, 5);
    for (uint8_t i = 0; i < numDrops; i++) {
      setVoxel(random(0, 8), 7, random(0, 8));
    }
  }
}

uint8_t planePosition = 0;
uint8_t planeDirection = 0;
bool looped = false;

void planeBoing() {
  if (loading) {
    clearCube();
    uint8_t axis = random(0, 3);
    planePosition = random(0, 2) * 7;
    setPlane(axis, planePosition);
    if (axis == XAXIS) {
      if (planePosition == 0) {
        planeDirection = POS_X;
      } else {
        planeDirection = NEG_X;
      }
    } else if (axis == YAXIS) {
      if (planePosition == 0) {
        planeDirection = POS_Y;
      } else {
        planeDirection = NEG_Y;
      }
    } else if (axis == ZAXIS) {
      if (planePosition == 0) {
        planeDirection = POS_Z;
      } else {
        planeDirection = NEG_Z;
      }
    }
    timer = 0;
    looped = false;
    loading = false;
  }

  timer++;
  if (timer > modeTimer) {
    timer = 0;
    shift(planeDirection);
    if (planeDirection % 2 == 0) {
      planePosition++;
      if (planePosition == 7) {
        if (looped) {
          loading = true;
        } else {
          planeDirection++;
          looped = true;
        }
      }
    } else {
      planePosition--;
      if (planePosition == 0) {
        if (looped) {
          loading = true;
        } else {
          planeDirection--;
          looped = true;
        }
      }
    }
  }
}

uint8_t selX = 0;
uint8_t selY = 0;
uint8_t selZ = 0;
uint8_t sendDirection = 0;
bool sending = false;

void sendVoxels() {
  if (loading) {
    clearCube();
    for (uint8_t x = 0; x < 8; x++) {
      for (uint8_t z = 0; z < 8; z++) {
        setVoxel(x, random(0, 2) * 7, z);
      }
    }
    loading = false;
  }

  timer++;
  if (timer > modeTimer) {
    timer = 0;
    if (!sending) {
      selX = random(0, 8);
      selZ = random(0, 8);
      if (getVoxel(selX, 0, selZ)) {
        selY = 0;
        sendDirection = POS_Y;
      } else if (getVoxel(selX, 7, selZ)) {
        selY = 7;
        sendDirection = NEG_Y;
      }
      sending = true;
    } else {
      if (sendDirection == POS_Y) {
        selY++;
        setVoxel(selX, selY, selZ);
        clearVoxel(selX, selY - 1, selZ);
        if (selY == 7) {
          sending = false;
        }
      } else {
        selY--;
        setVoxel(selX, selY, selZ);
        clearVoxel(selX, selY + 1, selZ);
        if (selY == 0) {
          sending = false;
        }
      }
    }
  }
}

uint8_t cubeSize = 0;
bool cubeExpanding = true;

void woopWoop() {
  if (loading) {
    clearCube();
    cubeSize = 2;
    cubeExpanding = true;
    loading = false;
  }

  timer++;
  if (timer > modeTimer) {
    timer = 0;
    if (cubeExpanding) {
      cubeSize += 2;
      if (cubeSize == 8) {
        cubeExpanding = false;
      }
    } else {
      cubeSize -= 2;
      if (cubeSize == 2) {
        cubeExpanding = true;
      }
    }
    clearCube();
    drawCube(4 - cubeSize / 2, 4 - cubeSize / 2, 4 - cubeSize / 2, cubeSize);
  }
}

uint8_t xPos;
uint8_t yPos;
uint8_t zPos;

void cubeJump() {
  if (loading) {
    clearCube();
    xPos = random(0, 2) * 7;
    yPos = random(0, 2) * 7;
    zPos = random(0, 2) * 7;
    cubeSize = 8;
    cubeExpanding = false;
    loading = false;
  }

  timer++;
  if (timer > modeTimer) {
    timer = 0;
    clearCube();
    if (xPos == 0 && yPos == 0 && zPos == 0) {
      drawCube(xPos, yPos, zPos, cubeSize);
    } else if (xPos == 7 && yPos == 7 && zPos == 7) {
      drawCube(xPos + 1 - cubeSize, yPos + 1 - cubeSize, zPos + 1 - cubeSize, cubeSize);
    } else if (xPos == 7 && yPos == 0 && zPos == 0) {
      drawCube(xPos + 1 - cubeSize, yPos, zPos, cubeSize);
    } else if (xPos == 0 && yPos == 7 && zPos == 0) {
      drawCube(xPos, yPos + 1 - cubeSize, zPos, cubeSize);
    } else if (xPos == 0 && yPos == 0 && zPos == 7) {
      drawCube(xPos, yPos, zPos + 1 - cubeSize, cubeSize);
    } else if (xPos == 7 && yPos == 7 && zPos == 0) {
      drawCube(xPos + 1 - cubeSize, yPos + 1 - cubeSize, zPos, cubeSize);
    } else if (xPos == 0 && yPos == 7 && zPos == 7) {
      drawCube(xPos, yPos + 1 - cubeSize, zPos + 1 - cubeSize, cubeSize);
    } else if (xPos == 7 && yPos == 0 && zPos == 7) {
      drawCube(xPos + 1 - cubeSize, yPos, zPos + 1 - cubeSize, cubeSize);
    }
    if (cubeExpanding) {
      cubeSize++;
      if (cubeSize == 8) {
        cubeExpanding = false;
        xPos = random(0, 2) * 7;
        yPos = random(0, 2) * 7;
        zPos = random(0, 2) * 7;
      }
    } else {
      cubeSize--;
      if (cubeSize == 1) {
        cubeExpanding = true;
      }
    }
  }
}

bool glowing;
uint16_t glowCount = 0;

void glow() {
  if (loading) {
    clearCube();
    glowCount = 0;
    glowing = true;
    loading = false;
  }

  timer++;
  if (timer > modeTimer) {
    timer = 0;
    if (glowing) {
      if (glowCount < 448) {
        do {
          selX = random(0, 8);
          selY = random(0, 8);
          selZ = random(0, 8);
        } while (getVoxel(selX, selY, selZ));
        setVoxel(selX, selY, selZ);
        glowCount++;
      } else if (glowCount < 512) {
        lightCube();
        glowCount++;
      } else {
        glowing = false;
        glowCount = 0;
      }
    } else {
      if (glowCount < 448) {
        do {
          selX = random(0, 8);
          selY = random(0, 8);
          selZ = random(0, 8);
        } while (!getVoxel(selX, selY, selZ));
        clearVoxel(selX, selY, selZ);
        glowCount++;
      } else {
        clearCube();
        glowing = true;
        glowCount = 0;
      }
    }
  }
}

uint8_t charCounter = 0;
uint8_t charPosition = 0;

void text(String someText, byte textLength) {
  if (loading) {
    clearCube();
    charPosition = -1;
    charCounter = 0;
    loading = false;
  }
  timer++;
  if (timer > modeTimer) {
    timer = 0;

    shift(NEG_Z);
    charPosition++;

    if (charPosition == 7) {
      charCounter++;
      if (charCounter > textLength - 1) {
        charCounter = 0;
        if (showResult) {         // если играли в змейку и проиграли
          showResult = false;     // опустить флаг
          currentEffect = lastMode;     // включить прошлый режим
          changeMode();           // активировать режим
          return;                 // покинуть функцию text
        }
      }
      charPosition = 0;
    }
    if (charPosition == 0) {
      for (uint8_t i = 0; i < 8; i++) {
        byte thisFont = getFont(someText[charCounter], i);
        byte mirrorFont = 0;
        for (byte i = 0; i < 7; i++) {
          if (thisFont & (1 << 6 - i)) mirrorFont |= (1 << i);
          else mirrorFont |= (0 << i);
        }
        if (MIRROR_FONT) cube[i][0] = mirrorFont;
        else cube[i][0] = thisFont;
      }
    }
  }
}

void lit() {
  if (loading) {
    clearCube();
    for (uint8_t i = 0; i < 8; i++) {
      for (uint8_t j = 0; j < 8; j++) {
        cube[i][j] = 0xFF;
      }
    }
    loading = false;
  }
}

void setVoxel(uint8_t x, uint8_t y, uint8_t z) {
  cube[7 - y][7 - z] |= (0x01 << x);
}

void clearVoxel(uint8_t x, uint8_t y, uint8_t z) {
  cube[7 - y][7 - z] ^= (0x01 << x);
}

bool getVoxel(uint8_t x, uint8_t y, uint8_t z) {
  return (cube[7 - y][7 - z] & (0x01 << x)) == (0x01 << x);
}

void setPlane(uint8_t axis, uint8_t i) {
  for (uint8_t j = 0; j < 8; j++) {
    for (uint8_t k = 0; k < 8; k++) {
      if (axis == XAXIS) {
        setVoxel(i, j, k);
      } else if (axis == YAXIS) {
        setVoxel(j, i, k);
      } else if (axis == ZAXIS) {
        setVoxel(j, k, i);
      }
    }
  }
}

void shift(uint8_t dir) {

  if (dir == POS_X) {
    for (uint8_t y = 0; y < 8; y++) {
      for (uint8_t z = 0; z < 8; z++) {
        cube[y][z] = cube[y][z] << 1;
      }
    }
  } else if (dir == NEG_X) {
    for (uint8_t y = 0; y < 8; y++) {
      for (uint8_t z = 0; z < 8; z++) {
        cube[y][z] = cube[y][z] >> 1;
      }
    }
  } else if (dir == POS_Y) {
    for (uint8_t y = 1; y < 8; y++) {
      for (uint8_t z = 0; z < 8; z++) {
        cube[y - 1][z] = cube[y][z];
      }
    }
    for (uint8_t i = 0; i < 8; i++) {
      cube[7][i] = 0;
    }
  } else if (dir == NEG_Y) {
    for (uint8_t y = 7; y > 0; y--) {
      for (uint8_t z = 0; z < 8; z++) {
        cube[y][z] = cube[y - 1][z];
      }
    }
    for (uint8_t i = 0; i < 8; i++) {
      cube[0][i] = 0;
    }
  } else if (dir == POS_Z) {
    for (uint8_t y = 0; y < 8; y++) {
      for (uint8_t z = 1; z < 8; z++) {
        cube[y][z - 1] = cube[y][z];
      }
    }
    for (uint8_t i = 0; i < 8; i++) {
      cube[i][7] = 0;
    }
  } else if (dir == NEG_Z) {
    for (uint8_t y = 0; y < 8; y++) {
      for (uint8_t z = 7; z > 0; z--) {
        cube[y][z] = cube[y][z - 1];
      }
    }
    for (uint8_t i = 0; i < 8; i++) {
      cube[i][0] = 0;
    }
  }
}

void drawCube(uint8_t x, uint8_t y, uint8_t z, uint8_t s) {
  for (uint8_t i = 0; i < s; i++) {
    setVoxel(x, y + i, z);
    setVoxel(x + i, y, z);
    setVoxel(x, y, z + i);
    setVoxel(x + s - 1, y + i, z + s - 1);
    setVoxel(x + i, y + s - 1, z + s - 1);
    setVoxel(x + s - 1, y + s - 1, z + i);
    setVoxel(x + s - 1, y + i, z);
    setVoxel(x, y + i, z + s - 1);
    setVoxel(x + i, y + s - 1, z);
    setVoxel(x + i, y, z + s - 1);
    setVoxel(x + s - 1, y, z + i);
    setVoxel(x, y + s - 1, z + i);
  }
}

void lightCube() {
  for (uint8_t i = 0; i < 8; i++) {
    for (uint8_t j = 0; j < 8; j++) {
      cube[i][j] = 0xFF;
    }
  }
}

void clearCube() {
  for (uint8_t i = 0; i < 8; i++) {
    for (uint8_t j = 0; j < 8; j++) {
      cube[i][j] = 0;
    }
  }
}
