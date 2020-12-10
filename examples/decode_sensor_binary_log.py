#!/usr/bin/env python3

import sys
import struct
from argparse import ArgumentParser
from brping import PingParser
from dataclasses import dataclass
from typing import IO, Any


@dataclass
class PingViewerBuildInfo:
    hash_commit: str = ''
    date: str = ''
    tag: str = ''
    os_name: str = ''
    os_version: str = ''

    def __str__(self):
        return f"""
PingViewerBuildInfo:
    hash: {self.hash_commit}
    date: {self.date}
    tag: {self.tag}
    os:
        name: {self.os_name}
        version: {self.os_version}
    """


@dataclass
class Sensor:
    family: int = 0
    type_sensor: int = 0

    def __str__(self):
        return f"""Sensor:
    Family: {self.family}
    Type: {self.type_sensor}
    """


@dataclass
class Header:
    string: str = ''
    version: int = 0
    ping_viewer_build_info = PingViewerBuildInfo()
    sensor = Sensor()

    def __str__(self):
        return f"""Header:
    String: {self.string}
    Version: {self.version}
    PingViewerBuildInfo:
        hash: {self.ping_viewer_build_info.hash_commit}
        date: {self.ping_viewer_build_info.date}
        tag: {self.ping_viewer_build_info.tag}
        os:
            name: {self.ping_viewer_build_info.os_name}
            version: {self.ping_viewer_build_info.os_version}
    Sensor:
        Family: {self.sensor.family}
        Type: {self.sensor.type_sensor}
    """


class Log:
    def __init__(self, filename: str):
        self.filename = filename
        self.header = Header()
        self.messages = []

    @staticmethod
    def unpack_int(file: IO[Any]):
        version_format = '>1i'
        data = file.read(struct.calcsize(version_format))
        return struct.Struct(version_format).unpack_from(data)[0]

    @staticmethod
    def unpack_array(file: IO[Any]):
        array_size = Log.unpack_int(file)
        return file.read(array_size)

    @staticmethod
    def unpack_string(file: IO[Any]):
        return Log.unpack_array(file).decode('UTF-8')

    @staticmethod
    def unpack_message(file: IO[Any]):
        timestamp = Log.unpack_string(file)
        message = Log.unpack_array(file)
        return (timestamp, message)

    def unpack_header(self, file: IO[Any]):
        self.header.string = self.unpack_string(file)
        self.header.version = self.unpack_int(file)

        self.header.ping_viewer_build_info.hash_commit = self.unpack_string(
            file)
        self.header.ping_viewer_build_info.date = self.unpack_string(file)
        self.header.ping_viewer_build_info.tag = self.unpack_string(file)
        self.header.ping_viewer_build_info.os_name = self.unpack_string(file)
        self.header.ping_viewer_build_info.os_version = self.unpack_string(
            file)

        self.header.sensor.family = self.unpack_int(file)
        self.header.sensor.type_sensor = self.unpack_int(file)

    def process(self):
        with open(self.filename, "rb") as file:
            self.unpack_header(file)
            while True:
                try:
                    self.messages.append(self.unpack_message(file))
                except:
                    break


if __name__ == "__main__":
    assert(sys.version_info.major >= 3 and sys.version_info.minor >= 7), "Python version should be at least 3.7."

    # Parse arguments
    parser = ArgumentParser(description=__doc__)
    parser.add_argument("-f", dest="file", required=True,
                        help="File that contains PingViewer sensor log file.")
    args = parser.parse_args()

    # Open log and process it
    log = Log(args.file)
    log.process()

    # Get information from log
    print(log.header)

    input("Press Enter to continue and decode received messages...")

    ping_parser = PingParser()
    for (timestamp, message) in log.messages:
        print("timestamp: %s" % timestamp)
        # Parse each byte of the message
        for byte in message:
            # Check if the parser has a new message
            if ping_parser.parse_byte(byte) is PingParser.NEW_MESSAGE:
                # Get decoded message
                decoded_message = ping_parser.rx_msg
                # Filter for the desired ID
                # 1300 for Ping1D profile message and 2300 for Ping360
                if decoded_message.message_id in [1300, 2300]:
                    # Print message
                    print(decoded_message)
