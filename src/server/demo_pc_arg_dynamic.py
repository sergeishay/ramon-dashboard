#!/bin/python

import socket
import struct
import json
import time
import sys

# Server IP address and port
HOST = socket.gethostbyname('localhost')  # For test on PC
PORT = 65432  # Replace with your chosen port

# Command enums
GET_DEVICE_STATUS = 0
GET_FILES_STATUS = 1
STOP_ALL = 2
UPLOAD_FILE_TO_STREAM = 3
DOWNLOAD_FILE_FROM_STREAM = 4
DELETE_STREAM = 5
FORMAT = 6
TEST = 7

RESPONSE_SIZE = 256 * 256

class RamonBE:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.counter = 0
        self.setup_connections()

    def setup_connections(self):
        self.requests_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.responses_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.requests_socket.connect((self.host, self.port + 1))
        self.responses_socket.connect((self.host, self.port))

    def close_connections(self):
        self.requests_socket.close()
        self.responses_socket.close()

    def receive_all(self):
        buffer = ""
        received = 0
        while received < RESPONSE_SIZE:
            data = self.responses_socket.recv(256 * 256).decode()
            received += len(data)
            buffer += data
        return buffer

    def send_all(self, message):
        self.requests_socket.sendall(message)
        time.sleep(0.05)

    def pack(self, cmd_type, data=""):
        packed_data = {
            "magic": 'Demo',
            "counter": self.counter,
            "type": cmd_type,
            "data_size": len(data),
            "string_field": data
        }
        self.counter += 1
        tmp_size = len(packed_data["string_field"])
        packed_data["string_field"] += '\0' * (256 - tmp_size)
        struct_format = f"!4siii{tmp_size}s"
        packed_data_binary = struct.pack(struct_format,
                                         packed_data["magic"].encode(),
                                         packed_data["counter"],
                                         packed_data["type"],
                                         packed_data["data_size"],
                                         packed_data["string_field"].encode())
        return packed_data_binary

    @staticmethod
    def string_in_big_buffer_to_json(buffer):
        brace_count = 0
        end_of_json = 0
        for i, char in enumerate(buffer):
            if char == '{':
                brace_count += 1
            elif char == '}':
                brace_count -= 1
                if brace_count == 0:
                    end_of_json = i + 1
                    break
        json_str = buffer[:end_of_json]
        parsed_json = json.loads(json_str)
        return parsed_json

    # Example method implementations
    def get_device_status(self):
        self.setup_connections()
        message = self.pack(GET_DEVICE_STATUS)
        self.send_all(message)
        data = self.receive_all()
        json_data = self.string_in_big_buffer_to_json(data)
        self.close_connections()
        print(json_data)
        return json_data

    def get_files_status(self):
        self.setup_connections()
        message = self.pack(GET_FILES_STATUS)
        self.send_all(message)
        data = self.receive_all()
        json_data = self.string_in_big_buffer_to_json(data)
        self.close_connections()
        return json_data

    # Add other methods as needed

if __name__ == '__main__':
    demo_instance = RamonBE(HOST, PORT)
    function_name = sys.argv[1] if len(sys.argv) > 1 else ''

    if hasattr(demo_instance, function_name):
        method = getattr(demo_instance, function_name)
        if callable(method):
            args = sys.argv[2:]
            result = method(*args) if args else method()
            if result is not None:
                print(result)
        else:
            print(f"{function_name} is not callable")
    else:
        print(f"No such method: {function_name}")
