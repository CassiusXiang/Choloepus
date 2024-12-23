/**
 * For Flexible_jaws controller
 * by X.Chang@Version1.0
 * LAB_121
 * 参数笔记
 * 最大张角对应传感器机械角度 0rad
 * 最小张角对应传感器机械角度 90rad
 * @version: v2.1.2
 */

#include <SimpleFOC.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <cstring>
// #include <WiFi.h>

// 任务句柄
TaskHandle_t Task1;
TaskHandle_t Task2;
// I2C 磁编码器实例初始化
MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);

// 电机实例初始化
BLDCMotor motor = BLDCMotor(7);                         // 初始化一个motor实例，极对数为7
BLDCDriver3PWM driver = BLDCDriver3PWM(25, 26, 27, 33); // 绑定三相电机驱动的 PWM 引脚

// 引脚定义
// const int TX1 = 18;
// const int RX1 = 17;

// dirction
const int left_hand = 1;

// 角度设置变量
float target = 0; // 目标

// 命令解释器的初始化
Commander command = Commander(Serial);
void doTarget(char *cmd) { command.scalar(&target, cmd); }
void onMotor(char *cmd) { command.motor(&motor, cmd); }
// 自定义函数
int32_t decodeToTwosComplement(const std::vector<uint8_t> &byteList) // decode
{
    if (byteList.size() < 4)
    {
        std::cerr << "Error: Byte list must contain at least 4 bytes." << std::endl;
        return 0;
    }

    // 将字节序列直接解释为一个32位有符号整数
    int32_t number;
    memcpy(&number, byteList.data(), sizeof(number));

    return number;
}

std::vector<uint8_t> getSubVector(const std::vector<uint8_t> &originalVector, int start, int end)
{ // 用来分割vector
    // 请注意，end 是不包含在内的，所以实际上是到 end - 1 的元素
    return std::vector<uint8_t>(originalVector.begin() + start, originalVector.begin() + end);
}

std::vector<int32_t> getModeSpeed(const std::vector<uint8_t> &originalVector)
{
    std::vector<int32_t> res = {};
    std::vector<uint8_t> originalVector1 = getSubVector(originalVector, 8, 12);
    std::vector<uint8_t> originalVector2 = getSubVector(originalVector, 12, 16);
    int32_t mode = decodeToTwosComplement(originalVector1);
    int32_t speed = decodeToTwosComplement(originalVector2);
    res.push_back(mode);
    res.push_back(speed);

    return res;
}

bool checkId(const std::vector<uint8_t> &originalVector)
{
    if (originalVector[7] == 0x14)
    {
        return true;
    }
    else
        return false;
}

// 定义任务函数
void Task1code(void *pvParameters)
{ // FOC control
    // 引脚初始化

    for (;;)
    {
        // main FOC algorithm function
        // the faster you run this function the better
        // Arduino UNO loop  ~1kHz
        // Bluepill loop ~10kHz
        motor.loopFOC();

        // Motion control function
        // velocity, position or voltage (defined in motor.controller)
        // this function can be run at much lower frequency than loopFOC() function
        // You can also use motor.move() and set the motor.target in the code
        motor.move(left_hand * target);

        // function intended to be used with serial plotter to monitor motor variables
        // significantly slowing the execution down!!!!
        // motor.monitor();
        // if (motor.)

        // user communication
        // command.run();

        // vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void Task2code(void *pvParameters)
{
    vTaskDelay(1000 / portTICK_PERIOD_MS); // 等待主控制程序启动
    Serial.println("setting...");
    motor.controller = MotionControlType::torque;
    target = 1.4;
    vTaskDelay(5000 / portTICK_PERIOD_MS); // 初始化
    target = 0;
    motor.sensor_offset = -sensor.getAngle();
    Serial.println("setting success");

    int serp = 0;

    for (;;) // 命令解码器
    {
        if (Serial1.available() > 0)
        {
            std::vector<uint8_t> data;
            int mode, value;
            while (Serial1.available() > 0)
            {
                data.push_back(Serial1.read());
            }
            // Serial.write(Serial1.read());s
            // Serial.write(data[0]);
            // Serial.write(data[1]);
            // Serial.write(data[2]);
            // Serial.write(data[3]);
            // Serial.write(data[4]);
            // Serial.write(data[5]);
            // Serial.write(data[6]);
            // Serial.write(data[7]);
            // Serial.write(data[8]);
            // Serial.write(data[9]);
            // Serial.write(data[10]);
            // Serial.write(data[11]);
            // Serial.write(data[12]);
            // Serial.write(data[13]);
            // Serial.write(data[14]);
            // Serial.write(data[15]);
            // Serial.println(data.size());
            if (data.size() == 16 && checkId(data))
            {
                mode = data[8];
                value = (data[9] << 8) + data[10];
                // --- debug ---
                // Serial.print(mode);
                // Serial.print(" ");
                // Serial.print(value);
                // Serial.println();
                // --- ----- ---
                switch (mode)
                {
                case 0:
                    motor.controller = MotionControlType::torque;
                    target = 1.4;
                    vTaskDelay(5000 / portTICK_PERIOD_MS); // 初始化
                    target = 0, motor.sensor_offset = sensor.getAngle();
                    break;

                case 1:
                    motor.controller = MotionControlType::torque; // 力矩控制
                    target = value / 100.0;
                    break;

                case 2:
                    motor.controller = MotionControlType::angle; // 角度控制
                    target = 0 - value / 100.0;
                    break;
                }
                data.clear();
            }
        }
        // serp ++;
        // if(serp == 100){
        //     Serial.println();
        // }
        // Protect
        // if (sensor.getAngle() < 0)
        // {
        //     motor.controller = MotionControlType::torque;
        //     target = 0;
        //     Serial.println("angle overrun");
        // }

        vTaskDelay(3 / portTICK_PERIOD_MS);
    }
}

void setup()
{

    // 传感器
    sensor.init();             // 初始化磁传感器
    motor.linkSensor(&sensor); // 将电机与传感器连接

    // 电机驱动配置
    driver.voltage_power_supply = 12; // 供电电压
    driver.init();                    // 驱动初始化
    motor.linkDriver(&driver);        // 绑定驱动与电机

    // FOC控制模式选择
    motor.foc_modulation = FOCModulationType::SpaceVectorPWM;

    // 运动控制环的配置
    // motor.controller = MotionControlType::velocity; // 速度环
    motor.controller = MotionControlType::torque; // 力矩环

    // 控制部分的配置
    // default parameters in defaults.h

    // velocity PI control
    motor.PID_velocity.P = 0.2;
    motor.PID_velocity.I = 2;
    // max velocity
    motor.voltage_limit = 6;
    motor.voltage_sensor_align = 1;

    // velocity low pass filtering time constant
    // the lower the less filtered
    motor.LPF_velocity.Tf = 0.05;

    // angle P controller
    motor.P_angle.P = 20;
    // maximal velocity of the position control
    motor.velocity_limit = 12;

    motor.sensor_offset = 1.78;              // Set the zero electrical angle
    motor.sensor_direction = Direction::CCW; // Set the sensor direction to counter-clockwise

    // use monitoring with serial
    Serial.begin(115200);
    // Serial1.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, 18, 17);
    // comment out if not needed
    motor.useMonitoring(Serial);

    // initialize motor
    motor.init();
    // align sensor and start FOC
    // motor.();
    motor.initFOC();
    // motor.initFOC(4.56, Direction::CW);
    // motor.initFOC(2.25, Direction::CCW);
    // 需要增加跳过矫正 参考http://simplefoc.cn/#/simplefoc_translation/3.3%E4%BB%A3%E7%A0%81/3.3.2%E7%94%B5%E6%9C%BA%E9%85%8D%E7%BD%AE%E4%BB%A3%E7%A0%81/3.3.2.1%E6%97%A0%E5%88%B7%E7%9B%B4%E6%B5%81%E7%94%B5%E6%9C%BA

    // add target command T
    // command.add('T', doTarget, "target angle");
    // command.add('M', onMotor, "my motor");

    Serial.println(F("Motor ready"));

    _delay(1000);

    // 创建任务
    xTaskCreatePinnedToCore(
        Task1code, /* 任务函数 */
        "Task1",   /* 任务名字 */
        50000,     /* 栈大小 */
        NULL,      /* 传递给任务函数的参数 */
        1,         /* 优先级 */
        &Task1,    /* 任务句柄 */
        0);        /* 核心编号 */
    xTaskCreatePinnedToCore(
        Task2code,
        "Task2",
        50000,
        NULL,
        1,
        &Task2,
        1);
}

void loop()
{
    // nothing
}