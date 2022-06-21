/*etask:
 * 0:running
 * 1:ready
 * 2:blocked
 * 3:suspended
 */

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_GFX.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>

#define PIN 4
#define PIN2 2

#define mw 32
#define mh 8
#define NUMMATRIX (mw*mh)
 
CRGB matrixleds[NUMMATRIX];
CRGB matrixleds2[NUMMATRIX];



FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(matrixleds, mw, mh, 1, 1,     
      NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
      NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG );
      
      FastLED_NeoMatrix *matrix2 = new FastLED_NeoMatrix(matrixleds2, mw, mh, 1, 1,     
      NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
      NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG );



// Replace with your network credentials
const char* ssid       = "testnet";
const char* password   = "marchuter";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);


//task handling
TaskHandle_t menuHandles[2];
int currentTask = 1;
//0: idle, 1: menu, 2: window
//
int mode;

//Inputs
struct ctrlState{
  int X,Y;
  boolean A,B;
};
ctrlState *ctrl_A, *ctrl_B, *ctrl_T;

//mode enum
//enum Emode{idle, menu, game};
//Emode mode;

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

// Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(7200);
  
   mode = 1; 
  //3 highest prio
  //essential tasks
  //xTaskCreate(inputTask,"inputs", 512, NULL, 3, NULL);
  xTaskCreate(master,"master", 4096, NULL, 2, NULL);
  //xTaskCreate(idleTask,"idle screen", 512, NULL, 1, NULL);

  
  menuHandles[0] = TaskHandle_t("timeTask");
  menuHandles[1] = TaskHandle_t("tempTask");
  menuHandles[2] = TaskHandle_t("pongTask");
  xTaskCreate(tempScreen,"tempAndHum", 512, NULL, 1, &menuHandles[1]);
  xTaskCreate(timeScreen,"time", 512, NULL, 1, &menuHandles[0]);
  xTaskCreate(pongScreen,"pong", 512, NULL, 1, &menuHandles[2]);
}

//die Tasks werden aufgerufen, welche die klickbar sind brauchen eigenen Title screen der auf den knopfdruck reagiert
//has to make sure only one task is running at a time
void master(void* pvparameters){
  vTaskDelay(1000);
  Serial.println("masterTask started");
  while(true){
    Serial.print("masterTask running in mode: ");
    Serial.println(mode);
    if(mode == 1){
      while(true){
        int taskArrLen = sizeof(menuHandles) / sizeof(menuHandles[0]);
        for(int i = 0; i<=taskArrLen;i++){
          Serial.print("Checking task: ");
          Serial.println(i);
          //suspend all unwanted tasks
          if(i != currentTask){
            if(eTaskGetState(menuHandles[i]) == eTaskState(eRunning) || eTaskGetState(menuHandles[i]) == eTaskState(eReady) || eTaskGetState(menuHandles[i]) == eTaskState(eBlocked)){
              vTaskSuspend(menuHandles[i]);
              Serial.print("  task suspended: ");
              Serial.println(i);
            }
          }
          //start current Task if needed
          if(i == currentTask){
            if(eTaskGetState(menuHandles[i]) == eTaskState(eSuspended)){//!= eTaskState(eRunning) || eTaskGetState(menuHandles[i]) != eTaskState(eReady) || eTaskGetState(menuHandles[i]) != eTaskState(eBlocked)){
              vTaskResume(menuHandles[i]);
              Serial.print("  task resumed: ");
              Serial.print(i);
              Serial.println(eTaskGetState(menuHandles[i]));
            }
          } 
        }
        if(ctrl_A){
          if(ctrl_A->B){
            if(ctrl_A->A){
              mode = 0;
              break;
              Serial.println("mode changed to idle");
            }else{
              currentTask= ++currentTask%taskArrLen;
              Serial.println("changed to Screen: ");
              Serial.println(currentTask);
            }
          }
        }
      vTaskDelay(500);
     }
     vTaskDelay(20);
    }
  }
}

void timeScreen(void* pvparameters){
  while(true){
    Serial.println("time");
    vTaskDelay(2000);
  }
}

void tempScreen(void* pvparameters){
  while(true){
    Serial.println("tempHum");
    vTaskDelay(2000);
  }
  
}

void pongScreen(void* pvparameters){
  while(true){
    //titlescreen am anfang(schrift was da kommt wenn ma klickt)
    //iwas von wegen button a gedrückt, dann spielstart und wenn beide Knöpfe 2sek gedrückt wieder menu mode
    Serial.println("pong");
    vTaskDelay(2000);
  }
}

void idleTask(void* pvparam){
  //bla bla cooles muster und so
  //irgendein knopf gedrückt mode = menu;
  while(true){
    Serial.println("idletask");
    vTaskDelay(5000);
  }
}


//Controller A, controller B, temporärer Speicher
void inputTask(void* pvparam) {
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
    vTaskDelay(20 / portTICK_PERIOD_MS);
    while(Serial2.available()>0){
      in[i] = Serial2.read();
      
      if(in[i] == '\n'){
        i=0;
        parseInput(in, ctrl_T);
        
        ctrlState *tmp=ctrl_B;
        ctrl_B=ctrl_T;
        ctrl_T=tmp;

        Serial.print("A: ");
        printCtrl(ctrl_A);
        Serial.print("B: ");
        printCtrl(ctrl_B);
      }else{
        i++;
      }
    }
    vTaskDelay(20 / portTICK_PERIOD_MS);
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


String formattedDate;
String dayStamp;
String timeStamp;

String Time()
{
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
return(timeStamp);
delay(1000);
}

String Date()
{
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(5, splitT);
  // Extract time
return(dayStamp);
delay(1000);
}


void printTimeDate(void* pvparam){
  //zeit und datum ausgeben
  while(true){

     matrix->fillScreen(0);
     matrix->setCursor(0,0);
     matrix->print(Time());
     matrix->setTextColor(matrix->Color(0, 0, 255));
     matrix->show();
    
     matrix2->fillScreen(0);
     matrix2->setCursor(0,0);
     matrix2->print(Date());
     matrix2->setTextColor(matrix->Color(0, 0, 255));
     matrix2->show(); 
     
    vTaskDelay(1000);
  }
}





void loop() {
}
