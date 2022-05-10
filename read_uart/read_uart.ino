struct ctrlState{
  int X,Y;
  boolean A,B;
};
ctrlState *ctrl_A, *ctrl_B, *ctrl_T;
void setup(){}
void loop(){}
void inputTask(void* pvparam) {
  Serial.begin(115200, );
  Serial1.begin(115200, SERIAL_8N1, 16,17);
  Serial2.begin(115200, SERIAL_8N1, 9,10);
  ctrl_A=(ctrlState *)malloc(sizeof(ctrlState));
  ctrl_B=(ctrlState *)malloc(sizeof(ctrlState));
  ctrl_T=(ctrlState *)malloc(sizeof(ctrlState));

  char in[16];
  int i;
  for(;;) {
    while(Serial1.available()>0){
      in[i] = Serial1.read();
      
      if(in[i] == '\n'){
        i=0;
        parseInput(in, ctrl_T);
        
        ctrlState *tmp=ctrl_A;
        ctrl_A=ctrl_T;
        ctrl_T=tmp;

        printCtrl(ctrl_A);
      }else{
        i++;
      }
    }
    vTaskDelay(20);
    while(Serial2.available()>0){
      in[i] = Serial2.read();
      
      if(in[i] == '\n'){
        i=0;
        parseInput(in, ctrl_T);
        
        ctrlState *tmp=ctrl_B;
        ctrl_B=ctrl_T;
        ctrl_T=tmp;

        printCtrl(ctrl_B);
      }else{
        i++;
      }
    }
    vTaskDelay(20);
  }
}

void parseInput(char str[], struct ctrlState *ctrl){
  ctrl->X = atoi(str);
  int i;
  for(i = 0; str[i] != ' '; i++);
  ctrl->Y = atoi(str+i);
  for(i++; str[i] != ' '; i++);
  ctrl->B = str[i+1]=='1';
  ctrl->A = str[i+3]=='1';
}

void printCtrl(struct ctrlState *ctrl){
  Serial.print(ctrl_T->X);
  Serial.print(" ");
  Serial.print(ctrl_T->Y);
  Serial.print(" ");
  Serial.print(ctrl_T->A);
  Serial.print(" ");
  Serial.println(ctrl_T->B);
  Serial.println();
}
