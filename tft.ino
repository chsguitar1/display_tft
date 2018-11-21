

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MINPRESSURE 200
#define MAXPRESSURE 1000

// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino
const int XP = 6, XM = A2, YP = A1, YM = 7; //ID=0x9341
const int TS_LEFT = 907, TS_RT = 136, TS_TOP = 942, TS_BOT = 139;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button on_btn, off_btn, ini_btn, stop_btn;

int pixel_x, pixel_y;     //Touch_getXY() updates global vars
int min = 0;
int time = 0;
int contTime = 1;
int tela = 1;
int porta_rele1 = 31;

bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
        pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
    }
    return pressed;
}

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
void escreveNumero(int posx,int posy, int texto,int tamanho,int cor){ // Função criada para facilitar escrita de texto
  tft.setCursor(posx,posy); // Cursor é deslocado para posição passada como parâmetro
  tft.setTextColor(cor); // Cor a ser escrita é alterada conforme valor recebido como parâmetro
  tft.setTextSize(tamanho); // Tamanho da fonte é  alterado conforme parâmetro recebido
  tft.print(texto); // Texto passado por parâmetro é escrito na tela
}
void escreveTexto(int posx,int posy, String texto,int tamanho,int cor){ // Função criada para facilitar escrita de texto
  tft.setCursor(posx,posy); // Cursor é deslocado para posição passada como parâmetro
  tft.setTextColor(cor); // Cor a ser escrita é alterada conforme valor recebido como parâmetro
  tft.setTextSize(tamanho); // Tamanho da fonte é  alterado conforme parâmetro recebido
  tft.print(texto); // Texto passado por parâmetro é escrito na tela
}
void setup(void)
{
    Serial.begin(9600);
    uint16_t ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);
    Serial.println("Calibrate for your Touch Panel");
     pinMode(porta_rele1, OUTPUT);
    
    if (ID == 0xD3D3) ID = 0x9486; // write-only shield
    tft.begin(0x9341);
    tft.setRotation(1);            //PORTRAIT
    tft.fillScreen(BLACK);//         lar,alt
     digitalWrite(porta_rele1,HIGH);
   telaInicial();
  
}
void telaInicial(){
   tft.fillScreen(BLACK);
  
   on_btn.initButton(&tft,  40, 70, 60, 70, WHITE, CYAN, BLACK, "-", 4);
    off_btn.initButton(&tft, 275, 70, 60, 70, WHITE, CYAN, BLACK, "+", 4);
    ini_btn.initButton(&tft, 155, 190, 200, 70, GREEN, CYAN, BLACK, "INICIAR", 3);
    on_btn.drawButton(false);
    off_btn.drawButton(false);
    ini_btn.drawButton(false);
    tft.fillRect(80, 30, 155, 80, RED);
    escreveNumero(145,50,min,6,WHITE);
    escreveTexto(5,10,"Indique os minutos abaixo",2,WHITE);
  }
  
void telaTratamento(){
   tela = 2;
  digitalWrite(porta_rele1,LOW);
  tft.fillScreen(BLACK);
 
   stop_btn.initButton(&tft, 155, 190, 200, 70, RED, YELLOW, BLACK, "CANCELAR", 3);
   stop_btn.drawButton(false);
   
   tft.fillRect(80, 30, 155, 80, RED);
   time = min * 60 * 1000;
    escreveNumero(145,50,min,6,WHITE);
     escreveTexto(30,10,"Minutos para finalizar",2,WHITE);
    
}
/* two buttons are quite simple
 */
void loop(void)
{
  
  if(tela == 1){
    bool down = Touch_getXY();
    on_btn.press(down && on_btn.contains(pixel_x, pixel_y));
    off_btn.press(down && off_btn.contains(pixel_x, pixel_y));
    ini_btn.press(down && ini_btn.contains(pixel_x, pixel_y));
    
    if (on_btn.justReleased())
        on_btn.drawButton();
    if (off_btn.justReleased())
        off_btn.drawButton();
        
    if (ini_btn.justReleased())
        ini_btn.drawButton();
    
    if (on_btn.justPressed()) {
        on_btn.drawButton(true);
       min--;
       if(min >= 0){
         String thisString = String(min);
          tft.fillRect(80, 30, 155, 80, RED);
         if(min<10){
       escreveTexto(145,50,thisString,6,WHITE);
       }else{
         escreveTexto(130,50,thisString,6,WHITE);
        }
        }
      
    }
    if (off_btn.justPressed()) {
        off_btn.drawButton(true);
        min++;
       String thisString = String(min);
       tft.fillRect(80, 30, 144, 80, RED);
       if(min<10){
       escreveTexto(145,50,thisString,6,WHITE);
       }else{
         escreveTexto(130,50,thisString,6,WHITE);
        }
    }
    if(ini_btn.justPressed()){
      ini_btn.drawButton(true);
      if(min > 0){
        telaTratamento();
        }
      }
  }
  if(tela== 2){
   bool down = Touch_getXY();
   stop_btn.press(down && stop_btn.contains(pixel_x, pixel_y));
   if (stop_btn.justReleased())
        stop_btn.drawButton();
        
        if (stop_btn.justPressed() && tela == 2) {
        stop_btn.drawButton(true);
        finalizar();
        }
        
    
    Serial.println(contTime);
    if(min > 0){
      if(contTime == 60){ 
      tft.fillRect(80, 30, 144, 80, RED);
      min--;
      String thisString = String(min);
      if(min< 10){
         escreveTexto(145,50,thisString,6,WHITE);
        }else{
            escreveTexto(130,50,thisString,6,WHITE);
          } 
    
      contTime = 1;
      
      }
        delay(1000);
        contTime++;
    }else{
      finalizar();
      }
    }
}
void finalizar(){
   digitalWrite(porta_rele1,HIGH);
  min = 0;
  contTime == 1;
     tela = 1;
      telaInicial();
  }


