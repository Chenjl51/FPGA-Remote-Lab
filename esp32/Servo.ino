#define PS2_X      15
#define PS2_Y      2
#define SW         4
#define SERVO1     13  // Y轴控制
#define SERVO2     12  // X轴控制
#define RESOLUTION 12
#define FREQ       50
#define CHANNEL1   0
#define CHANNEL2   1

int min_width = 0.6 / 20 * pow(2, RESOLUTION);
int max_width = 2.5 / 20 * pow(2, RESOLUTION);

// 设置灵敏度死区
int DEAD_ZONE = 100;  // 可调范围 50~200，越大越不敏感

int value_x, value_y;

void setup() {
  pinMode(PS2_X, INPUT);
  pinMode(PS2_Y, INPUT);
  pinMode(SW, INPUT_PULLUP);

  Serial.begin(9600);

  ledcSetup(CHANNEL1, FREQ, RESOLUTION);
  ledcAttachPin(SERVO1, CHANNEL1);

  ledcSetup(CHANNEL2, FREQ, RESOLUTION);
  ledcAttachPin(SERVO2, CHANNEL2);
}

void loop() {
  int raw_x = analogRead(PS2_X);
  int raw_y = analogRead(PS2_Y);

  // 死区处理
  if (abs(raw_x - 2048) < DEAD_ZONE) raw_x = 2048;
  if (abs(raw_y - 2048) < DEAD_ZONE) raw_y = 2048;

  value_y = map(raw_y, 0, pow(2, RESOLUTION), min_width, max_width);
  value_x = map(raw_x, 0, pow(2, RESOLUTION), min_width, max_width);

  // 串口输出
  Serial.printf("X: %d => %d | Y: %d => %d | Button: %d\n", 
                raw_x, value_x, raw_y, value_y, digitalRead(SW));

  // 输出PWM信号控制两个舵机
  ledcWrite(CHANNEL1, value_y);
  ledcWrite(CHANNEL2, value_x);

  delay(100);
}

