import serial
import time
import keyboard  # 引入keyboard库
from soft_gripper_can import gripper

id_num = 20  # 夹爪id默认为20
angle = 1

# 打开串口
my_serial_port = serial.Serial('COM22', 115200, timeout=1)

# 创建gripper对象，传入已打开的串口对象和ID
my_gripper = gripper(my_serial_port, id_num)

if __name__ == "__main__":
    print("v2.1.2 pydemo")
    print("使用上键控制夹爪打开，使用下键控制夹爪闭合，按 'q' 键退出程序")

# 无限循环，检测键盘输入
while True:
    try:
        # 监听键盘事件
        if keyboard.is_pressed('up'):  # 如果按下上键
            # print("打开夹爪")
            # my_gripper.set_angle(0)  # 假设0度是张开的状态
            angle += 0.1
            if(angle > 85):
                angle = 85
            # time.sleep(0.05)  # 为了防止一次触发多次动作，可以加一个延时

        elif keyboard.is_pressed('down'):  # 如果按下下键
            # print("闭合夹爪")
            # my_gripper.set_angle(90)  # 假设90度是闭合的状态
            angle -= 0.1
            if(angle < 1):
                angle = 1
            # time.sleep(0.05)  # 同样加一个短暂延时

        elif keyboard.is_pressed('q'):  # 按下q键退出程序
            print("退出程序")
            break
        my_gripper.set_angle(angle)
        time.sleep(0.01)

    except Exception as e:
        print(f"程序发生错误: {e}")
        break

    