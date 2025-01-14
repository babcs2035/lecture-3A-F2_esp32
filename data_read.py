import serial
import csv
import threading
import time
import argparse
import subprocess
import re
import numpy as np

def get_current_details():
    try:
        command = 'ioreg -l | grep "AdapterDetails" | sed -e \'s/[ \\|]//g\''
        result = subprocess.run(command, shell=True, capture_output=True, text=True)
        output = result.stdout

        current_match = re.search(r'"Current"=(\d+)', output)
        if current_match:
            current_value = current_match.group(1)
            print(current_value)
            return current_value
        else:
            return np.nan
    except Exception as e:
        print(f"Error: {e}")
        return np.nan

def read_serial(port, baudrate, csv_filename):
    try:
        # シリアルポートの設定
        mySP = serial.Serial(port, baudrate)

        # CSVファイルの設定
        with open(csv_filename, mode="w", newline="", encoding="utf-8") as file:
            writer = csv.writer(file)
            writer.writerow(["t", "max", "min", "amp", "pd_power"])

            while True:
                # シリアルポートからデータを読み取る
                myData = mySP.readline().decode("utf-8").strip()

                # current_valueを取得
                current_value = get_current_details()

                # データをCSVファイルに書き込む
                if myData == "start" or myData == "end" or myData == "reset":
                    print(f"esp32 {myData}\n> ", end="")
                else:
                    data_list = myData.split(",")
                    data_list.append(current_value)
                    writer.writerow(data_list)
                    file.flush()
    except Exception as e:
        print(f"Error: {e}")

def send_serial(port, baudrate, message):
    try:
        # シリアルポートの設定
        mySP = serial.Serial(port, baudrate)
        time.sleep(2)

        # メッセージをシリアルポートに送信する
        mySP.write(message.encode("utf-8"))

        # シリアルポートを閉じる
        mySP.close()
    except Exception as e:
        print(f"Error: {e}")

def main():
    parser = argparse.ArgumentParser(description="Read and write data to ESP32 via serial port")
    parser.add_argument("-p", "--port", type=str, default="/dev/cu.usbserial-140" ,help="Serial port name")
    parser.add_argument("-b", "--baudrate", type=int, default=115200, help="Baudrate")
    parser.add_argument("-d", "--dst", type=str, default="67", help="csv file name")
    args = parser.parse_args()

    port = args.port
    baudrate = args.baudrate
    csv_filename = f"w{args.dst}.csv"

    print(port, baudrate, csv_filename)

    # シリアルポートの読み取りを開始する
    serial_thread = threading.Thread(target=read_serial, args=(port, baudrate, csv_filename))
    serial_thread.daemon = True
    serial_thread.start()

    while True:
        user_input = input("> ")
        if user_input in ['s', 'q']:
            send_serial(port, baudrate, user_input)
        elif user_input == 'exit':
            break

if __name__ == "__main__":
    main()
