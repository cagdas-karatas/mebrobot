#include <Servo.h>
Servo bizim_bolge;
Servo rakip_bolge;
Servo ceza;

const int in1 = 5;
const int in2 = 6;
const int in3 = 3;
const int in4 = 4;
const int enA = 7;  //sağ motor
const int enB = 2;  //sol motor
const int sol_goz = 26;
const int on_goz = 24;
const int sag_goz = 28;

// ****** BÖLGE BULAN RENK SENSÖRÜ ******
#define s0 36   //Mavi
#define s1 38   //Sarı
#define s2 40   //Yeşil
#define s3 42   //Mor
#define out 34  //Turuncu
// VCC Kırmızı
// GND Beyaz

int kirmizi = 0, mavi = 0, noFilter = 0;

int sol_goz_durum = 0;

int baslangicNoFilter = 0;

int bolge_sinyali = A15;
int sayac_bildirim = A14;
int ceza_bildirim = A13;
int kilit_sinyali = A12;

int renk_switch = 44;
int rgb1 = 48;
int rgb2 = 50;

int bolge = 0;  //DEFAULT MAVİ

int duvarla_isim_var = 0;

void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(sol_goz, INPUT);
  pinMode(on_goz, INPUT);
  pinMode(sag_goz, INPUT);
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(bolge_sinyali, OUTPUT);
  pinMode(kilit_sinyali, OUTPUT);
  pinMode(rgb1, OUTPUT);
  pinMode(rgb2, OUTPUT);
  pinMode(renk_switch, INPUT);
  pinMode(sayac_bildirim, INPUT);
  pinMode(ceza_bildirim, INPUT);
  pinMode(out, INPUT);
  Serial.begin(9600);

  if (digitalRead(renk_switch) == 0) {
    //rengimiz mavi
    digitalWrite(rgb1, LOW);
    digitalWrite(rgb2, HIGH);
    digitalWrite(bolge_sinyali, LOW);
  } else {
    //rengimiz kırmızı
    digitalWrite(rgb1, HIGH);
    digitalWrite(rgb2, LOW);
    digitalWrite(bolge_sinyali, HIGH);
    bolge = 1;  //BÖLGEMİZ KIRMIZI
  }

  ceza.attach(10);
  rakip_bolge.attach(12);
  bizim_bolge.attach(11);
  ceza.write(40);
  rakip_bolge.write(120);
  bizim_bolge.write(0);

  digitalWrite(kilit_sinyali, LOW);

  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);
  olcum();
  baslangicNoFilter = noFilter;

  while (digitalRead(sag_goz) == 0)  // SAĞ GÖZDE ENGEL VAR İSE (YANİ BALŞANGIÇTA) HAREKET ETMEYECEK
  {}
}

void loop() {
  /*
  dur();
  while (true) {
    Serial.print("Sayaç Bildirim: ");
    Serial.println(digitalRead(sayac_bildirim));
    Serial.print("Ceza Bildirim: ");
    Serial.println(digitalRead(ceza_bildirim));
    Serial.println("\n");
    delay(500);
  }*/

  if (digitalRead(sayac_bildirim) == 1)  //KENDİ TOPUMUZUN SAYACI DOLMUŞ
  {
    //DUVAR TAKİBİ VE KENDİ BÖLGEMİZE BOŞALTMA
    digitalWrite(kilit_sinyali, HIGH);
    dur();
    delay(2000);
    duvarla_isim_var = 1;
    while (duvarla_isim_var == 1) {
      duvar_takip(1);
    }
    digitalWrite(kilit_sinyali, LOW);
    delay(2000);
  } else if (digitalRead(ceza_bildirim) == 1)  //CEZA TOPLADIK
  {
    //DUVAR TAKİBİ VE RAKİP BÖLGEYE BOŞALTMA
    digitalWrite(kilit_sinyali, HIGH);
    dur();
    delay(2000);
    duvarla_isim_var = 1;
    while (duvarla_isim_var == 1) {
      duvar_takip(0);
    }
    digitalWrite(kilit_sinyali, LOW);
    delay(1000);
  } else {
    rastgele();
  }
}

// ******************  RASTGELE FONKSİYONU  ****************************
void rastgele() {
  analogWrite(enA, 130);
  analogWrite(enB, 120);
  sol_goz_durum = 0;
  if (digitalRead(on_goz) == 0)  //  ÖNÜNDE ENGEL GÖRÜRSE SAĞ VE SOLDA DA ENGEL VAR MI DİYE KONTROL EDER
  {
    if (digitalRead(sol_goz) == 0)  // SOLDA DA ENGEL GÖRÜRSE
    {
      sol_goz_durum = 1;
      Serial.println("ÖN ve SOLDA AYNI ANDA ENGEL VAR...");
      // SOL KAPALI
      analogWrite(enA, 170);
      analogWrite(enB, 160);
      geri();
      delay(400);
      saga_don();
      delay(400);
    } else if (digitalRead(sag_goz) == 0)  // SAĞDA DA ENGEL GÖRÜRSE
    {
      Serial.println("ÖN ve SAĞDA AYNI ANDA ENGEL VAR...");
      // SAG KAPALI
      analogWrite(enA, 170);
      analogWrite(enB, 160);
      geri();
      delay(400);
      sola_don();
      delay(400);
    }
  } else if (digitalRead(sol_goz) == 0)  // SADECE SOLDA ENGEL GÖRÜRSE
  {
    sol_goz_durum = 1;
    Serial.println("SADECE SOLDA ENGEL VAR...");
    // SOL KAPALI
    analogWrite(enA, 170);
    analogWrite(enB, 160);
    geri();
    delay(100);
    saga_don();
    delay(200);
  } else if (digitalRead(sag_goz) == 0)  // SADECE SAĞDA ENGEL GÖRÜRSE
  {
    Serial.println("SADECE SAĞDA ENGEL VAR...");
    // SAG KAPALI
    analogWrite(enA, 170);
    analogWrite(enB, 160);
    geri();
    delay(100);
    sola_don();
    delay(200);
  } else {
    Serial.println("ENGEL YOOOK...");
    ileri();
  }
}

void olcum() {
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  delay(10);

  kirmizi = pulseIn(out, LOW);

  //maviyi okuyoruz
  digitalWrite(s2, LOW);
  digitalWrite(s3, HIGH);
  delay(10);

  mavi = pulseIn(out, LOW);

  //Filtresiz okuyoruz
  digitalWrite(s2, HIGH);
  digitalWrite(s3, LOW);
  delay(10);

  noFilter = pulseIn(out, LOW);

  Serial.print("KIRMIZI: ");
  Serial.print(kirmizi);
  Serial.print(" ");

  Serial.print("MAVİ: ");
  Serial.print(mavi);
  Serial.print(" ");

  Serial.print("FİLTRESİZ: ");
  Serial.println(noFilter);
}

// ******************  FONKSİYONLAR BAŞLIYOR ****************************

void sagdan_park(int nereye) {
  // *** fren ***
  dur();
  delay(500);
  // park kodu
  //SOLA DÖN
  analogWrite(enA, 140);
  analogWrite(enB, 140);
  sola_don();
  delay(700);
  //HAFİF SOLA MEYİLLİ İLERİ
  analogWrite(enA, 150);
  analogWrite(enB, 100);
  ileri();
  delay(700);
  dur();
  delay(250);
  //NORMAL GERİ
  analogWrite(enA, 150);
  analogWrite(enB, 150);
  geri();
  delay(1000);
  //HAFİF SOLA MEYİLLİ İLERİ
  analogWrite(enA, 140);
  analogWrite(enB, 100);
  ileri();
  delay(600);
  dur();
  delay(100);
  //NORMAL GERİ
  analogWrite(enA, 150);
  analogWrite(enB, 150);
  geri();
  delay(1000);
  dur();
  delay(1000);
  //KAPAK AÇILIR
  if (nereye == 1)  //KENDİ BÖLGEME GİTTİYSEM
  {
    bizim_bolge.write(60);
    delay(1000);
    if (digitalRead(sol_goz) == 0) {
      //HAFİF SAĞA MEYİLLİ İLERİ
      analogWrite(enA, 120);
      analogWrite(enB, 150);
      ileri();
      delay(2000);
    } else {
      //HAFİF SOLA MEYİLLİ İLERİ
      analogWrite(enA, 150);
      analogWrite(enB, 120);
      ileri();
      delay(1300);
    }
    bizim_bolge.write(0);
    dur();
  } else {
    ceza.write(0);
    delay(1000);
    if (digitalRead(sol_goz) == 0) {
      //HAFİF Sağa MEYİLLİ İLERİ
      analogWrite(enA, 120);
      analogWrite(enB, 150);
      ileri();
      delay(2000);
    } else {
      //HAFİF SOLA MEYİLLİ İLERİ
      analogWrite(enA, 150);
      analogWrite(enB, 120);
      ileri();
      delay(1300);
    }
    ceza.write(40);
    dur();
  }
}

void duvar_takip(int nereye)  //NEREYE 1SE KENDİ BÖLGEME 0SA RAKİBE
{
  if (digitalRead(sol_goz) == 0) {
    //HAFİF GERİ GEL
    analogWrite(enA, 130);
    analogWrite(enB, 130);
    geri();
    delay(200);
    dur();
    delay(100);
    //SOLA MEYİLLİ GERİ GEL
    analogWrite(enA, 0);
    analogWrite(enB, 140);
    geri();
    delay(1000);
    //SAĞA MEYİLLİ İLERİ
    analogWrite(enA, 130);
    analogWrite(enB, 80);
    ileri();
    delay(400);
  }
  if (digitalRead(sag_goz) == 0) {  //ÖN GÖZ GÖRENE KADAR İLERLE //SAĞ GÖZÜM DE GÖRÜYORSA KENDİNİ DUVARLA PARALEL YAP
    analogWrite(enA, 0);
    analogWrite(enB, 160);
    geri();
    delay(300);
    while (!((noFilter - baslangicNoFilter) > 15 && (kirmizi < mavi && nereye == bolge)) && !((noFilter - baslangicNoFilter) > 15 && (mavi < kirmizi && nereye != bolge)))  //BÖLGE OLMADIĞI SÜRECE DUVAR DÖNGÜSÜ
    {
      if (digitalRead(on_goz) == 0) {  //KÖŞEYE GELDİK
        //BEKLE
        dur();
        delay(100);
        //SOLA TANK DÖNÜŞÜ
        analogWrite(enA, 130);
        analogWrite(enB, 130);
        sola_don();
        delay(600);
      } else {
        while (digitalRead(sag_goz) == 0) {
          analogWrite(enA, 200);
          analogWrite(enB, 200);
          sola_don();
        }
        unsigned long firstTime = millis();
        while (digitalRead(sag_goz) == 1) {
          saga_ilerle();
          if (millis() - firstTime > 5000) {
            analogWrite(enA, 120);
            analogWrite(enB, 120);
            ileri();
            delay(1500);
          }
        }
      }
      olcum();
    }
    sagdan_park(nereye);
    duvarla_isim_var = 0;
  } else {
    analogWrite(enA, 110);
    analogWrite(enB, 110);
    ileri();
  }
}

void ileri() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}
void dur() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void geri() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void saga_don() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void sola_don() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void saga_ilerle() {
  analogWrite(enA, 80);
  analogWrite(enB, 130);
  ileri();
}

void sola_ilerle() {
  analogWrite(enA, 130);
  analogWrite(enB, 80);
  ileri();
}

void saga_gerile() {
  analogWrite(enA, 130);
  analogWrite(enB, 80);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void sola_gerile() {
  analogWrite(enA, 80);
  analogWrite(enB, 130);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

boolean yuzde_sorgu(int kucuk, int buyuk) {
  float yuzde = ((float)mavi / (float)kirmizi) * 100;
  Serial.print("m/k = ");
  Serial.println(yuzde);
  return (yuzde >= kucuk) && (yuzde <= buyuk);
}
