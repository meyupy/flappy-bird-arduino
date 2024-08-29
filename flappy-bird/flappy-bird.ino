/*

           1088BS
  16 15 14 13 12 11 10 09
  01 02 03 04 05 06 07 08

  01-08: 2-9
  09-12: 13-10
  13-16: A0-A3

*/

const byte PINS_ROW[8] = {A0, 12, A2, 13, 5, A3, 7, 2};
const byte PINS_COL[8] = {9, 8, 4, A1, 3, 10, 11, 6};
const byte PIN_BUTTON_1 = A4, PIN_BUTTON_2 = A5;

byte state[8][8] = {};

class Game{

  private:

    class Button{

      private:

        bool pressed = false;

      public:

        bool isClicked(int isPushedButton){
          if(!this->pressed&&isPushedButton){
            this->pressed = true;
            return true;
          }
          if(this->pressed&&!isPushedButton) this->pressed = false;
          return false;
        }

    };

    class Bird{

      public:

        byte y = 0;  // x = 1, it is fixed

        void update(int isClickedButton1, bool shouldMove){
          if(isClickedButton1) this->y -= 2;
          if(shouldMove) this->y += 1;
        }

    };

    class Pipes{

      public:

        byte data[2][2] = {{8, random(0, 3)}, {13, random(0, 3)}};  // x position, pipe top y position

        void update(){
          for(byte i=0; i<2; i++){
            const byte pipeX = this->data[i][0];
            if(pipeX>=0&&pipeX!=255){
              this->data[i][0] -= 1;
            } else{
              this->data[i][0] = 8;
              this->data[i][1] = random(0, 3);
            }
          }
        }

    };

    Button button1;
    Bird bird;
    Pipes pipes;

    void updateState(byte state[8][8], byte birdY, byte pipesData[2][2]){

      for(byte x=0; x<8; x++){
        for(byte y=0; y<8; y++){
          state[x][y] = 0;
        }
      }

      if(birdY>=0&&birdY<=7) state[1][birdY] = 1;

      for(byte i=0; i<2; i++){
        for(byte j=0; j<2; j++){
          const byte pipeX = pipesData[i][0]+j;
          if(pipeX>=0&&pipeX<=7){
            const byte pipeYTop = pipesData[i][1];
            for(byte pipeY=0; pipeY<8; pipeY++){
              if(pipeY<=pipeYTop||pipeY>=pipeYTop+4) state[pipeX][pipeY] = 1;
            }
          }
        }
      }

    }

    bool isGameOver(byte birdY, byte pipesData[2][2]) {

      if(birdY==8) return true;

      for(byte i=0; i<2; i++){
        const byte pipeX = pipesData[i][0];
        const byte pipeYTop = pipesData[i][1];
        if((pipeX==0||pipeX==1)&&(birdY<=pipeYTop||birdY>=pipeYTop+4)) return true;
      }

      return false;

    }

    const byte FTW = 5;  // frames to wait
    long int totalFrames = 0;
    bool running = false;

  public:

    void iterate(byte state[8][8], int isPushedButton1, int isPushedButton2){

      if(this->running){
        if(this->totalFrames%this->FTW==0 && this->isGameOver(this->bird.y,this->pipes.data)) this->running = false;
        this->bird.update(this->button1.isClicked(isPushedButton1), this->totalFrames%this->FTW==0);
        if(this->totalFrames%this->FTW==0) this->pipes.update();
        if(this->running) this->updateState(state, this->bird.y, this->pipes.data);
      }

      if(!this->running&&isPushedButton2){
        this->bird.y = 0;
        this->pipes.data[0][0]=8;this->pipes.data[1][0]=13;
        this->pipes.data[0][1]=random(0,3);this->pipes.data[1][1]=random(0,3);
        this->running = true;
      }

      this->totalFrames++;

    }

};

Game game;

void setup() {
  
  pinMode(PIN_BUTTON_1, INPUT_PULLUP);
  pinMode(PIN_BUTTON_2, INPUT_PULLUP);

  for(byte i=0; i<8; i++){
    pinMode(PINS_ROW[i], OUTPUT);
    pinMode(PINS_COL[i], OUTPUT);
    digitalWrite(PINS_COL[i], HIGH);
    for(byte j=0; j<8; j++){
      state[i][j] = i%2==j%2 ? 1 : 0;
    }
  }

}

void loop() {

  game.iterate(state, !digitalRead(PIN_BUTTON_1), !digitalRead(PIN_BUTTON_2));
  displayMatrix(state);

}

void displayMatrix(byte state[8][8]) {

  for(byte r=0; r<8; r++){
    digitalWrite(PINS_ROW[r], HIGH);
    for(byte c=0; c<8; c++){
      const byte pixelState = state[c][r];
      digitalWrite(PINS_COL[c], !pixelState);
      delay(1);
      if(pixelState) digitalWrite(PINS_COL[c], HIGH);
    }
    digitalWrite(PINS_ROW[r], LOW);
  }

}