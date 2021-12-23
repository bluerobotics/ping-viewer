#!/usr/bin/env python3

from decode_sensor_binary_log import (
    PingViewerLogReader, PingMessage,
    Header, Sensor, PingViewerBuildInfo
)
from typing import Iterable, Tuple

class PingViewerLogWriter:
    INT = PingViewerLogReader.INT
    UINT = PingViewerLogReader.UINT

    def __init__(self, filename: str,
                 messages: Iterable[Tuple[str, PingMessage]],
                 header: Header):
        """ Writes a log file in the PingViewer binary format.

        'filename' is the filename/path to write to
            (e.g. '20211223-132558123.bin')
        'messages' is an iterable of timestamp, PingMessage tuples, like that
            output by PingViewerLogReader.parser(). Timestamp should be in the
            format 'hh:mm:ss.zzz'.
        'header' is the Header to save with. If re-encoding an existing log
            file, the PingViewerLogWriter.with_reference_log constructor may
            be a more convenient alternative.

        """
        with open(filename, 'wb') as file:
            file.write(self.pack_header(header))
            for timestamp, message in messages:
                file.write(self.pack_message(timestamp, message))

    @classmethod
    def pack_array(cls, array: bytes):
        return cls.UINT.pack(len(array)) + array

    @classmethod
    def pack_string(cls, string: str):
        return cls.pack_array(string.encode('utf-8'))

    @classmethod
    def pack_message(cls, timestamp: str, message: PingMessage):
        return (cls.pack_string(timestamp)
                + cls.pack_array(message.pack_msg_data()))

    @classmethod
    def pack_header(cls, header: Header):
        return b''.join((
            cls.pack_string(header.string),
            cls.INT.pack(header.version),
            *(cls.pack_string(
                getattr(header.ping_viewer_build_info, build_info))
              for build_info in PingViewerBuildInfo.__annotations__),\
            *(cls.INT.pack(sensor_info)
              for sensor_info in (header.sensor.family,
                                  header.sensor.type_sensor))
            ))

    @classmethod
    def with_reference_log(cls, output_file: str,
                        messages: Iterable[Tuple[str, PingMessage]],
                        reference_bin_file: str):
        """ Writes a log file in the PingViewer binary format.

        This alternate constructor takes an existing log filename and extracts
            the header information for use in the saved file.

        """
        reference_log = PingViewerLogReader(reference_bin_file)
        next(iter(reference_log)) # parse first message, to extract header
        cls(output_file, messages, reference_log.header)


if __name__ == '__main__':
    from argparse import ArgumentParser
    from itertools import islice

    parser = ArgumentParser()
    parser.add_argument("file", help="Ping Viewer .bin sensor log to process")
    parser.add_argument("--output",
                        help="Output filename. Default is {file}_processed.bin")
    parser.add_argument("--start", type=int, default=0,
                        help=("Message to start at. Default 0."))
    parser.add_argument("--stop", type=int, default=None,
                        help=("Message to stop at. "
                              "Default None (stops at end)."))
    parser.add_argument("--step", type=int, default=1,
                        help=("Extract every nth message. "
                              "Default 1 (every message)."))
    args = parser.parse_args()

    in_file = args.file
    output = args.output or in_file[:in_file.rindex('.')] + "_processed.bin"

    log = PingViewerLogReader(in_file)
    process = islice(log.parser(), args.start, args.stop, args.step)
    PingViewerLogWriter.with_reference_log(output, process, in_file)
