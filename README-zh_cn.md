# Choloepus
[![English badge](https://img.shields.io/badge/%E8%8B%B1%E6%96%87-English-blue)](./README.md)
[![简体中文 badge](https://img.shields.io/badge/%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87-Simplified%20Chinese-green)](./README-zh_cn.md)

## 介绍
Choloepus 是一个使用三相电机驱动的FOC柔性夹爪

*实物图*

## 性能参数

- 可以夹起 10kg 的水（演示视频）
- 可以夹针操作，可以夹起蛋壳（演示视频）
- 最大驱动电流：2A
- 驱动电压范围：12V ~ 24V
- 支持三种控制模式：力闭环控制（电流闭环），位置闭环控制，力位置混合控制、

## 机械结构

![](4.Docs/Image/machine_struct.jpg)

## 电路设计

|  设计要点   | 方案  |
|  ----  | ----  |
| 主控芯片  | ESP32-WROOM-32E |
| 通信  | CAN, TTL |
| 电路层数  | 4层 |
| 驱动方案  | 6颗MOS构成的桥 |
| 电流采样  | 比较放大器放大采样电阻电压 |
| 位置采样  | AS5600霍尔传感器 |
| 驱动电源  | 24V->12V DCDC降压 |
| 控制电源  | 24V->5V DCDC降压, 5V->3.3V LDO降压 |

## 控制算法

## 通信

## 视频演示

## 许可证

OmniRob采用MIT许可证进行发布。请参考[LICENSE](https://github.com/CassiusXiang/OmniRob/blob/main/LICENSE)获取更多信息。

## 联系方式

如果您有任何问题或反馈，请通过[Issues](https://github.com/CassiusXiang/OmniRob/issues)与我们联系。

我的邮箱: changxiangchina@outlook.com
