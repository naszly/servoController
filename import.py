import sys
import time
import serial


def main():
    filename = "export.txt"
    port = "/dev/ttyUSB0"
    baud = 9600

    if len(sys.argv) > 1:
        filename = sys.argv[1]
        if len(sys.argv) > 2:
            port = sys.argv[2]
            if len(sys.argv) > 3:
                baud = sys.argv[3]

    usb_serial = serial.Serial(port, baud, timeout=.01)
    usb_serial.reset_output_buffer()

    def read_until(string):
        while True:
            serial_input = usb_serial.readline().decode().rstrip()
            if len(serial_input) > 0:
                print("received: " + serial_input)
                if string.upper() in serial_input.upper():
                    break

    read_until("ready")

    with open(filename) as file:
        for line in file:
            line = line.rstrip()
            if len(line) < 1:
                continue
            print("sent: " + line)
            usb_serial.write((line + "\n").encode())
            serial_input = usb_serial.readline().decode().rstrip()
            while len(serial_input) < 1:
                serial_input = usb_serial.readline().decode().rstrip()
            while len(serial_input) > 0:
                print("received: " + serial_input)
                if "ERR" in serial_input.upper():
                    input("Press Enter to continue...")
                serial_input = usb_serial.readline().decode().rstrip()

    response = input("Save? (Y/N)")
    if "Y" in response.upper():
        usb_serial.write("save\n".encode())
        read_until("saved")
    else:
        print("Data NOT saved")


if __name__ == "__main__":
    main()
