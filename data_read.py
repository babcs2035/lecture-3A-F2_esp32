import serial
import csv
import threading
import time
import argparse
import subprocess
import re
import numpy as np

def get_property_value(property):
    try:
        command = f'ioreg -rw0 -c AppleSmartBattery | grep -o -e \'"{property}" *= *[0-9]*\''
        result = subprocess.run(command, shell=True, capture_output=True, text=True)
        output = result.stdout

        property_match = re.search(fr'"{property}" *=( *[0-9]+)', output)
        if property_match:
            property_value = property_match.group(1)
            return property_value
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
            defalut_property_names = ["t", "max", "min", "amp"]
            property_list = ["ChargingCurrent", "ChargingVoltage", "AppleRawBatteryVoltage", "Amperage"]
            calculated_property_list = ["ChargingPower", "ConsumedPower", "SuppliedPower"]
            writer.writerow(defalut_property_names + property_list + calculated_property_list)

            while True:
                # シリアルポートからデータを読み取る
                myData = mySP.readline().decode("utf-8").strip()
                print(myData)

                # システムデータを取得
                property_values = [get_property_value(property) for property in property_list]

                # データをCSVファイルに書き込む
                if myData in ["start", "end", "reset"]:
                    print(f"esp32 {myData}\n> ", end="")
                else:
                    data_list = myData.split(",")
                    data_list.extend(property_values)
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
    parser.add_argument("-p", "--port", type=str, default="/dev/cu.usbserial-1110" ,help="Serial port name")
    parser.add_argument("-b", "--baudrate", type=int, default=115200, help="Baudrate")
    args = parser.parse_args()

    command = "system_profiler SPPowerDataType | grep Wattage"
    wattage = subprocess.run(command, shell=True, capture_output=True, text=True)
    #dst = wattage.stdout.split(":")[1].strip()
    dst = "0"

    timestamp = time.strftime("%Y%m%d%H%M%S")

    port = args.port
    baudrate = args.baudrate
    csv_filename = f"w{dst}_{timestamp}.csv"

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
