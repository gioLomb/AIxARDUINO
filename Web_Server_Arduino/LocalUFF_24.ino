
//AGGIUNGERE SERVO E PIR CON SCATOLA
#include <Servo.h> //LIBRERIA PER SERVOMOTORI
#include <LiquidCrystal_I2C.h> //LIBRERIA PER MONITOLìR LCD (I2C)
#include "WiFiS3.h"//LIBRERIA PER COLLEGAMENTO ALLA RETE
#include "pas.h"  //FILE PER PASSWORD E SSID DELLA RETE
#include <EEPROM.h> //LIBRERIA PER MEMORIA ARDUINO
//NOTE MELODIA
#define LA 220
#define SI 246
#define DO 261
#define RE 294
#define MI 330
#define FAD 370
#define SOL 392
#define LAA 501
#define PAUSA 0

#define LED 4 // LED_BUILTIN;
#define BUZZER 13
#define PIR 8

Servo servo_flag,servo_scatola;
LiquidCrystal_I2C lcd(0x27, 16, 4);

int contStorage = 0;
bool second = true;
//SSID E PASSWORD DELLA RETE
char ssid[] = SECRET_SSID; 
char pass[] = SECRET_PASS; 
int status = WL_IDLE_STATUS;//VARIABILE PER REGISTRARE LO STATO DELLA CONNESSIONE
WiFiServer server(80); //VARIABILE SERVER DA AVVIARE SULLA PORTA 80
//MELODIA
int arr[]={MI,FAD,MI,FAD,MI,FAD,MI,PAUSA,FAD,MI,PAUSA,FAD,MI,PAUSA,FAD,MI,FAD,MI,SI,DO,PAUSA,RE,MI,RE,MI,RE,MI,RE,PAUSA,MI,RE,MI,RE,LA,SI,PAUSA,MI,FAD,MI,FAD,MI,FAD,MI,PAUSA,FAD,MI,PAUSA,FAD,MI,FAD,MI,FAD,MI,SI,DO,PAUSA,RE,MI,RE,MI,RE,MI,RE,PAUSA,FAD,MI,FAD,MI,FAD,SOL,PAUSA,LAA,PAUSA,SOL,PAUSA,FAD,PAUSA,FAD,MI};
int durata[]={8,8,8,8,8,8,4,4,8,4,4,8,4,4,8,8,8,4,8,1,2,8,8,8,8,8,8,4,4,8,8,8,8,8,1,1,8,8,8,8,8,8,4,4,8,4,4,8,4,8,8,8,4,8,1,1,8,8,8,8,8,8,4,4,8,8,8,8,8,2,4,8,8,8,8,2,2,8,1};
bool entry=true;
String x = "";

void setup() {
//EEPROM.get(3,contStorage);
//EEPROM.put(3,contStorage);
    Serial.begin(921600);
    Serial.setTimeout(1);//TEMPO DI ATTESA PER LETTURA CARATTERI
    servo_flag.write(0);
    servo_flag.attach(9);
    servo_flag.write(0);
     servo_scatola.write(0);
servo_scatola.attach(10);
 servo_scatola.write(0);
    pinMode(BUZZER, OUTPUT);
    pinMode(LED, OUTPUT);
    pinMode(PIR, INPUT);

    lcd.init();
    lcd.backlight(); //RETROILLUMINAZIONE MONITOR LCD
    
//ATTENDI FINO A CHE NON VIENE STABILITA LA CONNESSIONE CON LA RETE
    while (status != WL_CONNECTED) {
        controlloPIR();
     
        status = WiFi.begin(ssid, pass);
        delay(10000);
    }

    server.begin(); //INIZIALIZZA WEB SERVER
    Status(); //STAMPA INFORMAZIONI RETE
}
///////////CONTINUA SU ufficiale_16-06
void loop() {
    WiFiClient client = server.available();//VERIFICA SE UN CLIENT E' DISPONIBILE 
    controlloPIR();

    if (client) { //VERIFICA CHE IL CLIENT SIA COLLEGATO
        String currentLine = "";

        while (client.connected()) {//VERIFICA CHE UN CLIENT GIA' PRESENTE SIA ANCORA CONNESSO
            if (client.available()) {//VERIFICA L'INVIO DI DATI DAL CLIENT
                char c = client.read();//LEGGE OGNI CARATTERE DELLA RICHIESTA DEL CLIENT
               
                controlloPIR();

                if (c == '\n') {
                    if (currentLine.length() == 0) {
                      //RICHIESTA HTTP TERMINATA
                        if (second) {
                            HTTPResponseMainPage(client);//RISPOSTA HTTP DEL SERVER CON PAGINA WEB PRINCIPALE ()
                        } else {
                          //PAGINA WEB SECONDARIA COLLEGATA (/L)
                          int val=((int)EEPROM.read(0)+EEPROM.read(1));

String offPage = "<!DOCTYPE html><html><head><style>body{font-family:Arial,sans-serif;margin:20px;}table{width:100%;border-collapse:collapse;}th,td{padding:15px;text-align:center;border:1px solid #ddd;}th{background-color:#4CAF50;color:white;font-size:18px;}tr:nth-child(even){background-color:#f2f2f2;}h1{color:#333;}</style><title>LED OFF</title></head><body><table><thead><tr><th>NOME</th><th>DATA</th><th>ORA</th></tr></thead><tbody><tr id='0'><td></td><td></td><td></td></tr><tr id='1'><td></td><td></td><td></td></tr><tr id='2'><td></td><td></td><td></td></tr></tbody></table><h1>TOTALE ACCESSI: "+ String(val) +"</h1><script>let acc0=localStorage.getItem('0');let acc1=localStorage.getItem('1');let acc2=localStorage.getItem('2');if(localStorage.key(0)==null)acc0='   ';if(localStorage.key(1)==null)acc1='   ';if(localStorage.key(2)==null)acc2='   ';document.getElementById('0').children[0].innerHTML=(acc0 || '   ').split(' ')[0];document.getElementById('0').children[1].innerHTML=(acc0 || '   ').split(' ')[1];document.getElementById('0').children[2].innerHTML=(acc0 || '   ').split(' ')[2];document.getElementById('1').children[0].innerHTML=(acc1 || '   ').split(' ')[0];document.getElementById('1').children[1].innerHTML=(acc1 || '   ').split(' ')[1];document.getElementById('1').children[2].innerHTML=(acc1 || '   ').split(' ')[2];document.getElementById('2').children[0].innerHTML=(acc2 || '   ').split(' ')[0];document.getElementById('2').children[1].innerHTML=(acc2 || '   ').split(' ')[1];document.getElementById('2').children[2].innerHTML=(acc2 || '   ').split(' ')[2];document.getElementById('" + String(contStorage) + "').style.color='red';</script></body></html>";

                            client.print(offPage);
                        }
                        client.println();//TERMINA RISPOSTA
                        break; //ESCI DAL CICLO WHILE POICHE' LA RICHIESTA E' TERMINATA
                    } else {
                        currentLine = "";//SVUOTA PER LA PROSSIMA RIGA
                    }
                } else if (c != '\r') {//IGNORA CARATTERI DI RITORNO A CAPO
                    currentLine += c;//AGGIUNGI IL CARATTERE LETTO ALLA RIGA CORRENTE
                }

                if (currentLine.indexOf("GET /H")==0) { //VERIFICA RICHIESTA HTTP PAGINA PRINCIPALE
              currentLine="";
                  x.trim();
                    if(x.length()>0 ||entry){
                        EEPROM.get(3, contStorage);

    // Se contStorage non è inizializzato (valore non valido), impostalo a 0
    if (contStorage < -1 || contStorage > 1024) { // Assumendo che il valore massimo di contStorage sia 1024
        contStorage = 0;
    }
          contStorage++;
         /** if(entry){
            contStorage++;
            entry=false;
          }**/
          EEPROM.put(3,contStorage);
          entry=false;
          }
                    second = true;
                    Serial.print("now");//STRINGA PER ESEGUIRE FACE RECOGNITION
                   sorpresa();
                }
                if (currentLine.indexOf("GET /L")==0) {//VERIFICA RICHIESTA HTTP SECONDA PAGINA
                currentLine="";
                    second = false;
                    
                }
            }
        }
        client.stop(); //DISCONNESSIONE CLIENT
    }
}

void Status() {
  bool ipChanged=false;
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
//IPAddress ip (192,168,194,114);//POSSIBILITA' DI INSERIRE L'IP STATICO
 for (int i = 0; i < 4; i++) {
    if (ip[i] != EEPROM.read(i + 8)) {
     ipChanged=true;
    } 
    Serial.println(ip[i]);
    Serial.println(EEPROM.read(i+8));
  }

  // SALVA IP NELLA EEPROM
  for (int i = 0; i < 4; i++) {
    EEPROM.write(i + 8, ip[i]);
    Serial.println(EEPROM.read(i+8));
    delay(1000);
  }
  //SE L'IP E' NUOVO AZZERO IL CONTSTORAGE
  if(ipChanged){
   contStorage = -1;
      EEPROM.put(3, contStorage);
  }
    Serial.print("IP Address: ");
    Serial.println(ip);
lcd.print(ip);
    Serial.print("http://");
    Serial.println(ip);
}

void controlloPIR() {
    if (digitalRead(PIR) == HIGH) {
        digitalWrite(LED, HIGH);
    } else {
        digitalWrite(LED, LOW);
    }
}

void HTTPResponseMainPage(WiFiClient cl) {
  //COMPOSIZIONE RISPOSTA HTTP PRINCIPALE
    cl.println("HTTP/1.1 200 OK");
    cl.println("Content-type:text/html");
    cl.println();
                            EEPROM.get(3, contStorage);

     if (contStorage < -1 || contStorage > 1024) { // Assumendo che il valore massimo di contStorage sia 1024
        contStorage = 0;
    }
    if (contStorage >= 3) contStorage = 0;
    EEPROM.put(3,contStorage);
    x.trim();
    if (x.length()==0) {x = "   ";}else{
      
      cl.print("<script>localStorage.setItem('" + String(contStorage) + "','" + x + "')</script>");
     
    }
    cl.print("<p style=\"font-size:14pt;\">clicca <a href=\"/H\">qui</a> per farti riconoscere<br></p>");
    cl.print("<p style=\"font-size:14pt;\">visualizza <a href=\"/L\">qui</a> gli ultimi 3 accessi<br></p>");
}

void sorpresa() {
    while (!Serial.available());
    x = Serial.readString(); //LETTURA DAL SERIALE (PYTHON)
    Serial.print(x);
    
    //SPLITTING STRINGA LETTA
    String nome=x.substring(0,x.indexOf(' '));
    String data=x.substring(x.indexOf(' ')+1,x.lastIndexOf(' '));
    String ora=x.substring(x.lastIndexOf(' ')+1,x.length());
    //STAMPA STRINGA SU MONITOR LCD
    lcd.clear();
    lcd.print("BENVENUTO");
    lcd.setCursor(0, 1);
    lcd.print("NOME: "+nome);
    lcd.setCursor(0, 2);
    lcd.print("DATA: "+data);
    lcd.setCursor(0, 3);
    lcd.print("ORA: "+ora);
    controlloPIR();
//MOVIMENTO CONTROLLATO SERVOMOTORI
servo_scatola.write(90);
    for (int i = 0; i < 90; i++) {
        servo_flag.write(i);
        delay(50);
    }
    //INCREMENTO ACCESSI

byte value = EEPROM.read(0);
if(value=255){
  value=EEPROM.read(1);
  value+=1;
  EEPROM.write(1,value);
}else{
value += 1;
EEPROM.write(0, value);
}
  
  //MUSICA
    int dimensione = sizeof(arr) / sizeof(int);
    for (int i = 0; i < dimensione; i++) {
        int delayNote = 1000 / durata[i];
        tone(BUZZER, arr[i], delayNote);
        //MOVIMENTO BANDIERINA
        if (i % 2 == 0) {
            servo_flag.write(60);
        } else {
            servo_flag.write(120);
        }
        delay(delayNote + 100);
        noTone(BUZZER);
    }
    servo_flag.write(0);
    servo_scatola.write(0);
}

