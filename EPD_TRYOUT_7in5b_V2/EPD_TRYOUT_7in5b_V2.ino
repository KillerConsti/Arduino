/**
 *  @filename   :   epd7in5b_V2-demo.ino
 *  @brief      :   7.5inch b V2 e-paper display demo
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare      Nov 30 2020
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <SPI.h>
#include "epd7in5b_V2.h"
#include "imagedata.h"

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println("try to init");
    Epd epd;
    if (epd.Init() != 0) {
        Serial.print("e-Paper init failed");
        return;
    }
    Serial.println("e-Paper init success");
    //Epd::Displaypart(const unsigned char* pbuffer, unsigned long xStart,         unsigned long yStart,unsigned long Picture_Width,  unsigned long Picture_Height, unsigned char Block)
    epd.Displaypart(IMAGE_DATA,0, 100,240,103, 0);
    epd.Displaypart(IMAGE_DATA,250, 300,240,103, 1);
    Serial.print("image shall be there");
    delay(10000);
    epd.Sleep();
}

void loop() {
  // put your main code here, to run repeatedly:

}
