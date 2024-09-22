#include <Servo.h>

Servo arka_sol_kapak;
Servo arka_sag_kapak;
Servo ust_kapak;

#define out 8
#define s0 9
#define s1 10
#define s2 11
#define s3 12

int kirmizi, mavi, noFilter;
int kirmizi_veriler[5] = { 0, 0, 0, 0, 0 };
int mavi_veriler[5] = { 0, 0, 0, 0, 0 };

int dogru_sayac = 0, rakip_sayac = 0;
int bolge_bildirim = A2;
int sayac_sinyali = A3;
int ceza_sinyali = A4;
int kilit_bildirim = A5;
int top_sensoru = 13;

int bolge = 0;

void setup() {
  delay(2000);
  arka_sol_kapak.attach(3);
  arka_sag_kapak.attach(5);
  ust_kapak.attach(6);

  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(sayac_sinyali, OUTPUT);
  pinMode(ceza_sinyali, OUTPUT);
  pinMode(top_sensoru, INPUT);
  pinMode(out, INPUT);
  pinMode(kilit_bildirim, INPUT);
  pinMode(bolge_bildirim, INPUT);

  //MEGADAN GELEN BÖLGE BİLDİRİMİ
  if (digitalRead(bolge_bildirim) == 1) {
    bolge = 1;
  }

  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);
  digitalWrite(sayac_sinyali, LOW);
  digitalWrite(ceza_sinyali, LOW);

  Serial.begin(9600);

  //SERVOLAR AYARLANIYOR
  baslangic();

  for (int i = 0; i < 5; i++) {
    olcum();
    kirmizi_veriler[i] = kirmizi;
    mavi_veriler[i] = mavi;
  }
}

void loop() {
  //verileri güncelliyoruz
  for (int i = 0; i < 5; i++) {
    olcum();
    kirmizi_veriler[i] = kirmizi;
    mavi_veriler[i] = mavi;
    delay(40);
  }

  kirmizi = stabilSonucuBul(kirmizi_veriler, 5);
  mavi = stabilSonucuBul(mavi_veriler, 5);

  Serial.print("KIRMIZI: ");
  Serial.print(kirmizi);
  Serial.print(" ");

  Serial.print("MAVİ: ");
  Serial.print(mavi);
  Serial.print(" ");

  Serial.print("Top var mı: ");
  Serial.println(digitalRead(top_sensoru));

  if (yuzde_sorgu(mavi, kirmizi, 160, 400, "m/k: ") && top_var_mi())  //KIRMIZI İÇİN
  {
    //EMİN OLMA ÖLÇÜMÜ
    for (int i = 0; i < 5; i++) {
      olcum();
      kirmizi_veriler[i] = kirmizi;
      mavi_veriler[i] = mavi;
    }

    kirmizi = stabilSonucuBul(kirmizi_veriler, 5);
    mavi = stabilSonucuBul(mavi_veriler, 5);

    if (yuzde_sorgu(mavi, kirmizi, 160, 400, "m/k: ") && top_var_mi()) {
      //KIRMIZI TOP ALGILADIK
      Serial.println("Kırmızı");

      if (bolge)  //BÖLGEMİZ KIRMIZI
      {
        dogru_al();
        dogru_sayac++;
      } else  //BÖLGEMİZ MAVİ
      {
        rakip_al();
      }

      //eğer sayac dolmuşsa sinyal gönder
      //while'a girilecek mega işini bitirip sinyal gönderene kadar bu while'dan çıkılamayacak
      sayac_kitlenmesi();

      //renk sensörü hala kırmızı algılamasın diye verileri güncelliyoruz
      for (int i = 1; i < 7; i++) {
        olcum();
        kirmizi_veriler[i] = kirmizi;
        mavi_veriler[i] = mavi;
        delay(40);
      }
    }

  } else if (yuzde_sorgu(mavi, kirmizi, 30, 70, ""))  //MAVİ İÇİN
  {
    //EMİN OLMA ÖLÇÜMÜ
    for (int i = 0; i < 5; i++) {
      olcum();
      kirmizi_veriler[i] = kirmizi;
      mavi_veriler[i] = mavi;
    }

    kirmizi = stabilSonucuBul(kirmizi_veriler, 5);
    mavi = stabilSonucuBul(mavi_veriler, 5);

    if (yuzde_sorgu(mavi, kirmizi, 30, 70, "")) {
      //MAVİ TOP ALGILADIK
      Serial.println("Mavi");

      if (bolge)  //BÖLGEMİZ KIRMIZI
      {
        rakip_al();
      } else  //BÖLGEMİZ MAVİ
      {
        dogru_al();
        dogru_sayac++;
      }

      sayac_kitlenmesi();

      //renk sensörü hala mavi algılamasın diye verileri güncelliyoruz
      for (int i = 1; i < 7; i++) {
        olcum();
        kirmizi_veriler[i] = kirmizi;
        mavi_veriler[i] = mavi;
        delay(40);
      }
    }

  } else if (top_var_mi())  //CEZA İÇİN
  {
    //EMİN OLMA ÖLÇÜMÜ
    for (int i = 0; i < 5; i++) {
      olcum();
      kirmizi_veriler[i] = kirmizi;
      mavi_veriler[i] = mavi;
    }

    kirmizi = stabilSonucuBul(kirmizi_veriler, 5);
    mavi = stabilSonucuBul(mavi_veriler, 5);

    if (!yuzde_sorgu(mavi, kirmizi, 160, 400, "") && !yuzde_sorgu(mavi, kirmizi, 30, 80, "") && top_var_mi()) {
      //CEZA YUMURTASI ALGILADIK
      Serial.println("Ceza");

      ceza_al();

      //direkt megaya sinyal gönder ve tekrar while'a girilir
      digitalWrite(ceza_sinyali, HIGH);
      delay(10000);
      while (digitalRead(kilit_bildirim) == 1) {
      }
      digitalWrite(ceza_sinyali, LOW);

      //renk sensörü hala ceza algılamasın diye verileri güncelliyoruz
      for (int i = 1; i < 7; i++) {
        olcum();
        kirmizi_veriler[i] = kirmizi;
        mavi_veriler[i] = mavi;
        delay(40);
      }
    }

  } else {
    Serial.println("Boşş");
  }

  delay(300);
}

void olcum() {
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  delay(20);

  kirmizi = pulseIn(out, LOW);

  //maviyi okuyoruz
  digitalWrite(s2, LOW);
  digitalWrite(s3, HIGH);
  delay(20);

  mavi = pulseIn(out, LOW);
}

void baslangic() {
  arka_sol_kapak.write(25);
  arka_sag_kapak.write(140);
  ust_kapak.write(11);
}

void rakip_al() {
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

void dogru_al() {
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

void ceza_al() {
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

void sayac_kitlenmesi() {
  if (dogru_sayac == 3) {
    digitalWrite(sayac_sinyali, HIGH);
    delay(10000);
    while (digitalRead(kilit_bildirim) == 1) 
    {
    }
    dogru_sayac = 0;
    digitalWrite(sayac_sinyali, LOW);
  }
}

boolean yuzde_sorgu(int buyukDegisken, int kucukDegisken, int kucuk, int buyuk, String mesaj) {
  float yuzde = ((float)buyukDegisken / (float)kucukDegisken) * 100;
  Serial.print(mesaj);
  Serial.println(yuzde);
  return (yuzde >= kucuk) && (yuzde <= buyuk);
}

boolean top_var_mi() {
  return (digitalRead(top_sensoru) == 0);
}





// Ortalama hesaplayan fonksiyon
double ortalamaHesapla(const int arr[], int size) {
  double sum = 0;
  for (int i = 0; i < size; i++) {
    sum += arr[i];
  }
  return sum / size;
}

// Standart sapmayı hesaplayan fonksiyon
double standartSapmaHesapla(const int arr[], int size, double mean) {
  double sum = 0;
  for (int i = 0; i < size; i++) {
    sum += pow(arr[i] - mean, 2);
  }
  return sqrt(sum / size);
}

// Uç değerleri filtreleyerek en istikrarlı değeri bul
int stabilSonucuBul(const int arr[], int size) {
  // Dizinin ortalamasını hesapla
  double mean = ortalamaHesapla(arr, size);

  // Dizinin standart sapmasını hesapla
  double stdDev = standartSapmaHesapla(arr, size, mean);

  // Standart sapmanın 1.5 katından daha uzak olan değerleri göz ardı et
  const double threshold = 1.5 * stdDev;

  // Filtrelenmiş dizinin ortalamasını ve en yakın değeri bul
  double filteredSum = 0;
  int filteredCount = 0;
  int closestValue = arr[0];
  double minDifference = 1e6;  // çok büyük bir sayı kullanıyoruz

  for (int i = 0; i < size; i++) {
    if (fabs(arr[i] - mean) <= threshold) {
      filteredSum += arr[i];
      filteredCount++;

      // Ortalamaya en yakın değeri bul
      double diff = fabs(arr[i] - mean);
      if (diff < minDifference) {
        minDifference = diff;
        closestValue = arr[i];
      }
    }
  }

  return closestValue;
}
