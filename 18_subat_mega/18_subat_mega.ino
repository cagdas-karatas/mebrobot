#include <Servo.h>
Servo bizim_bolge;
Servo rakip_bolge;
Servo ceza;

const int in1 = 3;
const int in2 = 4;
const int in3 = 5;
const int in4 = 6;
const int enA = 2;
const int enB = 7;
const int sol_goz = 26;
const int on_goz = 24;
const int sag_goz = 22;

// ****** BÖLGE BULAN RENK SENSÖRÜ ******
#define s0 36 //Mavi
#define s1 38 //Sarı
#define s2 40 //Yeşil
#define s3 42 //Mor
#define out 34 //Turuncu
// VCC Kırmızı
// GND Beyaz

int kirmizi = 0, yesil = 0, mavi = 0, noFilter = 0;

int enKucukKirmizi = 50, enBuyukKirmizi = 50,
    enKucukYesil = 50, enBuyukYesil = 50,
    enKucukMavi = 50, enBuyukMavi = 50,
    enKucukFiltresiz = 50, enBuyukFiltresiz = 50;

int sol_goz_durum = 0;

int baslangicNoFilter = 0;

int bolge_sinyali = A15;
int sayac_bildirim = A14;
int ceza_bildirim = A13;
int kilit_sinyali = A12;

int renk_switch = 44;
int rgb1 = 48;
int rgb2 = 50;

int bolge = 0;//DEFAULT MAVİ

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

  if (digitalRead(renk_switch) == 0)
  {
    //rengimiz mavi
    digitalWrite(rgb1, LOW);
    digitalWrite(rgb2, HIGH);
    digitalWrite(bolge_sinyali, LOW);
  }
  else
  {
    //rengimiz kırmızı
    digitalWrite(rgb1, HIGH);
    digitalWrite(rgb2, LOW);
    digitalWrite(bolge_sinyali, HIGH);
    bolge = 1; //BÖLGEMİZ KIRMIZI
  }

  ceza.attach(10);
  rakip_bolge.attach(11);
  bizim_bolge.attach(12);
  ceza.write(180);
  rakip_bolge.write(180);
  bizim_bolge.write(180);

  digitalWrite(kilit_sinyali, LOW);

  olcum();
  baslangicNoFilter = noFilter;
/*
  while (digitalRead(sag_goz) == 0) // SAĞ GÖZDE ENGEL VAR İSE (YANİ BALŞANGIÇTA) HAREKET ETMEYECEK
  {}*/
  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);
}

void loop()
{
  Serial.println(digitalRead(sayac_bildirim));
  Serial.println(digitalRead(ceza_bildirim));
  if (digitalRead(sayac_bildirim)) //KENDİ TOPUMUZUN SAYACI DOLMUŞ
  {
    //DUVAR TAKİBİ VE KENDİ BÖLGEMİZE BOŞALTMA
    digitalWrite(kilit_sinyali, HIGH);
    duvar_takip(1);
  }
  else if (digitalRead(ceza_bildirim)) //CEZA TOPLADIK
  {
    //DUVAR TAKİBİ VE RAKİP BÖLGEYE BOŞALTMA
    digitalWrite(kilit_sinyali, HIGH);
    duvar_takip(0);
  }
  else
  {
    rastgele();
  }
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

  //yeşili okuyoruz
  digitalWrite(s2, HIGH);
  digitalWrite(s3, HIGH);
  delay(10);

  yesil = pulseIn(out, LOW);

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
void soldan_park(int nereye) //nereye 1se kendi bölgemin kapağı 0sa rakibe gidiyorum cezanın kapağı
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
  if (nereye) //KENDİ BÖLGEME GİTTİYSEM
  {
    bizim_bolge.write(180);
    delay(1750);
    bizim_bolge.write(0);
  }
  else
  {
    ceza.write(180);
    delay(1750);
    ceza.write(0);
  }

  while (1)
  {
    dur();
  }
}

void sagdan_park(int nereye)
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
  if (nereye) //KENDİ BÖLGEME GİTTİYSEM
  {
    bizim_bolge.write(180);
    delay(1750);
    bizim_bolge.write(0);
  }
  else
  {
    ceza.write(180);
    delay(1750);
    ceza.write(0);
  }
  while (1)
  {
    dur();
  }
}

void duvar_takip(int nereye) //NEREYE 1SE KENDİ BÖLGEME 0SA RAKİBE
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
          if ( (kirmizi < mavi && (noFilter - baslangicNoFilter) > 20) && (nereye == bolge) ) // (nereye == 1 && bolge == 1) || (nereye == 0 && bolge == 0)
          {
            //KIRMIZI BÖLGE
            soldan_park(nereye);
          }
          else if ( (mavi < kirmizi && (noFilter - baslangicNoFilter) > 20) && (nereye != bolge) )
          {
            //MAVİ BÖLGE
            soldan_park(nereye);
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
          analogWrite(enA, 0);
          analogWrite(enB, 160);
          geri();
          delay(700);
          dur();
          delay(300);
          olcum();
          if ( (kirmizi < mavi && (noFilter - baslangicNoFilter) > 20) && (nereye == bolge) ) // (nereye == 1 && bolge == 1) || (nereye == 0 && bolge == 0)
          {
            //KIRMIZI BÖLGE
            sagdan_park(nereye);
          }
          else if ( (mavi < kirmizi && (noFilter - baslangicNoFilter) > 20) && (nereye != bolge) )
          {
            //MAVİ BÖLGE
            sagdan_park(nereye);
          }
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
