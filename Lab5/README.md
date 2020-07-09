# 題目功能
   1. 開機後OLED顯示器全熄滅
   2. 以間隔2秒速度讀取AM2302溫濕度感應器，計算後顯示在OLED顯示器上，同時COM Port送出
      "Temperature=xx.x , Humidity = yy.y \r\n"字串。
      (xx.x為溫度讀值，yy.y為濕度)
   3. 對溫濕度感應器吹氣，檢查讀值是否有正確
   4. OLED顯示內容      "NTUST
                    Temperature=xx.x
                    Humidity = yy.y"
# GPIO Define
   1. 溫溼度感應器(AM2302)插在擴充板D2位置
      SIG pin -> GPIO 0
   2. OLED顯示器插在I2C位置
      I2C0_CLK -> GPIO 27
      I2C0_DATA -> GPIO 28
