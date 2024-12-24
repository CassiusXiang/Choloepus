# -*- coding=utf-8 -*-

""" soft_gripper 二指FOC柔性夹爪Python函数库(面向对象)

@ Version
v2.1.2(basic-hardware-software)

@ public function brief

@ can 参数
1. mode : 包模式
2. baudrate : 100,000bps(1Mbps)

@author
Xiang Chang Tianjin University

@email
changxiangchina@outlook.com

"""


class gripper:
    def __init__(self, serial_port, ID):
        self.serial_port = serial_port  # 已经打开的串口对象
        self.ID = ID
        
    # Public ---------------------------------------------------------------
        
    def set_angle(self, angle):
        data = format_data(angle, 0x02, self.ID)
        self.send_command(data=data)
        
    def reset_angle(self):
        data = format_data(0x00, 0x00, self.ID)
        self.send_command(data=data)
        
    def set_torque(self, torque):
        data = format_data(torque, 0x01, self.ID)
        self.send_command(data=data)
        
    # 类私有函数 ------------------------------------------------------------
    # CAN发送函数
    def send_command(self, cmd=0x09, data=[], rtr=0):
        cdata = [0x08, 0, 0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
        id_list = (self.ID << 5) + cmd # id号右移5位 + cmd，此处根据odrive CAN node_id号与cmd_id的角度关系决定
        cdata[1] = id_list >> 8
        cdata[2] = id_list & 0xFF
        for i in range(8):
            cdata[3 + i] = data[i] # data[]中包含命令的内容，如角度、转速、转矩等
        # print("cdata: ", cdata)
        data = can_to_uart(data=cdata, rtr=rtr)
        # print("send_command: ", data)
        self.write_data(data)
        # write_data(data=can_to_uart(data=cdata, rtr=rtr))
        
        # NOTE: the recive data of esp32 is cdata

    # 串口发送函数
    def write_data(self, data=[]):
        try:
            result = self.serial_port.write(data)  # 写数据
            # print("write_data: ", data)
            return result
        except Exception as e:
            print("---error in write_data--:", e)
            print("restart the serial...")
            self.serial_port.close()
            self.serial_port.open()
            result = self.serial_port.write(data)  # 写数据
            return result

# USB转CAN模块包模式：CAN报文->串行帧
def can_to_uart(data=[], rtr=0):
    udata = [0xAA, 0, 0, 0x08, 0, 0, 0, 0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00] # 其中0x08代表CAN读取字头中的DLC（报文长度）
    # udata[1]对应CAN字头中的IDE，udata[2]对应CAN字头中的RTR，udata[3]对应CAN字头中的DLC，udata[4~7]对应id
    # udata 根据USB转CAN模块串口包模式定义
    if len(data) == 11 and data[0] == 0x08: # 0x08 为预设的一个校验字节
        if rtr == 1:
            udata[2] = 0x01 # rtr 标志位
        for i in range(10):
            udata[6 + i] = data[i + 1]
        return udata
    else:
        return []

# 格式化数据
# value : 浮点型数据

def format_data(value, mode, id):
    # value 16bit
    # mode 8bit
    # id 16bit
    rdata = [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
    value = range_constrian(value, 1, 90)
    value = round(value, 2) * 100
    rdata[0] = id >> 8
    rdata[1] = id & 0xFF
    rdata[2] = mode
    # mode = 0 : 力矩控制
    # mode = 1 : 位置控制
    rdata[3] = int(value) >> 8 # high
    rdata[4] = int(value) & 0xFF # low
    # print(rdata)
    return rdata

def range_constrian(value, low, high):
    if value <= low:
        return low
    elif value >= high:
        return high
    return value