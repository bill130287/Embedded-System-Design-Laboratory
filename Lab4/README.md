# 題目功能
   1. 開機後OLED顯示器全熄滅
   2. 以2Hz速度做超音波測距功能，計算後顯示在OLED顯示器上，同時COM Port送出"Distance = xxx.x \r\n"字串
      (xxx.x為測距後公分讀值，解析度0.1公
   3. 在超音波測距感應器前移動物體，檢查測距值是否有正確
   4. OLED顯示內容      "NTUST
                    Distance = xxx.x"
# GPIO Define
   1. 超音波測距感應器插在擴充板D2位置
      SIG pin -> GPIO 0
   2. OLED顯示器插在I2C位置
      I2C0_CLK -> GPIO 27
      I2C0_CLK -> GPIO 28
