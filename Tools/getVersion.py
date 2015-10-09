
# Map CRC of given files to official or unoffical version string
# Official version string is listed in the json file.
# Unofficial version string is 'x<crc32>'
# The idea is that the version number changes when any sources change,
# to avoid accidental misidentification of the binary.

from zlib import crc32
from sys import argv
import json

with open(argv[1]) as fp: versions = json.load(fp)

crc = 0
for file in sorted(argv[2:]):
        with open(file) as fp:
                crc = crc32(fp.read(), crc)

crc='x%08x' % (crc & 0xffffffff)
print versions[crc] if crc in versions else crc

