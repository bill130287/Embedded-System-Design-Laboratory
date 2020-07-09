# 題目功能
   1. 開機後OLED顯示器全熄滅1秒
   2. 以間隔0.1秒速度讀取MPU9250，計算後顯示在OLED顯示 器上，同時COM Port送出“Temperature=xx.x ,
      G:xx.x yy.y zz.z , A:xx.x yy.y zz.z , M:xx.x yy.y zz.z \r\n”字串
      (xx.x為溫度(°C)讀值，G為陀螺儀(rad/s)讀值，A為加速度計(m/s2)讀值，M為磁力儀(uT)讀值
   3. 移動MPU9250，檢查讀值是否有正確
   4. OLED顯示內容      "NTUST 
                    Temperature=xx.x
                    G:xx.x yy.y zz.z
                    A:xx.x yy.y zz.z
                    M:xx.x yy.y zz.z"
# GPIO Define
   1. MPU9250插在I2C位置
   2. OLED顯示器插在I2C位置
      I2C0_CLK -> GPIO 27
      I2C0_DATA -> GPIO 28

