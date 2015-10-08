
# Map CRC of given files to official or unoffical version string

from zlib import crc32
from sys import argv
import json

with open(argv[1]) as fp:
        versions = json.load(fp)

crc=0
for file in sorted(argv[2:]):
        with open(file) as fp:
                crc = crc32(fp.read(), crc)

crc='x%08x' % (crc & 0xffffffff)
print versions[crc] if crc in versions else crc

