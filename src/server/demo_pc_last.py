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
        self.counter = 0
        self.requests_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.responses_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.requests_socket.connect((host, port + 1))
        self.responses_socket.connect((host, port))

    def __del__(self):
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

    def get_device_status(self):
        message = self.pack(GET_DEVICE_STATUS)
        self.send_all(message)
        data = self.receive_all()
        json_data = self.string_in_big_buffer_to_json(data)
        print(json_data)
        return json_data

    def get_files_status(self):
        message = self.pack(GET_FILES_STATUS)
        self.send_all(message)
        data = self.receive_all()
        json_data = self.string_in_big_buffer_to_json(data)
        return json_data
    def stop(self):
        message = self.pack(STOP_ALL)
        self.send_all(message)
        data = self.receive_all()
        return

    def get_files_status(self):
        message = self.pack(GET_FILES_STATUS)
        self.send_all(message)
        data = self.receive_all()
        json_data = self.string_in_big_buffer_to_json(data)
        return json_data

    def upload_file_to_stream(self, file_name):
        message = self.pack(UPLOAD_FILE_TO_STREAM, file_name)
        self.send_all(message)
        return "Upload successful for " + file_name

    def download_file_from_stream(self, stream_name):
        message = self.pack(DOWNLOAD_FILE_FROM_STREAM, stream_name)
        self.send_all(message)
        return "Download successful for " + stream_name

    def delete_stream(self, stream_name):
        message = self.pack(DELETE_STREAM, stream_name)
        self.send_all(message)
        return "Delete successful for " + stream_name

    def format(self):
        message = self.pack(FORMAT)
        self.send_all(message)
        return "Format successful"

if __name__ == '__main__':
    print(f"Arguments received: {sys.argv}")  # Debugging line
    demo_instance = RamonBE(HOST, PORT)
    function_name = sys.argv[1] if len(sys.argv) > 1 else ''

    # Adjusted execution to handle functions without arguments
    if hasattr(demo_instance, function_name):
        method = getattr(demo_instance, function_name)
        if callable(method):
            # Check if the method expects arguments
            if len(sys.argv) > 2:
                args = sys.argv[2:]
                result = method(*args)
            else:
                result = method()
            if result is not None:
                print(result)
        else:
            print(f"{function_name} is not callable")
    else:
        print(f"No such method: {function_name}")
