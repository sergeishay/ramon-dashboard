#!/bin/python

import socket
import struct
import json
import time

# Server IP address and port
HOST = "localhost"  # for test on PC
#HOST = "10.1.1.128"  # for Versal
PORT = 65432         # Replace with your chosen port


#command types
GET_DEVICE_STATUS = 0
GET_FILES_STATUS = 1
STOP_ALL = 2
UPLOAD_FILE_TO_STREAM = 3
DOWNLOAD_FILE_FROM_STREAM = 4
DELETE_STREAM = 5
FORMAT = 6
TEST = 7

RESPONSE_SIZE = 256*256

class Ramon_BE:

    def __init__(self, host, port):
        self.counter = 0
        self.requests_socet = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.responses_socet = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # Connect to the server
        self.requests_socet.connect((host, port + 1))
        self.responses_socet.connect((host, port))
        
    def __del__(self):
        self.requests_socet = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.responses_socet = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
    def recvall(self):
        buffer = ""
        received = 0
        while (received < RESPONSE_SIZE):
            data = self.responses_socet.recv(256*256).decode()
            received += len(data)
            buffer += data
        return buffer
     
    def sendall(self, message):
        self.requests_socet.sendall(message)
        time.sleep(0.05)
        
    def pack(self, cmd_type, data = ""):
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
        # Pack data into a binary string
        packed_data_binary = struct.pack(struct_format,
            packed_data["magic"].encode(),
            packed_data["counter"],
            packed_data["type"],
            packed_data["data_size"],
            packed_data["string_field"].encode())
        
        return packed_data_binary
        
    def string_in_big_buffer_to_json(self, buffer):
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
        
        # Extract the JSON string
        json_str = buffer[:end_of_json]
        
        # Parse the JSON string into a Python dictionary
        parsed_json = json.loads(json_str)
        
        return parsed_json
        
    def get_device_status(self):
        message = self.pack(GET_DEVICE_STATUS)
        self.sendall(message)
        data = self.recvall()
        json_data = self.string_in_big_buffer_to_json(data)
        print(json_data)
        return json_data
        
    def get_files_status(self):
        message = self.pack(GET_FILES_STATUS)
        self.sendall(message)
        data = self.recvall()
        json_data = self.string_in_big_buffer_to_json(data)
        return json_data
        
    def stop(self):
        message = self.pack(STOP_ALL)
        self.sendall(message)
        data = self.recvall()
        return 
        
    def upload_file_to_stream(self, file_name):
        message = self.pack(UPLOAD_FILE_TO_STREAM, file_name)
        self.sendall(message)
        
    def download_file_from_stream(self, stream_name):
        message = self.pack(DOWNLOAD_FILE_FROM_STREAM, stream_name)
        self.sendall(message)
        
    def delete_stream(self, stream_name):
        message = self.pack(DELETE_STREAM, stream_name)
        self.sendall(message)
        
    def format(self):
        message = self.pack(FORMAT)
        self.sendall(message)

if __name__ == '__main__':
    demo_instance = Ramon_BE(HOST, PORT)
    
    #start = time.time()
    files_json = demo_instance.get_files_status()
    files_list = list(files_json.keys())
    #end = time.time()
    #print(end - start)

    #start = time.time()
    demo_instance.get_device_status()
    #end = time.time()
    #print(end - start)
    
    #demo_instance.get_files_status()
    
    #demo_instance.delete_stream("")
    #demo_instance.delete_stream("not real")
    #demo_instance.delete_stream("S02E04_Garden_of_Bones.mkv")
    
    demo_instance.upload_file_to_stream("not_real.mkv")
    demo_instance.upload_file_to_stream(files_list[1])
    demo_instance.upload_file_to_stream(files_list[3])
    demo_instance.upload_file_to_stream(files_list[3])
    demo_instance.upload_file_to_stream(files_list[3])
    demo_instance.download_file_from_stream(files_list[3])
    
    #demo_instance.stop()
    #demo_instance.format()
    demo_instance.get_device_status()
    demo_instance.get_device_status()

