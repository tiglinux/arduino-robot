/* --> Código para desarmar bomba.
 --> Está faltando implementar algumas funções básicas, estão marcadas como //IMPLEMENTAR
 --> link dos estados e da imagem:
-> Estados: http://pastebin.com/idFtvMDz
-> Imagem: http://pixahc.com.br/blog/wp-content/uploads/2015/04/Mapa.png

Coded by: João Carlos Pandolfi Santana (06/06/2015)
*/
#include <NewPing.h>
#include <Wire.h>
#include <Servo.h>

// Default Values
#define DEBUG false
#define DEFAULT_SPEED 140

//Sensores Ultrassom
#define ECHO_PIN_F 2
#define TRIG_PIN_F 4
#define ECHO_PIN_L 5
#define TRIG_PIN_L 6
#define ECHO_PIN_R 7
#define TRIG_PIN_R 10
#define MAX_DISTANCE 300

//Distancias
#define WALL_DISTANCE 10    // 10 centimetros
#define OVER_DISTANCE 1000  // 1 Metro (passou disso, ignora)
#define DOOR_DISTANCE 20    // 20 centimetros (distancia ao lado da porta fechada)
#define COLIDER_DISTANCE 3  // 3 centimetros p evitar colisão

//Estado atual
int actual_state = 0;

//ultimo estado
int last_state = 0;

// tarefa [0 - Desarmar, 1 - Resgatar]
int task = 0;

//matriz de estados - Desarma bomba e preencho os estados (-1, sem estado)
int states_bomb [11][3] = {
  {0, 1, -1},
  {1, 2, -1},
  {2, 3, 4},
  {3, 5, -1},
  {4, 6, -1},
  {5, 7, -1},
  {6, 8, -1},
  {7, 9, -1},
  {8, 10, -1},
  {9, 10, -1},
  {10, -1, -1}
};

//condicoes dos estados
char condition_states_bomb[11] = {'A', 'B', 'C', 'D', 'D', 'G', 'G', 'E', 'F', 'E', 'M'};

//Inicialização do robô
Servo claw;

void setup() {
  Serial.begin(9600);
  //Sensores de ultrassom
  pinMode(TRIG_PIN_L, OUTPUT);
  pinMode(TRIG_PIN_R, OUTPUT);
  pinMode(TRIG_PIN_F, OUTPUT);
  pinMode(ECHO_PIN_L, INPUT);
  pinMode(ECHO_PIN_R, INPUT);
  pinMode(ECHO_PIN_F, INPUT);

  //Garra
  claw.attach(9);

  //funcoes
  enable_barcode_scan();
}


void loop() {
  if (task == 0) {
    check_change_state();
  }
}

//ativa leitura do cod de barras
void enable_barcode_scan() {
  //TODO
}

//desativa leitura do cod de barras
void disable_barcode_scan() {
  //TODO
}

// ====================== MAQUINA DE ESTADOS =======================

//verifica se mudou o estado e toma decisao [UTILIZÁVEL PELO USUARIO]
int check_change_state() {
  int newState = calculate_state_bomb();
  if (newState != actual_state) {
    last_state = actual_state;
    actual_state = newState;
    take_decision_state();
    return 1;
  }
  return 0;
}

//Calculate State
int calculate_state_bomb() {
  //calcula o estado atual
  int newState = actual_state;

  //percorro os possiveis estados
  for (int i = 0; i < 3; i++) {
    //se nao tiver um proximo estado, para loop
    if (states_bomb[actual_state][i] == -1)
      break;

    switch (condition_states_bomb[states_bomb[actual_state][i]]) {
      case 'A':
        if (check_state_A())
          newState = states_bomb[actual_state][i];
        break;

      case 'B':
        if (check_state_B())
          newState = states_bomb[actual_state][i];
        break;

      case 'C':
        if (check_state_C())
          newState = states_bomb[actual_state][i];
        break;

      case 'D':
        if (check_state_D())
          newState = states_bomb[actual_state][i];
        break;

      case 'E':
        if (check_state_E())
          newState = states_bomb[actual_state][i];
        break;

      case 'F':
        if (check_state_F())
          newState = states_bomb[actual_state][i];
        break;

      case 'G':
        if (check_state_G())
          newState = states_bomb[actual_state][i];
        break;
    }
  }

  return newState;
}

//Toma decisão após a troca de estado
void take_decision_state() {
  //para robo para tomada de decisao
  motorsStop();

  //toma decisao
  if (actual_state == 0) {
    enable_barcode_scan();
    move2Front();
  } else if (actual_state == 1) {
    disable_barcode_scan();
    move2Front();
  } else if (actual_state == 2) { //escolhe entre direita e esquerda
    turn(-90); //escolhi girar para Esquerda [ver se coloca random]
    move2Front();
  } else if (actual_state == 3) {
    move2Front();
  } else if (actual_state == 4 || actual_state == 6) {
    turn(-90); //viro para esquerda
    move2Front();
  } else if (actual_state == 5 || actual_state == 7) {
    turn(90); //viro para direita
    move2Front();
  } else if (actual_state == 8) {
    turn(-90); //gira para esquerda
    //teoricamente neste ponto, está de cara com a bomba
  } else if (actual_state == 9) {
    turn(90); //gira para direita
    //teoricamente neste ponto, está de cara com a bomba
  } else if (actual_state == 10) {
    //### ESTA DE CARA COM A BOMBA ###
    //ALGORITMO PARA APERTAR O BOTAO
  }
}

// ----------------------- ESTADOS ------------------------

int check_state_A() {
  if (get_distance('F') > DOOR_DISTANCE &&
      get_distance('L') <= WALL_DISTANCE &&
      get_distance('R') <= WALL_DISTANCE)
    return 1;

  return 0;
}

int check_state_B() {
  if (get_distance('F') >= COLIDER_DISTANCE)
    return 1;

  return 0;
}

int check_state_C() {
  if (get_distance('F') <= COLIDER_DISTANCE &&
      get_distance('L') > DOOR_DISTANCE &&
      get_distance('R') > DOOR_DISTANCE)
    return 1;

  return 0;
}

int check_state_D() {
  if (get_distance('F') >= COLIDER_DISTANCE &&
      get_distance('L') <= WALL_DISTANCE &&
      get_distance('R') <= WALL_DISTANCE)
    return 1;

  return 0;
}

int check_state_E() {
  if (get_distance('F') >= COLIDER_DISTANCE &&
      get_distance('L') <= WALL_DISTANCE &&
      get_distance('R') > DOOR_DISTANCE)
    return 1;

  return 0;
}

int check_state_F() {
  if (get_distance('F') >= COLIDER_DISTANCE &&
      get_distance('L') > WALL_DISTANCE &&
      get_distance('R') <= WALL_DISTANCE)
    return 1;

  return 0;
}

int check_state_G() {
  if (get_distance('F') <= COLIDER_DISTANCE ||
     (get_distance('L') <= WALL_DISTANCE &&
      get_distance('R') <= WALL_DISTANCE))
    return 1;

  return 0;
}

// ============== CONTROLE DE MOVIMENTACAO ================

// ------- ROBO ---------

//gira 90 ou -90 graus
void turn(int angle) {
  //TODO
}

//para robo
void motorsStop() {
  //TODO
}

//move o robo para frente
void move2Front() {
  move2Front(DEFAULT_SPEED);
}
void move2Front(int speed) {
  //TODO
}

//move o robo para tras
void move2Back() {
  move2Back(DEFAULT_SPEED);
}

void move2Back(int speed) {
  //TODO
}

// ============== CONTROLE DE SENSORES ======================

int read_barCode() {
  //TODO
  return 0;
}

NewPing sonarFront(TRIG_PIN_F, ECHO_PIN_F, MAX_DISTANCE);
NewPing sonarLeft(TRIG_PIN_L, ECHO_PIN_L, MAX_DISTANCE);
NewPing sonarRight(TRIG_PIN_R, ECHO_PIN_R, MAX_DISTANCE);

int get_distance(char p) {
  switch (p) {
    case 'L': {
        int cmL = sonarLeft.ping() / US_ROUNDTRIP_CM;
        if(DEBUG)
          debug_ultrasonic('L', cmL);
        return cmL;
      }
    case 'R': {
        int cmR = sonarRight.ping() / US_ROUNDTRIP_CM;
        if(DEBUG)
          debug_ultrasonic('R', cmR);
        return cmR;
      }
    case 'F': {
        int cmF = sonarFront.ping() / US_ROUNDTRIP_CM;
        if(DEBUG)
          debug_ultrasonic('F', cmF);
        return cmF;
      }
  }

  // throw invalid_argument( "the argument must be one of three characters L to left R to right and F front" );
}

void debug_ultrasonic(char p, int cm) {
  Serial.print(p + " ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  delay(500);
}

void moveClaw(char action) {
  int pos = 2;
  
  switch(action) {
    case 'p':
      for(pos = 75; pos >= 2; pos -= 1) {
        claw.write(pos);
        delay(15);
      }
    case 'c':
      for(pos = 2; pos < 75; pos += 1) {
        claw.write(pos);
        delay(15);
      }
  }
}
