#include <Servo.h>

Servo arka_sol_kapak;
Servo arka_sag_kapak;
Servo ust_kapak;

#define out 7 // beyaz
#define s0 8  // sarı
#define s1 9  // turuncu
#define s2 10 // yeşil 
#define s3 11 // mavi

int kirmizi, yesil, mavi, noFilter;

int enKucukKirmizi = 50, enBuyukKirmizi = 50, 
    enKucukYesil = 50, enBuyukYesil = 50, 
    enKucukMavi = 50, enBuyukMavi = 50,
    enKucukFiltresiz = 50, enBuyukFiltresiz = 50;
 
int kirmizi_veriler[7] = {0,0,0,0,0,0,0};
int mavi_veriler[7] = {0,0,0,0,0,0,0}; 
int yesil_veriler[7] = {0,0,0,0,0,0,0}; 
int noFilter_veriler[7] = {0,0,0,0,0,0,0};

int baslangicNoFilter = 0;

void setup() {
  arka_sol_kapak.attach(3); 
  arka_sag_kapak.attach(5);
  ust_kapak.attach(6);

  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  //sensör çıkış pininden bilgi alıyoruz
  pinMode(out, INPUT);

  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);

  Serial.begin(9600);

  baslangic();

  delay(2000);
  for(int i = 0; i < 7; i++)
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
  
  olcum();
  //verileri güncelliyoruz

  //verileri 1 geri kaydırıyoruz
  for(int i = 0; i < 6; i++)
  {
    kirmizi_veriler[i] = kirmizi_veriler[i+1];
    mavi_veriler[i] = mavi_veriler[i+1];
    yesil_veriler[i] = yesil_veriler[i+1];
    noFilter_veriler[i] = noFilter_veriler[i+1];
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

  
  if ( yuzdeSorgu(mavi, kirmizi, 195, 260, "mavi / kirmizi = ") && top_var_mi() ) 
  {
    //KIRMIZI TOP ALGILADIK
    Serial.println("Kırmızı");
    
    dogru_al();
    //sayac arttır
    //eğer sayac dolmuşsa sinyal gönder
    //while'a girilecek mega işini bitirip sinyal gönderene kadar bu while'dan çıkılamayacak

    //renk sensörü hala kırmızı algılamasın diye verileri güncelliyoruz
    for(int i = 1; i < 7; i++){
      olcum();
      kirmizi_veriler[i] = kirmizi;
      mavi_veriler[i] = mavi;
      yesil_veriler[i] = yesil;
      noFilter_veriler[i] = noFilter;
      delay(40);
    }
    
  } 
  else if ( yuzdeSorgu(kirmizi, mavi, 170, 210, "kirmizi / mavi = ") && top_var_mi() ) 
  {
    //MAVİ TOP ALGILADIK
    Serial.println("Mavi");
    
    rakip_al();

    //renk sensörü hala mavi algılamasın diye verileri güncelliyoruz
    for(int i = 1; i < 7; i++){
      olcum();
      kirmizi_veriler[i] = kirmizi;
      mavi_veriler[i] = mavi;
      yesil_veriler[i] = yesil;
      noFilter_veriler[i] = noFilter;
      delay(40);
    }
  } 
  else if ( yuzdeSorgu(mavi, kirmizi, 80, 170, "mavi / kirmizi = ") && top_var_mi() ) 
  {
    //CEZA YUMURTASI ALGILADIK
    Serial.println("Ceza");
    ceza_al();
    //direkt megaya sinyal gönder ve terkrar while'a girilir

    //renk sensörü hala ceza algılamasın diye verileri güncelliyoruz
    for(int i = 1; i < 7; i++){
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

  delay(300);
  
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
  digitalWrite(s2,LOW);
  digitalWrite(s3,LOW);
  delay(50);

  kirmizi = pulseIn(out, LOW);

  Serial.print("Kırmızı: ");
  Serial.print(kirmizi);
  Serial.print("\t");

  //maviyi okuyoruz
  digitalWrite(s2,LOW);
  digitalWrite(s3,HIGH);
  delay(50);

  mavi = pulseIn(out, LOW);

  Serial.print("Mavi: ");
  Serial.print(mavi);
  Serial.print("\t");

  //yeşili okuyoruz
  digitalWrite(s2,HIGH);
  digitalWrite(s3,HIGH);
  delay(50);

  yesil = pulseIn(out, LOW);

  Serial.print("Yeşil: ");
  Serial.print(yesil);
  Serial.print("\t");

  //filtresiz okuyoruz
  digitalWrite(s2,HIGH);
  digitalWrite(s3,LOW);
  delay(50);

  noFilter = pulseIn(out, LOW);

  Serial.print("Filtresiz: ");
  Serial.println(noFilter);

  if(kirmizi < enKucukKirmizi)
  {
    enKucukKirmizi = kirmizi;
  }
  if(kirmizi > enBuyukKirmizi)
  {
    enBuyukKirmizi = kirmizi;
  }
  if(mavi < enKucukMavi)
  {
    enKucukMavi = mavi;
  }
  if(mavi > enBuyukMavi)
  {
    enBuyukMavi = mavi;
  }
  if(yesil < enKucukYesil)
  {
    enKucukYesil = yesil;
  }
  if(yesil > enBuyukYesil)
  {
    enBuyukYesil = yesil;
  }
  if(noFilter < enKucukFiltresiz)
  {
    enKucukFiltresiz = noFilter;
  }
  if(noFilter > enBuyukFiltresiz)
  {
    enBuyukFiltresiz = noFilter;
  }
}

void olcum()
{
  digitalWrite(s2,LOW);
  digitalWrite(s3,LOW);
  delay(10);

  kirmizi = pulseIn(out, LOW);
  //kirmizi = map(kirmizi, 24, 354, 0, 100);


  //maviyi okuyoruz
  digitalWrite(s2,LOW);
  digitalWrite(s3,HIGH);
  delay(10);

  mavi = pulseIn(out, LOW);
  //mavi = map(mavi, 22, 323, 0, 100);
 
  
  //yeşili okuyoruz
  digitalWrite(s2,HIGH);
  digitalWrite(s3,HIGH);
  delay(10);

  yesil = pulseIn(out, LOW);
  //yesil = map(yesil, 23, 405, 0, 100);

  
  //Filtresiz okuyoruz
  digitalWrite(s2,HIGH);
  digitalWrite(s3,LOW);
  delay(10);

  noFilter = pulseIn(out, LOW);
  //noFilter = map(noFilter, 8, 122, 0, 100);

/*
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
  Serial.println(noFilter);*/
  
}

void dogru_veri_al(){
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

  for(int i = 0; i < 7; i++)
  {
    //sayının ne kadar tekrar ettiğini tutucaz
    int sayac = 0;

    //i'deki elemanı kendi dahil tüm elemanlarla karşılaştırıyoruz
    for(int j = 0; j < 7; j++)
    {
      if(dizi[i] == dizi[j])
      {
        //eğer eşitlik varsa sayac 1 artırılır
        sayac++;
      }
    }

    //eğer sayac maxSayacı aşmışsa şu ana kadar en çok sayılmış eleman demektir
    if(sayac > maxSayac)
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

boolean yuzdeSorgu(int buyukDegisken, int kucukDegisken, int kucuk, int buyuk, String mesaj){
  float yuzde = ((float)buyukDegisken / (float)kucukDegisken)*100;
  Serial.print(mesaj);
  Serial.println(yuzde);
  return (yuzde >= kucuk) && (yuzde <= buyuk);
}

boolean top_var_mi()
{
  float yuzde = ((float)noFilter / (float)baslangicNoFilter)*100;
  Serial.print("filtre oranı: ");
  Serial.println(yuzde);
  return (yuzde >= 120) && (yuzde <= 220);
}

boolean ceza_sorgu(){
  float yuzde_yesil_noFilter =  ( (float)yesil / (float)noFilter ) * 100;
  float yuzde_yesil_kirmizi =  ( (float)yesil / (float)kirmizi ) * 100;
  float yuzde_yesil_mavi =  ( (float)yesil / (float)mavi ) * 100;
  return (yuzde_yesil_noFilter >= 135) && (yuzde_yesil_noFilter <= 150) && (yuzde_yesil_kirmizi >= 165) && (yuzde_yesil_mavi >= 125);
}
