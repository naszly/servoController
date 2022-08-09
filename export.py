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

    usb_serial = serial.Serial(port, baud, timeout=.1)
    usb_serial.reset_output_buffer()

    def read_until(string):
        while True:
            serial_input = usb_serial.readline().decode().rstrip()
            if len(serial_input) > 0:
                print("received: " + serial_input)
                if string.upper() in serial_input.upper():
                    break

    read_until("ready")

    usb_serial.write("export\n".encode())

    read_until("begin")

    with open(filename, 'w') as file:
        print("writing to file " + filename)
        end = False
        while not end:
            serial_input = usb_serial.readline().decode().rstrip()
            if len(serial_input) > 0:
                print("received: " + serial_input)
                if "END" in serial_input.upper():
                    end = True
                else:
                    file.write(serial_input + "\n")


if __name__ == "__main__":
    main()
