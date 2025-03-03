# KM-BUS Protocol (incomplete)
## Resources
A lot of information can be found @ https://github.com/openv/openv/wiki/KM-Bus
But the page is incomplete and, I think, gets some basics of the 0xBF / 0x3F command wrong

## Basic packet structure

[DA][SA][CMD][LEN][DC][SC][.*][CRC][CRC]

DA: Destination Address
SA: Source Address
CMD: Command
LEN: length of the packet incl. DA & CRC. So, at least 8, if no data is sent
DC: I am guessing this is an internal target identifier, like the heating circuit. (Described as "internal slot" in openv.)
SC: I am guessing this is an internal source identifier. (Described as "Source subclass" in openv)
.*: Data (optional)
CRC: 2 bytes of CRC

## Commands
0x00 Noop / Ping
0x80 Noop / Pong
0x31 requesting 1 byte
0x33 requesting n bytes
0x3f this seems to not be a valid command
0xb1 sending 1 byte
0xb3 sending n bytes
0xbf command ??

## Addresses
0x00 Master
0x11 Vitotrol
0x20 Vitocom

## Examples

110000080101CCCC
11 (DA, Vitotrol) is receiving from
00 (SA, Master) a
00 (CMD, Ping)
08 (LEN 8) total bytes
01 (DC, C)

r11003F12010134A8A8A24BAE429D6ACD0D3A


## Data
Registers (with faked registers)
Heater off:
{
25: [2,0],
26: [4,1,1],
29: [0,0,0,0,0,0,17,0,0,0,0,0],
173: [197,1,2],
190: [16,10,0,0,0,0,0,0,0,0,0,0]
}
heater on:
{
25: [2,0],
26: [4,1,2],
29: [0,0,0,0,0,0,17,0,0,0,196,0],
173: [197,1,2],
190: [16,10,0,0,1,0,0,0,0,0,0,0]}
