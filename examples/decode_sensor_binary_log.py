#!/usr/bin/env python3

import struct, sys, re

# 3.7 for dataclasses, 3.8 for walrus (:=) in recovery
assert (sys.version_info.major >= 3 and sys.version_info.minor >= 8), \
    "Python version should be at least 3.8."

from brping import PingParser
from dataclasses import dataclass
from typing import IO, Any


def indent(obj, by=' '*4):
    return by + str(obj).replace('\n', f'\n{by}')


@dataclass
class PingViewerBuildInfo:
    hash_commit: str = ''
    date: str = ''
    tag: str = ''
    os_name: str = ''
    os_version: str = ''

    def __str__(self):
        return f"""PingViewerBuildInfo:
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
    {indent(self.ping_viewer_build_info)}
    {indent(self.sensor)}
    """


class Log:
    ''' Structured as a big-endian sequence of
        size: uint32, data: byte_array[size].
    '''

    # int32 values used in message header
    INT = struct.Struct('>i')
    # big-endian uint32 'size' parsed for every timestamp and message
    #  -> only compile and calcsize once
    UINT = struct.Struct('>I')
    # NOTE: ping-viewer message buffer length is 10240 (from pingparserext.h)
    #  should we use this instead??
    # longest possible ping message (id:2310) w/ 1200 samples
    #  -> double the length in case windows used UTF-16
    MAX_ARRAY_LENGTH = 1220*2
    # timestamp format for recovery hh:mm:ss.xxx
    # includes optional \x00 (null byte) before every character because Windows
    TIMESTAMP_FORMAT = re.compile(
        b'(\x00?\d){2}(\x00?:\x00?[0-5]\x00?\d){2}\x00?\.(\x00?\d){3}')
    MAX_TIMESTAMP_LENGTH = 12 * 2

    def __init__(self, filename: str):
        self.filename = filename
        self.header = Header()
        self.messages = []

    @classmethod
    def unpack_int(cls, file: IO[Any]):
        data = file.read(cls.INT.size)
        return cls.INT.unpack_from(data)[0]

    @classmethod
    def unpack_uint(cls, file: IO[Any]):
        ''' String and data array lengths. '''
        data = file.read(cls.UINT.size)
        return cls.UINT.unpack_from(data)[0]

    @classmethod
    def unpack_array(cls, file: IO[Any]):
        ''' Returns the unpacked array if <= MAX_ARRAY_LENGTH, else None. '''
        array_size = cls.unpack_uint(file)
        if array_size <= cls.MAX_ARRAY_LENGTH:
            return file.read(array_size)

    @classmethod
    def unpack_string(cls, file: IO[Any]):
        return cls.unpack_array(file).decode('UTF-8')

    @classmethod
    def unpack_message(cls, file: IO[Any]):
        timestamp = cls.unpack_string(file)
        message = cls.unpack_array(file)
        if message is None:
            return cls.recover(file)
        return (timestamp, message)

    @classmethod
    def recover(cls, file: IO[Any]):
        """ Attempt to recover from a failed read.

        Assumed that a bad number has been read from the last cls.UINT.size
        set of bytes -> try to recover by seeking 'file' back to there, then
        read until the next timestamp, and continue as normal from there.

        """
        file.seek(current_pos := (file.tell() - cls.UINT.size))
        prev_ = next_ = b''
        start = amount_read = 0
        while not (match := cls.TIMESTAMP_FORMAT.search(
                roi := (prev_ + next_), start)):
            prev_ = next_
            next_ = file.read(cls.MAX_ARRAY_LENGTH)
            if not next_:
                break # run out of file
            amount_read += cls.MAX_ARRAY_LENGTH
            if start == 0 and prev_:
                # onto the second read
                # -> match on potential overlap + new region, not the
                #    already-checked (impossible) region
                start = cls.MAX_ARRAY_LENGTH - cls.MAX_TIMESTAMP_LENGTH
        else:
            # match was found
            end = match.end()
            timestamp = roi[match.start():end].decode('UTF-8')
            # return the file pointer to the end of this timestamp
            file.seek(current_pos + amount_read - (len(roi) - end))
            # attempt to extract the corresponding message, or recover anew
            if (message := cls.unpack_array(file)) is None:
                return cls.recover(file)
            return (timestamp, message)
        raise EOFError('No timestamp match found in recovery attempt')

    def unpack_header(self, file: IO[Any]):
        self.header.string = self.unpack_string(file)
        self.header.version = self.unpack_int(file)

        for info in ('hash_commit', 'date', 'tag', 'os_name', 'os_version'):
            setattr(self.header.ping_viewer_build_info, info,
                    self.unpack_string(file))

        self.header.sensor.family = self.unpack_int(file)
        self.header.sensor.type_sensor = self.unpack_int(file)

    def process(self):
        """ Process and store the entire file into self.messages. """
        self.messages.extend(self)

    def __iter__(self):
        """ Creates an iterator for efficient reading of this Log.

        Yields (timestamp, message) pairs for decoding.

        """
        with open(self.filename, "rb") as file:
            self.unpack_header(file)
            while True:
                try:
                    yield self.unpack_message(file)
                except struct.error:
                    break # reading complete

    def parser(self):
        """ Returns a generator that parses and decodes this Log.

        Yields (timestamp, message) pairs. Message decoded as PingMessage.

        """
        self._parser = PingParser()
        ping_ids = {1300, 2300, 2301} # Ping1D, Ping360, Ping360 auto-data

        for (timestamp, message) in self:
            # parse each byte of the message
            for byte in message:
                # Check if the parser has registered and verified this message
                if self._parser.parse_byte(byte) == self._parser.NEW_MESSAGE:
                    # Get decoded message
                    decoded_message = self._parser.rx_msg
                    if decoded_message.message_id in ping_ids:
                        yield timestamp, decoded_message
                        break # this message is (should be?) over, get next one
                # else message is still being parsed


if __name__ == "__main__":
    from argparse import ArgumentParser

    # Parse arguments
    parser = ArgumentParser(description=__doc__)
    parser.add_argument("file",
                        help="File that contains PingViewer sensor log file.")
    args = parser.parse_args()

    # Open log and begin processing
    log = Log(args.file)

    for index, (timestamp, decoded_message) in enumerate(log.parser()):
        if index == 0:
            # Get header information from log
            # (parser has to do first yield before header info is available)
            print(log.header)

            # ask if processing
            yes = input("Continue and decode received messages? [Y/n]: ")
            if yes.lower() in ('n', 'no'):
                break

        print('timestamp:', repr(timestamp))
        print(decoded_message)
        # uncomment to confirm continuing after each message is printed
        #out = input('q to quit, enter to continue: ')
        #if out.lower() == 'q': break
