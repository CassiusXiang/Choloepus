#include <Arduino.h>

void setup()
{
    // 初始化串口通信
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, 18, 17); // RX=17, TX=18

    // 等待串口连接（对于某些板子，如 Arduino Leonardo 或其他基于 USB 的串口）
    while (!Serial)
    {
        ; // 等待串口连接
    }
}

void loop()
{
    // 从 Serial1 读取数据并发送到 Serial
    while (Serial1.available())
    {
        int data = Serial1.read();
        Serial.write(data);
    }

    // 从 Serial 读取数据并发送到 Serial1
    while (Serial.available())
    {
        int data = Serial.read();
        Serial1.write(data);
    }
}
