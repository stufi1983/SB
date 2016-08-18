//IC4094
//Pin 8 connected  Strobe (pin 1)
//Pin 9 connected  Data (pin 2)
//Pin 10 connected  Clock (pin 3)
/*
#define strobePin 8
#define dataPin  9
#define clockPin  10
byte segChar[] = {0x04, 0x2f, 0x18, 0x09, 0x23, 0x41, 0x40, 0x0f, 0x00, 0x01,0x7b,0x7f};
#define STARTDIGIT digitalWrite(strobePin, LOW);
#define ENDDIGIT digitalWrite(strobePin, HIGH);

#define strobePinThn 11
#define dataPinThn  12
#define clockPinThn  13
byte segCharThn[] = {0x7b,0x12,0x67,0x37,0x1e,0x3d,0x7d,0x13,0x7f,0x3f,0x04,0x00};
#define STARTDIGITTHN digitalWrite(strobePinThn, LOW);
#define ENDDIGITTHN digitalWrite(strobePinThn, HIGH);
*/

#define strobePinTgl 3
#define dataPinTgl  4
#define clockPinTgl  5
byte segCharTgl[] = {0x01,0x37,0x42,0x12,0x34,0x18,0x08,0x33,0x00,0x10,0xff,0x00};
#define STARTDIGITTGL digitalWrite(strobePinTgl, LOW);
#define ENDDIGITTGL digitalWrite(strobePinTgl, HIGH);


void enable4094(){
/*  pinMode(strobePin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(strobePinThn, OUTPUT);
  pinMode(clockPinThn, OUTPUT);
  pinMode(dataPinThn, OUTPUT);*/
  pinMode(strobePinTgl, OUTPUT);
  pinMode(clockPinTgl, OUTPUT);
  pinMode(dataPinTgl, OUTPUT);
}

/*
void displayDigitThn(byte digit) {
  shiftOut(dataPinThn, clockPinThn, MSBFIRST, segCharThn[digit]);
}
void displayJamMenitThn(byte jam, byte menit, byte detik) {
  displayDigitThn(detik % 10);tanggalJam[8]=detik % 10;
  displayDigitThn(detik / 10);tanggalJam[9]=detik / 10;
  displayDigitThn(menit % 10);tanggalJam[10]=menit % 10;
  displayDigitThn(menit / 10);tanggalJam[11]=menit / 10;
  displayDigitThn(jam % 10);tanggalJam[12]=jam % 10;
  displayDigitThn(jam / 10); tanggalJam[13]=jam / 10;
}
void displayTglBulanTahun(byte tgl, byte bln, byte tahun) {
  displayDigitThn(tahun % 10);tanggalJam[0]=tahun % 10;
  displayDigitThn(tahun / 10);tanggalJam[1]=tahun / 10;
  displayDigitThn(0);tanggalJam[2]=0;
  displayDigitThn(2);tanggalJam[3]=2;
  displayDigitThn(bln % 10); tanggalJam[4]=bln % 10;
  displayDigitThn(bln / 10); tanggalJam[5]=bln / 10;
  displayDigitThn(tgl % 10); tanggalJam[6]=tgl % 10;
  displayDigitThn(tgl / 10); tanggalJam[7]=tgl / 10;
}


void displayDigit(byte digit) {
  shiftOut(dataPin, clockPin, MSBFIRST, segChar[digit]);
}
void displayJamMenit(byte jam, byte menit) {
  int angka = jam * 100 + menit;
  displayDigit(menit % 10);
  displayDigit(menit / 10);
  displayDigit(jam % 10);
  displayDigit(jam / 10);
}
void displayMenitDetik(byte menit, byte detik) {
  displayJamMenit(menit, detik);
}
*/
void displayDigitTgl(byte digit) {
  shiftOut(dataPinTgl, clockPinTgl, MSBFIRST, segCharTgl[digit]);
}
void displayJamMenitTgl(byte jam, byte menit, byte detik) {
  displayDigitTgl(detik % 10);JamMenitTgl[0]=detik % 10;
  displayDigitTgl(detik / 10);JamMenitTgl[1]=detik / 10;
  displayDigitTgl(menit % 10);JamMenitTgl[2]=menit % 10;
  displayDigitTgl(menit / 10);JamMenitTgl[3]=menit / 10;
  displayDigitTgl(jam % 10);JamMenitTgl[4]=jam % 10;
  displayDigitTgl(jam / 10); JamMenitTgl[5]=jam / 10;
}
