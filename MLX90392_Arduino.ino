/**
 * @copyright (C) 2021 Melexis <http://www.melexis.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 * Rev2.0: RequestFrom return can be used to check for Nack.
 */


#include <Wire.h>

char Cmd = '0';       // Char received from the PC.
int Nack = 0;         // >0 If the IC did not send an Ack.
int Nbytes = 0;       // Number of bytes to read from IC.
int Flag = 0;         // Flag to indicate termination of burst writing to the IC.
byte Register = 0;    // Register to read from or write to.
long freq = 100000;   // I2C Frequency, in Hz.

void setup()
{
  Wire.begin();
  Wire.setClock(freq);
  Serial.begin(9600);
}

void loop()
{
  if (Serial.available() > 0) {
    Cmd = Serial.read();
  }
  switch (Cmd) {
    case 'A':
      // Read Nbytes (if 0, then 16 bytes), starting from the specified Register.
      while (!Serial.available()) {}
      Register = hex2dec(Serial.read());
      while (!Serial.available()) {}
      Register = 16 * Register + hex2dec(Serial.read());
      while (!Serial.available()) {}
      Nbytes = hex2dec(Serial.read());
      if (Nbytes == 0) {
        Nbytes = 16;
      }
      Wire.beginTransmission(0x0C);
      Wire.write(Register);
      Nack = Wire.endTransmission(false);
      Nack = Nack + (Nbytes - Wire.requestFrom(0x0C, Nbytes, true));
      Serial.print(Nack);
      Serial.print(";");
      while (Nbytes > 0) {
        Serial.print(Wire.read());
        Serial.print(";");
        Nbytes--;
      }
      Serial.print("\n");
      Cmd = '0';
      break;
    case 'B':
      // Read direct Nbytes (if 0, then 16 bytes).
      while (!Serial.available()) {}
      Nbytes = hex2dec(Serial.read());
      if (Nbytes == 0) {
        Nbytes = 16;
      }
      Nack = (Nbytes - Wire.requestFrom(0x0C, Nbytes, true));
      Serial.print(Nack);
      Serial.print(";");
      while (Nbytes > 0) {
        Serial.print(Wire.read());
        Serial.print(";");
        Nbytes--;
      }
      Serial.print("\n");
      Cmd = '0';
      break;
    case 'D':
      // Addressed reset.
      while (!Serial.available()) {}
      Register = hex2dec(Serial.read());
      while (!Serial.available()) {}
      Register = 16 * Register + hex2dec(Serial.read());
      Wire.beginTransmission(Register);
      Wire.write(byte(0x11));
      Wire.write(byte(0x06));
      Nack = Wire.endTransmission(true);
      Serial.print(Nack);
      Serial.print("\n");
      Cmd = '0';
      break;
    case 'E':
      // Write consecutive bytes into the registers of the IC, starting from the specified Register.
      while (!Serial.available()) {}
      Register = hex2dec(Serial.read());
      while (!Serial.available()) {}
      Register = 16 * Register + hex2dec(Serial.read());
      Wire.beginTransmission(0x0C);
      Wire.write(Register);
      // Write byte per byte, checking each time if writing should be terminated (reception of char 'T').
      Flag = 0;
      while (Flag == 0) {
        while (!Serial.available()) {}
        Cmd = Serial.read();
        if (Cmd == 'T') {
          Flag = 1;
        } else {
          while (!Serial.available()) {}
          Wire.write(byte(16 * hex2dec(Cmd) + hex2dec(Serial.read())));
        }
      }
      Nack = Wire.endTransmission(true);
      Serial.print(Nack);
      Serial.print("\n");
      Cmd = '0';
      break;
    case 'W':
      // Change the I2C frequency.
      while (!Serial.available()) {}
      freq = hex2dec(Serial.read());
      while (!Serial.available()) {}
      freq = 10 * freq + hex2dec(Serial.read());
      while (!Serial.available()) {}
      freq = 10 * freq + hex2dec(Serial.read());
      while (!Serial.available()) {}
      freq = 10 * freq + hex2dec(Serial.read());
      while (!Serial.available()) {}
      freq = 10 * freq + hex2dec(Serial.read());
      while (!Serial.available()) {}
      freq = 10 * freq + hex2dec(Serial.read());
      while (!Serial.available()) {}
      freq = 10 * freq + hex2dec(Serial.read());
      Wire.setClock(freq);
      Serial.print("I2C freq now: ");
      Serial.print(freq);
      Serial.print(" Hz\n");
      Cmd = '0';
      break;
    default:
      Cmd = '0';
      break;
  }
}

int hex2dec(char input)
// Convert the hexadecimal ASCII input to the integer it represents.
{
  int result = 0;
  if (input >= 48 && input <= 57) {
    result = (int) input - 48;
  }
  if (input >= 65 && input <= 70) {
    result = (int) input - 55;
  }
  return result;
}
