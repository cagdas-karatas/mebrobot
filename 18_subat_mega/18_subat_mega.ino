#include <Servo.h>
Servo bizim_bolge;
Servo tokatlama;

const int in1 = 3;
const int in2 = 4;
const int in3 = 5;
const int in4 = 6;
const int enA = 2;
const int enB = 7;
const int sol_goz = 32;
const int on_goz = 31;
const int sag_goz = 30;


// ****** BÖLGE BULAN RENK SENSÖRÜ ******
#define s0 22 //Mavi
#define s1 23 //Sarı
#define s2 24 //Yeşil
#define s3 25 //Mor
#define out 26 //Turuncu
// VCC Kırmızı
// GND Beyaz

int kirmizi = 0, yesil = 0, mavi = 0, noFilter = 0;

int enKucukKirmizi = 50, enBuyukKirmizi = 50,
    enKucukYesil = 50, enBuyukYesil = 50,
    enKucukMavi = 50, enBuyukMavi = 50,
    enKucukFiltresiz = 50, enBuyukFiltresiz = 50;

int sol_goz_durum = 0;

int baslangicNoFilter = 0;

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
  pinMode(out, INPUT);
  Serial.begin(9600);
  bizim_bolge.attach(9);
  bizim_bolge.write(1);
  //tokatlama.attach(8);
  //tokatlama.write(1);

  olcum();
  baslangicNoFilter = noFilter;

  while (digitalRead(sag_goz) == 0) // SAĞ GÖZDE ENGEL VAR İSE (YANİ BALŞANGIÇTA) HAREKET ETMEYECEK
  {}
  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);
}

void loop()
{
rastgele();
   // Mega Sinyal Gönderdiğinde başlayacak
   //duvar_takip();

}

// ******************  RASTGELE FONKSİYONU  ****************************
void rastgele()
{
  analogWrite(enA, 160);
  analogWrite(enB, 150);
  sol_goz_durum = 0;
  if (digitalRead(on_goz) == 0) //  ÖNÜNDE ENGEL GÖRÜRSE SAĞ VE SOLDA DA ENGEL VAR MI DİYE KONTROL EDER
  {
    if (digitalRead(sol_goz) == 0) // SOLDA DA ENGEL GÖRÜRSE
    {
      sol_goz_durum = 1;
      Serial.println("ÖN ve SOLDA AYNI ANDA ENGEL VAR...");
      // SOL KAPALI
      analogWrite(enA, 160);
      analogWrite(enB, 170);
      geri();
      delay(400);
      saga_don();
      delay(400);
    }
    else if (digitalRead(sag_goz) == 0) // SAĞDA DA ENGEL GÖRÜRSE
    {
      Serial.println("ÖN ve SAĞDA AYNI ANDA ENGEL VAR...");
      // SAG KAPALI
      analogWrite(enA, 160);
      analogWrite(enB, 170);
      geri();
      delay(400);
      sola_don();
      delay(400);
    }
  }
  else if (digitalRead(sol_goz) == 0) // SADECE SOLDA ENGEL GÖRÜRSE
  {
    sol_goz_durum = 1;
    Serial.println("SADECE SOLDA ENGEL VAR...");
    // SOL KAPALI
    analogWrite(enA, 160);
    analogWrite(enB, 170);
    geri();
    delay(100);
    saga_don();
    delay(200);
  }
  else if (digitalRead(sag_goz) == 0) // SADECE SAĞDA ENGEL GÖRÜRSE
  {
    Serial.println("SADECE SAĞDA ENGEL VAR...");
    // SAG KAPALI
    analogWrite(enA, 160);
    analogWrite(enB, 170);
    geri();
    delay(100);
    sola_don();
    delay(200);
  }
  else
  {
    Serial.println("ENGEL YOOOK...");
    ileri();
  }

}

void kalibre () {
  //Loop kısmında bu kod olacak
  /*
    long firstTime = millis();
    while( (millis() - firstTime) <= 15000)
    {
    kalibre();
    }
    Serial.println("Değişim yapın...");
    delay(5000);
    firstTime = millis();
    while( (millis() - firstTime) <= 15000)
    {
    kalibre();
    }
    Serial.print("Kırmızı Aralık : map(kirmizi, ");
    Serial.print(enKucukKirmizi);
    Serial.print(", ");
    Serial.print(enBuyukKirmizi);
    Serial.println(", 0, 100);");
    Serial.print("Mavi Aralık : map(mavi, ");
    Serial.print(enKucukMavi);
    Serial.print(", ");
    Serial.print(enBuyukMavi);
    Serial.println(", 0, 100);");
    Serial.print("Yeşil Aralık : map(yesil, ");
    Serial.print(enKucukYesil);
    Serial.print(", ");
    Serial.print(enBuyukYesil);
    Serial.println(", 0, 100);");
    Serial.print("Filtresiz Aralık : map(noFilter, ");
    Serial.print(enKucukFiltresiz);
    Serial.print(", ");
    Serial.print(enBuyukFiltresiz);
    Serial.println(", 0, 100);");
    while(true){}
  */

  //kırmızıyı okuyoruz
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  delay(50);

  kirmizi = pulseIn(out, LOW);

  Serial.print("Kırmızı: ");
  Serial.print(kirmizi);
  Serial.print("\t");

  //maviyi okuyoruz
  digitalWrite(s2, LOW);
  digitalWrite(s3, HIGH);
  delay(50);

  mavi = pulseIn(out, LOW);

  Serial.print("Mavi: ");
  Serial.print(mavi);
  Serial.print("\t");

  //yeşili okuyoruz
  digitalWrite(s2, HIGH);
  digitalWrite(s3, HIGH);
  delay(50);

  yesil = pulseIn(out, LOW);

  Serial.print("Yeşil: ");
  Serial.print(yesil);
  Serial.print("\t");

  //filtresiz okuyoruz
  digitalWrite(s2, HIGH);
  digitalWrite(s3, LOW);
  delay(50);

  noFilter = pulseIn(out, LOW);

  Serial.print("Filtresiz: ");
  Serial.println(noFilter);

  if (kirmizi < enKucukKirmizi)
  {
    enKucukKirmizi = kirmizi;
  }
  if (kirmizi > enBuyukKirmizi)
  {
    enBuyukKirmizi = kirmizi;
  }
  if (mavi < enKucukMavi)
  {
    enKucukMavi = mavi;
  }
  if (mavi > enBuyukMavi)
  {
    enBuyukMavi = mavi;
  }
  if (yesil < enKucukYesil)
  {
    enKucukYesil = yesil;
  }
  if (yesil > enBuyukYesil)
  {
    enBuyukYesil = yesil;
  }
  if (noFilter < enKucukFiltresiz)
  {
    enKucukFiltresiz = noFilter;
  }
  if (noFilter > enBuyukFiltresiz)
  {
    enBuyukFiltresiz = noFilter;
  }
}

void olcum() {
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  delay(10);

  kirmizi = pulseIn(out, LOW);
  kirmizi = map(kirmizi, 21, 207, 0, 100);

  //maviyi okuyoruz
  digitalWrite(s2, LOW);
  digitalWrite(s3, HIGH);
  delay(10);

  mavi = pulseIn(out, LOW);
  mavi = map(mavi, 6, 187, 0, 100);


  //yeşili okuyoruz
  digitalWrite(s2, HIGH);
  digitalWrite(s3, HIGH);
  delay(10);

  yesil = pulseIn(out, LOW);
  yesil = map(yesil, 22, 246, 0, 100);


  //Filtresiz okuyoruz
  digitalWrite(s2, HIGH);
  digitalWrite(s3, LOW);
  delay(10);

  noFilter = pulseIn(out, LOW);
  noFilter = map(noFilter, 8, 68, 0, 100);

  
    Serial.print("KIRMIZI: ");
    Serial.print(kirmizi);
    Serial.print(" ");

    Serial.print("MAVİ: ");
    Serial.print(mavi);
    Serial.print(" ");

    Serial.print("YEŞİL: ");
    Serial.print(yesil);
    Serial.print(" ");

    Serial.print("FİLTRESİZ: ");
    Serial.println(noFilter);

}

boolean aralik_sorgu(int kk, int kb, int mk, int mb, int yk, int yb, int fk, int fb) {
  return (
           (kirmizi >= kk) && (kirmizi <= kb)
           && (mavi >= mk) && (mavi <= mb)
           && (yesil >= yk) && (yesil <= yb)
           && (noFilter >= fk) && (noFilter <= fb) );
}

// ******************  FONKSİYONLAR BAŞLIYOR ****************************
void soldan_park()
{
  // *** fren ***
  dur();
  delay(500);
  // *** sağa geriye tank dönüşü ***
  analogWrite(enA, 255);
  analogWrite(enB, 100);
  saga_don();
  delay(2200);
  // *** fren ***
  dur();
  delay(500);
  // *** geri ***
  analogWrite(enA, 160);
  analogWrite(enB, 160);
  geri();
  delay(1000);
  // *** fren ***
  dur();
  delay(500);
  // *** sağa ileri***
  analogWrite(enA, 100);
  analogWrite(enB, 160);
  ileri();
  delay(550);
  // *** geri ***
  analogWrite(enA, 160);
  analogWrite(enB, 160);
  geri();
  delay(1000);
  // *** fren ***
  dur();
  delay(300);
  // *** Bırakma hareketi için ileri ***
  ileri();
  //KAPAK AÇILIR
  //bizim_bolge.write(80);
  delay(1750);
  while (1)
  {
    dur();
    //bizim_bolge.write(1);
  }
}


void sagdan_park()
{
  // *** fren ***
  dur();
  delay(500);
  // *** sola geriye tank dönüşü ***
  analogWrite(enA, 100);
  analogWrite(enB, 255);
  sola_don();
  delay(2200);
  // *** fren ***
  dur();
  delay(500);
  // *** geri ***
  analogWrite(enA, 160);
  analogWrite(enB, 160);
  geri();
  delay(1000);
  // *** fren ***
  dur();
  delay(500);
  // *** sola ileri***
  analogWrite(enA, 160);
  analogWrite(enB, 100);
  ileri();
  delay(550);
  // *** geri ***
  analogWrite(enA, 160);
  analogWrite(enB, 160);
  geri();
  delay(1000);
  // *** fren ***
  dur();
  delay(300);
  // *** Bırakma hareketi için ileri ***
  ileri();
  //KAPAK AÇILIR
  //bizim_bolge.write(80);
  delay(1750);
  while (1)
  {
    dur();
    //bizim_bolge.write(1);
  }
}


void duvar_takip()
{
  if (digitalRead(on_goz) == 0)
  {
    dur();
    delay(100);
    geri();
    delay(300);
  }
  else if (digitalRead(sol_goz) == 0)
  {
    // SOL KAPALI
    if (digitalRead(on_goz) == 0)
    {
      // ÖN KAPALI
      dur();
      delay(100);
      sola_gerile();
      delay(300);
    }
    else
    {
      // SOL DUVAR DÖNGÜSÜ
      dur();
      delay(100);
      while (digitalRead(sol_goz) == 0)
      {
        //SOL KAPALI
        if (digitalRead(on_goz) == 0)
        {
          // ÖN KAPALI
          //KÖŞEYE GELDİK
          dur();
          delay(100);
          analogWrite(enA, 160);
          analogWrite(enB, 0);
          geri();
          delay(700);
          dur();
          delay(300);
          olcum();
          if (kirmizi < mavi && (noFilter - baslangicNoFilter) > 20) {
            //KIRMIZI BÖLGE
            soldan_park();
          } else if (mavi < kirmizi && (noFilter - baslangicNoFilter) > 20) {
            soldan_park();
          }
          saga_ilerle();
          delay(200);
        }
        else
        {
          //SOL DUVARDAYIZ
          saga_ilerle();
        }
      }
      while (digitalRead(sol_goz) == 1)
      {
        //SOL GÖZDE ENGEL YOK SOL DUVARA YANAŞ
        sola_ilerle();
      }
    }
  }

  else if (digitalRead(sag_goz) == 0)
  {

    // SAG KAPALI
    if (digitalRead(on_goz) == 0)
    {
      // ÖN KAPALI
      dur();
      delay(100);
      saga_gerile();
      delay(300);
    }
    else
    {
      // SAĞ DUVAR DÖNGÜSÜ
      dur();
      delay(100);
      while (digitalRead(sag_goz) == 0)
      {
        //SAĞ KAPALI
        if (digitalRead(on_goz) == 0)
        {
          // ÖN KAPALI
          //KÖŞEDEYİZ
          dur();
          delay(100);
          olcum();
          if (kirmizi < mavi && (noFilter - baslangicNoFilter) > 20) {
            //KIRMIZI BÖLGE
            sagdan_park();
          } else if (mavi < kirmizi && (noFilter - baslangicNoFilter) > 20) {
            //MAVİ BÖLGE
            sagdan_park();
          }
          analogWrite(enA, 0);
          analogWrite(enB, 160);
          geri();
          delay(700);
          sola_ilerle();
          delay(200);
        }
        else
        {
          sola_ilerle();
        }
      }
      while (digitalRead(sag_goz) == 1)
      {
        //SAĞDA ENGEL YOK SAĞ DUVARA YANAŞ
        saga_ilerle();
      }
    }
  }
  else
  {
    analogWrite(enA, 160);
    analogWrite(enB, 150);
    ileri();
  }
}

void ileri()
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}
void dur()
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, HIGH);
}

void geri()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void saga_don()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void sola_don()
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void saga_ilerle()
{
  analogWrite(enA, 100);
  analogWrite(enB, 130);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void saga_gerile()
{
  analogWrite(enA, 160);
  analogWrite(enB, 100);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void sola_ilerle()
{
  analogWrite(enA, 130);
  analogWrite(enB, 100);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void sola_gerile()
{
  analogWrite(enA, 100);
  analogWrite(enB, 160);
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
