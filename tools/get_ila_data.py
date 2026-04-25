import serial
import serial.tools.list_ports
import pandas as pd
import time
import tkinter as tk
from tkinter import ttk, messagebox, filedialog
import threading

class SerialDataCollector:
    def __init__(self, root):
        self.root = root
        self.root.title("WIN04 ILA数据采集器")
        self.root.geometry("800x500")
        
        # 创建变量
        self.serial_port = tk.StringVar()
        self.baudrate = tk.StringVar(value="921600")
        self.databits = tk.StringVar(value="8")
        self.parity = tk.StringVar(value="无")
        self.stopbits = tk.StringVar(value="1")
        self.timeout = tk.StringVar(value="60")
        self.max_size = tk.StringVar(value="65536")  # 64KB
        self.is_receiving = False
        self.ser = None
        
        # 创建界面
        self.create_widgets()
        
        # 刷新串口列表
        self.refresh_ports()
        
    def create_widgets(self):
        # 串口配置区域
        config_frame = ttk.LabelFrame(self.root, text="串口配置", padding=10)
        config_frame.pack(fill="x", padx=5, pady=5)
        
        # 第一行：串口选择和波特率
        row1_frame = ttk.Frame(config_frame)
        row1_frame.pack(fill="x", pady=2)
        
        ttk.Label(row1_frame, text="串口:").pack(side="left", padx=5)
        self.port_combo = ttk.Combobox(row1_frame, textvariable=self.serial_port, width=15)
        self.port_combo.pack(side="left", padx=5)
        ttk.Button(row1_frame, text="刷新", command=self.refresh_ports).pack(side="left", padx=5)
        
        ttk.Label(row1_frame, text="波特率:").pack(side="left", padx=(20,5))
        ttk.Entry(row1_frame, textvariable=self.baudrate, width=10).pack(side="left", padx=5)
        
        # 第二行：数据位、校验位、停止位
        row2_frame = ttk.Frame(config_frame)
        row2_frame.pack(fill="x", pady=2)
        
        ttk.Label(row2_frame, text="数据位:").pack(side="left", padx=5)
        databits_combo = ttk.Combobox(row2_frame, textvariable=self.databits, width=8)
        databits_combo['values'] = ('5', '6', '7', '8')
        databits_combo.pack(side="left", padx=5)
        
        ttk.Label(row2_frame, text="校验位:").pack(side="left", padx=(20,5))
        parity_combo = ttk.Combobox(row2_frame, textvariable=self.parity, width=8)
        parity_combo['values'] = ('无', '奇校验', '偶校验')
        parity_combo.pack(side="left", padx=5)
        
        ttk.Label(row2_frame, text="停止位:").pack(side="left", padx=(20,5))
        stopbits_combo = ttk.Combobox(row2_frame, textvariable=self.stopbits, width=8)
        stopbits_combo['values'] = ('1', '1.5', '2')
        stopbits_combo.pack(side="left", padx=5)
        
        # 第三行：超时时间和最大接收大小
        row3_frame = ttk.Frame(config_frame)
        row3_frame.pack(fill="x", pady=2)
        
        ttk.Label(row3_frame, text="超时时间(秒):").pack(side="left", padx=5)
        ttk.Entry(row3_frame, textvariable=self.timeout, width=10).pack(side="left", padx=5)
        
        ttk.Label(row3_frame, text="最大接收大小(字节):").pack(side="left", padx=(20,5))
        ttk.Entry(row3_frame, textvariable=self.max_size, width=15).pack(side="left", padx=5)
        
        # 控制按钮区域
        control_frame = ttk.Frame(self.root)
        control_frame.pack(fill="x", padx=5, pady=5)
        
        self.start_btn = ttk.Button(control_frame, text="开始接收", command=self.start_receiving)
        self.start_btn.pack(side="left", padx=5)
        
        self.stop_btn = ttk.Button(control_frame, text="停止接收", command=self.stop_receiving, state="disabled")
        self.stop_btn.pack(side="left", padx=5)
        
        # 状态显示区域
        status_frame = ttk.LabelFrame(self.root, text="状态", padding=10)
        status_frame.pack(fill="both", expand=True, padx=5, pady=5)
        
        # 创建文本和滚动条的容器
        text_frame = ttk.Frame(status_frame)
        text_frame.pack(fill="both", expand=True)
        
        # 创建文本框和滚动条
        self.status_text = tk.Text(text_frame, height=10, wrap=tk.WORD)
        scrollbar = ttk.Scrollbar(text_frame, orient="vertical", command=self.status_text.yview)
        
        # 配置滚动条和文本框的关系
        self.status_text.configure(yscrollcommand=scrollbar.set)
        
        # 使用grid布局管理器放置文本框和滚动条
        self.status_text.grid(row=0, column=0, sticky="nsew")
        scrollbar.grid(row=0, column=1, sticky="ns")
        
        # 配置grid的权重
        text_frame.grid_rowconfigure(0, weight=1)
        text_frame.grid_columnconfigure(0, weight=1)
        
    def get_serial_config(self):
        """获取串口配置参数"""
        # 数据位映射
        databits_map = {
            '5': serial.FIVEBITS,
            '6': serial.SIXBITS,
            '7': serial.SEVENBITS,
            '8': serial.EIGHTBITS
        }
        
        # 校验位映射
        parity_map = {
            '无': serial.PARITY_NONE,
            '奇校验': serial.PARITY_ODD,
            '偶校验': serial.PARITY_EVEN
        }
        
        # 停止位映射
        stopbits_map = {
            '1': serial.STOPBITS_ONE,
            '1.5': serial.STOPBITS_ONE_POINT_FIVE,
            '2': serial.STOPBITS_TWO
        }
        
        return {
            'bytesize': databits_map[self.databits.get()],
            'parity': parity_map[self.parity.get()],
            'stopbits': stopbits_map[self.stopbits.get()]
        }
        
    def refresh_ports(self):
        ports = serial.tools.list_ports.comports()
        port_list = [f"{port.device} - {port.description}" for port in ports]
        self.port_combo['values'] = port_list
        if port_list:
            self.port_combo.current(0)
            
    def log_message(self, message):
        self.status_text.insert(tk.END, f"{message}\n")
        self.status_text.see(tk.END)
        self.root.update()
        
    def start_receiving(self):
        if not self.serial_port.get():
            messagebox.showerror("错误", "请选择串口！")
            return
            
        self.is_receiving = True
        self.start_btn.config(state="disabled")
        self.stop_btn.config(state="normal")
        
        # 在新线程中运行接收程序
        threading.Thread(target=self.receive_data, daemon=True).start()
        
    def stop_receiving(self):
        self.is_receiving = False
        self.start_btn.config(state="normal")
        self.stop_btn.config(state="disabled")
        if self.ser and self.ser.is_open:
            self.ser.close()
            
    def receive_data(self):
        try:
            # 获取串口号（去掉描述部分）
            port = self.serial_port.get().split(' - ')[0]
            
            # 获取串口配置
            config = self.get_serial_config()
            
            # 初始化串口
            self.ser = serial.Serial(
                port=port,
                baudrate=int(self.baudrate.get()),
                bytesize=config['bytesize'],
                parity=config['parity'],
                stopbits=config['stopbits'],
                timeout=float(self.timeout.get())
            )
            
            self.log_message(f"已打开串口: {port}")
            self.log_message(f"配置: {self.databits.get()}数据位, {self.parity.get()}, {self.stopbits.get()}停止位")
            
            # 接收数据
            received_data = bytearray()
            last_data_time = time.time()
            max_wait_time = float(self.timeout.get())
            max_size = int(self.max_size.get())
            
            self.log_message("开始接收数据...")
            
            while self.is_receiving and len(received_data) < max_size:
                if self.ser.in_waiting > 0:
                    chunk = self.ser.read(self.ser.in_waiting)
                    received_data.extend(chunk)
                    last_data_time = time.time()
                    self.log_message(f"已接收 {len(received_data)} 字节")
                
                if time.time() - last_data_time > max_wait_time:
                    self.log_message("连续未接收到数据，接收结束")
                    break
                
                time.sleep(0.01)
            
            if received_data:
                self.save_data(received_data)
            
        except Exception as e:
            self.log_message(f"错误: {str(e)}")
            messagebox.showerror("错误", str(e))
        finally:
            self.stop_receiving()
            
    def save_data(self, data):
        try:
            # 将数据按4字节分组并转换为16进制
            hex_list = []
            for i in range(0, len(data), 4):
                chunk = data[i:i+4]
                while len(chunk) < 4:
                    chunk.append(0)
                value = int.from_bytes(chunk, byteorder='little', signed=False)
                hex_value = f"0x{value:08X}"
                hex_list.append(hex_value)
            
            # 创建DataFrame
            df = pd.DataFrame({
                'Index': range(len(hex_list)),
                'Hex': hex_list
            })
            
            # 选择保存位置
            filename = filedialog.asksaveasfilename(
                defaultextension=".xlsx",
                filetypes=[("Excel files", "*.xlsx"), ("All files", "*.*")],
                initialfile=f"ila_data_{time.strftime('%Y%m%d_%H%M%S')}.xlsx"
            )
            
            if filename:
                df.to_excel(filename, index=False)
                self.log_message(f"数据已保存到: {filename}")
                messagebox.showinfo("成功", "数据保存成功！")
            
        except Exception as e:
            self.log_message(f"保存数据时出错: {str(e)}")
            messagebox.showerror("错误", f"保存数据时出错: {str(e)}")

def main():
    root = tk.Tk()
    app = SerialDataCollector(root)
    root.mainloop()

if __name__ == "__main__":
    main()
