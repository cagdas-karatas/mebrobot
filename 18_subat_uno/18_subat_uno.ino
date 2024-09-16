#include <Servo.h>

Servo arka_sol_kapak;
Servo arka_sag_kapak;
Servo ust_kapak;

#define out 8
#define s0 9
#define s1 10
#define s2 11
#define s3 12

int kirmizi, yesil, mavi, noFilter;

int enKucukKirmizi = 50, enBuyukKirmizi = 50,
    enKucukYesil = 50, enBuyukYesil = 50,
    enKucukMavi = 50, enBuyukMavi = 50,
    enKucukFiltresiz = 50, enBuyukFiltresiz = 50;

int kirmizi_veriler[7] = {0, 0, 0, 0, 0, 0, 0};
int mavi_veriler[7] = {0, 0, 0, 0, 0, 0, 0};
int yesil_veriler[7] = {0, 0, 0, 0, 0, 0, 0};
int noFilter_veriler[7] = {0, 0, 0, 0, 0, 0, 0};

int baslangicNoFilter = 0, dogru_sayac = 0, rakip_sayac = 0;
int bolge_bildirim = A2;
int sayac_sinyali = A3;
int ceza_sinyali = A4;
int kilit_bildirim = A5;

int bolge = 0;

void setup() {
  delay(500);
  arka_sol_kapak.attach(3);
  arka_sag_kapak.attach(5);
  ust_kapak.attach(6);

  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(sayac_sinyali, OUTPUT);
  pinMode(ceza_sinyali, OUTPUT);
  //sensör çıkış pininden bilgi alıyoruz
  pinMode(out, INPUT);
  pinMode(kilit_bildirim, INPUT);
  pinMode(bolge_bildirim, INPUT);

  if (digitalRead(bolge_bildirim))
  {
    bolge = 1;
  }

  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);
  digitalWrite(sayac_sinyali, LOW);
  digitalWrite(ceza_sinyali, LOW);

  Serial.begin(9600);

  baslangic();

  delay(2000);
  for (int i = 0; i < 7; i++)
  {
    olcum();
    kirmizi_veriler[i] = kirmizi;
    mavi_veriler[i] = mavi;
    yesil_veriler[i] = yesil;
    noFilter_veriler[i] = noFilter;
  }

  dogru_veri_al();

  baslangicNoFilter = noFilter;
}

void loop()
{
  //verileri güncelliyoruz
  olcum();

  //verileri 1 geri kaydırıyoruz
  for (int i = 0; i < 6; i++)
  {
    kirmizi_veriler[i] = kirmizi_veriler[i + 1];
    mavi_veriler[i] = mavi_veriler[i + 1];
    yesil_veriler[i] = yesil_veriler[i + 1];
    noFilter_veriler[i] = noFilter_veriler[i + 1];
  }

  //en son gelen veriyi son elemanlara yolluyoruz
  kirmizi_veriler[6] = kirmizi;
  mavi_veriler[6] = mavi;
  yesil_veriler[6] = yesil;
  noFilter_veriler[6] = noFilter;

  //bütün diziler içindeki en çok tekrar eden değerler benim kirmizi, mavi, yesil ve noFilter değişkenlerime atanır
  dogru_veri_al();

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


  if ( yuzde_sorgu(mavi, kirmizi, 250, 400, "m/k: ") && top_var_mi() )
  {
    //KIRMIZI TOP ALGILADIK
    Serial.println("Kırmızı");

    if (bolge) //BÖLGEMİZ KIRMIZI
    {
      dogru_al();
      dogru_sayac++;
    }
    else //BÖLGEMİZ MAVİ
    {
      rakip_al();
    }

    //eğer sayac dolmuşsa sinyal gönder
    //while'a girilecek mega işini bitirip sinyal gönderene kadar bu while'dan çıkılamayacak
    if (dogru_sayac == 3)
    {
      digitalWrite(sayac_sinyali, HIGH);
      delay(3000);
      while (digitalRead(kilit_bildirim) == 1)
      {

      }
      dogru_sayac = 0;
    }

    //renk sensörü hala kırmızı algılamasın diye verileri güncelliyoruz
    for (int i = 1; i < 7; i++) {
      olcum();
      kirmizi_veriler[i] = kirmizi;
      mavi_veriler[i] = mavi;
      yesil_veriler[i] = yesil;
      noFilter_veriler[i] = noFilter;
      delay(40);
    }

  }
  else if ( (kirmizi - mavi) > 20 && top_var_mi() )
  {
    //MAVİ TOP ALGILADIK
    Serial.println("Mavi");

    if (bolge) //BÖLGEMİZ KIRMIZI
    {
      rakip_al();
    }
    else //BÖLGEMİZ MAVİ
    {
      dogru_al();
      dogru_sayac++;
    }

    if (dogru_sayac == 3)
    {
      digitalWrite(sayac_sinyali, HIGH);
      delay(3000);
      while (digitalRead(kilit_bildirim) == 1)
      {

      }
      dogru_sayac = 0;
    }

    //renk sensörü hala mavi algılamasın diye verileri güncelliyoruz
    for (int i = 1; i < 7; i++) {
      olcum();
      kirmizi_veriler[i] = kirmizi;
      mavi_veriler[i] = mavi;
      yesil_veriler[i] = yesil;
      noFilter_veriler[i] = noFilter;
      delay(40);
    }

  }
  else if ( yuzde_sorgu(mavi, kirmizi, 125, 140, "m/k: ") && top_var_mi() )
  {
    //CEZA YUMURTASI ALGILADIK
    Serial.println("Ceza");

    ceza_al();

    //direkt megaya sinyal gönder ve tekrar while'a girilir
    digitalWrite(ceza_sinyali, HIGH);
    delay(3000);
    while (digitalRead(kilit_bildirim) == 1)
    {

    }

    //renk sensörü hala ceza algılamasın diye verileri güncelliyoruz
    for (int i = 1; i < 7; i++) {
      olcum();
      kirmizi_veriler[i] = kirmizi;
      mavi_veriler[i] = mavi;
      yesil_veriler[i] = yesil;
      noFilter_veriler[i] = noFilter;
      delay(40);
    }
  }
  else
  {
    Serial.println("Boşş");
  }

  delay(20);

}

void olcum()
{
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
}

void dogru_veri_al() {
  kirmizi = en_cok_tekrar_eden(kirmizi_veriler);
  mavi = en_cok_tekrar_eden(mavi_veriler);
  yesil = en_cok_tekrar_eden(yesil_veriler);
  noFilter = en_cok_tekrar_eden(noFilter_veriler);
}

int en_cok_tekrar_eden(int dizi[7])
{
  //en çok sayılan değerin ne kadar sayıldığını tutmak için
  int maxSayac = 0;
  //işin sonunda geri döndüreceğimiz en çok tekrar edecek sayıyı tutacak değişken
  int sayi = dizi[0];

  for (int i = 0; i < 7; i++)
  {
    //sayının ne kadar tekrar ettiğini tutucaz
    int sayac = 0;

    //i'deki elemanı kendi dahil tüm elemanlarla karşılaştırıyoruz
    for (int j = 0; j < 7; j++)
    {
      if (dizi[i] == dizi[j])
      {
        //eğer eşitlik varsa sayac 1 artırılır
        sayac++;
      }
    }

    //eğer sayac maxSayacı aşmışsa şu ana kadar en çok sayılmış eleman demektir
    if (sayac > maxSayac)
    {
      //en çok sayılan elemanı sayının içine atıyoruz
      sayi = dizi[i];
      maxSayac = sayac;
    }
  }

  //sayıyı döndürüyoruz
  return sayi;
}

void baslangic()
{
  arka_sol_kapak.write(25);
  arka_sag_kapak.write(140);
  ust_kapak.write(11);
}

void rakip_al()
{
  arka_sag_kapak.write(90);
  arka_sol_kapak.write(10);
  delay(100);
  ust_kapak.write(60);
  delay(250);
  ust_kapak.write(0);
  delay(100);
  arka_sag_kapak.write(180);
  delay(200);
  arka_sol_kapak.write(25);
  arka_sag_kapak.write(140);

}

void dogru_al()
{
  arka_sag_kapak.write(180);
  arka_sol_kapak.write(0);
  delay(100);
  ust_kapak.write(60);
  delay(250);
  ust_kapak.write(0);
  delay(200);
  arka_sol_kapak.write(25);
  arka_sag_kapak.write(140);
}

void ceza_al()
{
  arka_sol_kapak.write(70);
  arka_sag_kapak.write(170);
  delay(100);
  ust_kapak.write(60);
  delay(250);
  ust_kapak.write(0);
  delay(100);
  arka_sol_kapak.write(0);
  delay(200);
  arka_sol_kapak.write(25);
  arka_sag_kapak.write(140);
}

boolean aralik_sorgu(int kk, int kb, int mk, int mb, int yk, int yb, int fk, int fb) {
  return (
           (kirmizi >= kk) && (kirmizi <= kb)
           && (mavi >= mk) && (mavi <= mb)
           && (yesil >= yk) && (yesil <= yb)
           && (noFilter >= fk) && (noFilter <= fb) );
}

boolean yuzde_sorgu(int buyukDegisken, int kucukDegisken, int kucuk, int buyuk, String mesaj) {
  float yuzde = ((float)buyukDegisken / (float)kucukDegisken) * 100;
  Serial.print(mesaj);
  Serial.println(yuzde);
  return (yuzde >= kucuk) && (yuzde <= buyuk);
}

boolean top_var_mi()
{
  float yuzde = ((float)noFilter / (float)baslangicNoFilter) * 100;
  Serial.print("filtre oranı: ");
  Serial.println(yuzde);
  return (yuzde > 150);
}

boolean ceza_sorgu() {
  float yuzde_yesil_noFilter =  ( (float)yesil / (float)noFilter ) * 100;
  float yuzde_yesil_kirmizi =  ( (float)yesil / (float)kirmizi ) * 100;
  float yuzde_yesil_mavi =  ( (float)yesil / (float)mavi ) * 100;
  return (yuzde_yesil_noFilter >= 135) && (yuzde_yesil_noFilter <= 150) && (yuzde_yesil_kirmizi >= 165) && (yuzde_yesil_mavi >= 125);
}
