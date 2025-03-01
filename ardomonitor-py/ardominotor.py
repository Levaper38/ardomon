import psutil
import argparse
import time
import sys
import glob

import serial

import json

settings = {
    "port": "",
    "temp_sensor": "",
}

def SaveSetting():
    with open("ardomonitor.conf", "w", encoding="utf-8") as file:
        json.dump(settings, file, ensure_ascii=False, indent=4) 

try:
    # Открываем файл JSON для чтения
    with open("ardomonitor.conf", "r", encoding="utf-8") as file:
        ardoconf = json.load(file)  # Загружаем данные из файла

    # Обновляем только существующие ключи
    for key in settings:
        if key in ardoconf:
            settings[key] = ardoconf[key]

except ValueError:
    SaveSetting()


def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result

def get_network_speed_mb(interval=1):
    """
    Возвращает скорость загрузки и отдачи в мегабайтах в секунду.
    :param interval: Интервал времени для замера (в секундах).
    :return: Кортеж (upload_speed_mb, download_speed_mb).
    """
    # Получаем начальные значения сетевого ввода-вывода
    initial_counters = psutil.net_io_counters()
    bytes_sent_start = initial_counters.bytes_sent
    bytes_recv_start = initial_counters.bytes_recv

    # Ждем указанный интервал времени
    time.sleep(interval)

    # Получаем конечные значения сетевого ввода-вывода
    final_counters = psutil.net_io_counters()
    bytes_sent_end = final_counters.bytes_sent
    bytes_recv_end = final_counters.bytes_recv

    # Вычисляем скорость (байты в секунду)
    upload_speed_bytes = (bytes_sent_end - bytes_sent_start) / interval
    download_speed_bytes = (bytes_recv_end - bytes_recv_start) / interval

    # Преобразуем скорость в мегабайты в секунду
    upload_speed_mb = upload_speed_bytes / (1024 * 1024)
    download_speed_mb = download_speed_bytes / (1024 * 1024)

    return upload_speed_mb, download_speed_mb

def get_disk_speed_mb(interval=1):
    """
    Возвращает скорость чтения и записи на дисках в мегабайтах в секунду.
    :param interval: Интервал времени для замера (в секундах).
    :return: Кортеж (read_speed_mb, write_speed_mb).
    """
    # Получаем начальные значения операций ввода-вывода на дисках
    initial_counters = psutil.disk_io_counters()
    bytes_read_start = initial_counters.read_bytes
    bytes_written_start = initial_counters.write_bytes

    # Ждем указанный интервал времени
    time.sleep(interval)

    # Получаем конечные значения операций ввода-вывода на дисках
    final_counters = psutil.disk_io_counters()
    bytes_read_end = final_counters.read_bytes
    bytes_written_end = final_counters.write_bytes

    # Вычисляем скорость (байты в секунду)
    read_speed_bytes = (bytes_read_end - bytes_read_start) / interval
    write_speed_bytes = (bytes_written_end - bytes_written_start) / interval

    # Преобразуем скорость в мегабайты в секунду
    read_speed_mb = read_speed_bytes / (1024 * 1024)
    write_speed_mb = write_speed_bytes / (1024 * 1024)

    return read_speed_mb, write_speed_mb

baudrate = 9600



def main():
    try:
        # Open the COM port
        ser = serial.Serial(settings["port"], baudrate=baudrate)
        print("Serial connection established.")

        # Send commands to the Arduino
        while True:
            tempDict = psutil.sensors_temperatures(fahrenheit=False)

            cpu_percent = psutil.cpu_percent(interval=1)
            mem = psutil.virtual_memory()
            mem_percent = (mem.total - mem.available) / mem.total * 100
            currentTemp = tempDict[settings["temp_sensor"]][0].current       
            upload_speed_mb, download_speed_mb = get_network_speed_mb(interval=1)
            read_speed_mb, write_speed_mb = get_disk_speed_mb(interval=1)
            stringToSend = f"{cpu_percent:06.2f}"+f"{mem_percent:06.2f}"+f"{currentTemp:06.2f}"+f"{upload_speed_mb:06.2f}"+f"{download_speed_mb:06.2f}"+f"{read_speed_mb:06.2f}"+f"{write_speed_mb:06.2f}"
            ser.write(stringToSend.encode())
    
            time.sleep(1.5)

    except serial.SerialException as se:
        print("Serial port error:", str(se))
        sys.exit(1)

    except KeyboardInterrupt:
        pass

    finally:
        sys.exit(1)

def choose_port(arr):
    if not arr:
        print("Нет доступных портов")
        return None
    
    for index, value in enumerate(arr):
        print(f"{index + 1}: {value}")
    print("0: Выход")
    
    while True:
        try:
            choice = int(input("Введите номер порта (или 0 для выхода): "))
            if choice == 0:
                return None
            elif 1 <= choice <= len(arr):
                return arr[choice - 1]
            else:
                print("Пожалуйста, введите номер порта")
        except ValueError:
            print("Пожалуйста, выберите из доступных портов.")

def choose_temp_sensor(dictionary):
    if not dictionary:
        print("Не удалось определить датчики!")
        return None
    
    print("Доступные датчики:")
    for index, key in enumerate(dictionary.keys()):
        print(f"{index + 1}: {key} ({dictionary[key]})")
    print("0: Выход")
    
    while True:
        try:
            choice = int(input("Введите номер выбранного датчика (или 0 для выхода): "))
            if choice == 0:
                return None
            elif 1 <= choice <= len(dictionary):
                selected_key = list(dictionary.keys())[choice - 1]
                return selected_key
            else:
                print("Пожалуйста, введите номер из предложенного списка.")
        except ValueError:
            print("Пожалуйста, введите корректное число.")

def config():
    selected_port = choose_port(serial_ports())    
    settings["port"] = selected_port  
    tempDict = psutil.sensors_temperatures(fahrenheit=False)
    selected_temp_sensor = choose_temp_sensor(tempDict)
    settings["temp_sensor"] = selected_temp_sensor

    if selected_port == None:
        sys.exit(1)

    SaveSetting()
    main()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="АрдоМонитор")
    parser.add_argument('--config', action='store_true', help="Запустить настройку скрипта")

    args = parser.parse_args()

    if args.config:
        config()
    else:
        main()












