/**
 *  AutoConnect portal site web page declaration.
 *  @file   AutoConnectPage.h
 *  @author hieromon@gmail.com
 *  @version    0.9.1
 *  @date   2018-02-13
 *  @copyright  MIT license.
 */

#ifndef _AUTOCONNECTPAGE_H_
#define _AUTOCONNECTPAGE_H_

#include "AutoConnectLabels.h"

#define AUTOCONNECT_PARAMID_SSID  "SSID"
#define AUTOCONNECT_PARAMID_PASS  "Passphrase"
#define AUTOCONNECT_PARAMID_CRED  "Credential"

// AutoConnect menu hyper-link as image
#define AUTOCONNECT_GLYPH_COG_24  "iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAC2klEQVRIS61VvWsUQRSfmU2p" \
                                  "on9BUIkQUaKFaCBKgooSb2d3NSSFKbQR/KrEIiIKBiGF2CgRxEpjQNHs7mwOUcghwUQ7g58I" \
                                  "sbGxEBWsb2f8zR177s3t3S2cA8ftzPu993vzvoaSnMu2vRKlaqgKp74Q/tE8qjQPyHGcrUrR" \
                                  "jwlWShmDbFMURd/a6TcQwNiYUmpFCPElUebcuQ2vz6aNATMVReHEPwzfSSntDcNwNo2rI+Dc" \
                                  "vQzhpAbA40VKyV0p1Q9snzBG1qYVcYufXV1sREraDcxpyHdXgkfpRBj6Uwm2RsC5dxxmZ9pd" \
                                  "OY9cKTISRcHTCmGiUCh4fYyplTwG2mAUbtMTBMHXOgK9QfyXEZr+TkgQ1oUwDA40hEgfIAfj" \
                                  "+HuQRaBzAs9eKyUZ5Htx+T3ZODKG8DzOJMANhmGomJVMXPll+hx9UUAlzZrJJ4QNCDG3VEfg" \
                                  "uu7mcpmcB/gkBOtShhQhchAlu5jlLUgc9ENgyP5gf9+y6LTv+58p5zySkgwzLNOIGc8sEoT1" \
                                  "Lc53NMlbCQQuvMxeCME1NNPVVkmH/i3IzzXDtCSA0qQQwZWOCJDY50jsQRjJmkslEOxvTcDR" \
                                  "O6zPxOh5xZglKkYLhWM9jMVnkIsTyMT6NBj7IbOCEjm6HxNVVTo2WXqEWJZ1T8rytB6Gxizy" \
                                  "DkPhWVpBqfiXUtbo/HywYJSpA9kMamNNPZ71R9Hcm+TMHHZNGw3EuraXEUldbfvw25UdOjqO" \
                                  "t+JhMwJd7+jSTpZaEiIcaCDwPK83jtWnTkwnunFMtxeL/ge9r4XItt1RNNaj/0GAcV2bR3U5" \
                                  "sG3nEh6M61US+Qrfd9Bs31GGulI2GOS/8dgcQZV1w+ApjIxB7TDwF9GcNzJzoA+rD0/8HvPn" \
                                  "XQJCt2qFCwbBTfRI7UyXumWVt+HJ9NO4XI++bdsb0YyrqXmlh+AWOLHaLqS5CLQR5EggR3Yl" \
                                  "cVS9gKeH2hnX8r8Kmi1CAsl36QAAAABJRU5ErkJggg=="
#define AUTOCONNECT_GLYPH_COG_32  "iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAETElEQVRYR61XX0hbVxg/5yYx" \
                                  "QlEnzM46HYw+7KVuq826Vba1UIRmevNHltHCZEKn7g9lLzJwD8W+dAXZy6i2jaV92BRcMZpc" \
                                  "g7Q+zLaTPXQb7E9toaCUwVbWOZlBk5jcc/Y7WCW5uffuXNvAJSH3933f73zn933nO5Rs4xMI" \
                                  "BN4kRLlebMpeSiQSvzh1R50aCHwgEPoGX5FiWz6cSMS7nfpzTACrr8Pq7yOQ2xBsTddz9clk" \
                                  "ctkJCVMCoVBoP2P8bcbcw1NT4/cKHapq8BSl9KRFkN5EYvKLwnfw9TJjrJtz90VNi/1ktCsh" \
                                  "IILrOrtGqVIlwJyTWQS84PW6Y+l0ukZRXD8QQmstCCzmcp5X0+kdqaqq5Xdg+yGwrz3CLisK" \
                                  "aZmcnPyx0LaIgKq2v0JIfmYzuCHIMlZSqYCBXYqBWQdmFZhqE1wJiSICwWBoEKw/crKH28B+" \
                                  "jm36bNOuiAAE9iIE9vM2nMqacM713ZqmLZoSEH+2tYXmsFfNsh4d4q5i9UcsNfCoxt/F91cO" \
                                  "HUvBKeXt8Xh8wpaA3+/3ejzePy1EJOoiw7lyjlJ9NJPJ3Flfr9MrKlYaFYV1oHp6IMAyczb8" \
                                  "wcrKvw2zs7N5WwKqqj7HOV0wUztWcB8qb8Ue3jYLgipqArGkWZnCLo39f9bYqLZE6PdHatzu" \
                                  "9S4EQe0q9cYAwoHLRX1I4bxdviFkH+y/B8bYKQlj5A/oK+p2K9FYLCayTCgazz68OIHfR/F4" \
                                  "rZ3zAfT6T2U2W1XD57GQHissFpMDEaGFQYqDhcs4JUT+tAOB10HgpoxfWQK8tnanNxqN5mSc" \
                                  "tra2Vrtcnn9ksFIEkDLm8zV5+vv7mYzTJ05ABIWC91ip30gqGAy+gUq6IUOWtrWFD6CGP8bx" \
                                  "G7GuYUGAn9G0eJ+M00AgGIW+u2ywa/A4gufsVhmGw+Gd+Tx7H8w/gEIbTIzXOHft07Txu3Yk" \
                                  "NmYJMmdWhhDyAkp0CIPLpc1+UDIPIH0Nus4XrRoRHL9l1QvEcU4p04B5xoQkjmi2C3NjyrYT" \
                                  "RiKRsmw29wAgs/NcqCGD9A6hHY+kUpXzFRVL4Ko0Yovew+pE2ksakAgIHf9VV1dbb6ykkgyo" \
                                  "augYpWRUZq+dYtAbjiJ7Y7YZAIFvQeCQU+cyeFEZmjZx0JIA+vgLSKOtyGQC2WGM5WwYycID" \
                                  "2Mvexw1iT4B/iXL+ZBNjHMl8jCkzKMOnjE4goiX8J4ZSz/8QRI0ToXSzSvgbcjxceIMqEaE4" \
                                  "TotJsO+g+KHycs94NputxrtbFn2CiHkhn8/vXV1dTVVWVgdQkj3Y9xaQQRz2EOP9YYjwV1sR" \
                                  "ipcbZzrt1HXlfDI58VuhAU5P0Q1Pm2UBAfowcZ0pfIcB53no6jhIjxmDC5zjqxkcPo17w+8w" \
                                  "LTeQyOJS0jA9feWhEw05JiCc4/5wGfeHzuJA7GvsbYeT4NvKgDDamP1Y0RWLMdo8NTUhRjFH" \
                                  "n/8AoRLGHM6hJDMAAAAASUVORK5CYII="
#define AUTOCONNECT_GLYPH_BAR_24  "iVBORw0KGgoAAAANSUhEUgAAABgAAAAYBAMAAAASWSDLAAAALVBMVEUAAAAAAAAAAAAAAAAA" \
                                  "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD////BHg4sAAAADXRSTlMACA8YREhJg5mn" \
                                  "u8rdcKICYgAAAAFiS0dEDm+9ME8AAABFSURBVBjTY2AgDjBWdEBBuwADy104cGBgQ3ASGJj3" \
                                  "nIGC0wZEmszANGsVFKxUYGBFmBaAykFRRiRAcQ6KQ1G8gOI54gAAQlFeCYGJCTQAAAAASUVO" \
                                  "RK5CYII="
#define AUTOCONNECT_GLYPH_BAR_32  "iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAApklEQVRYR2NkGGDAOMD2M4w6" \
                                  "YDQEkEMgEJggZwCxGI0T5mug+alAvBFkD7IDXtLBcpjfXgEZ4ugOeAETpHEIgIwHeVYC3QH+" \
                                  "0CgAS9AQgCwHRcFmdAfQ0E7cRo9mw0EVAqPlAKhwEKVTVsBZDsyiQ2k4Wg6gxPKgyoZ0Sn+o" \
                                  "1iCHQBBQaiYQi9DYJTjbAyAJWluOtz0wWg7QOOqxGz+aDUdDYMBDAACA0x4hs/MPrwAAAABJ" \
                                  "RU5ErkJggg=="
#define AUTOCONNECT_GLYPH_BAR_48  "iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAABIUlEQVRoQ+1YywrCQBCzF72J" \
                                  "fqD6AYKKIPgLKiKKd/1DH0dPpgdBilvtTMp2JcJcLMnsJNtu2qyV+C9LfP0tDRDbQTkgB5wK" \
                                  "aAs5BXTDiw50wLhGDVE9NzuX4A66M2qBeryoiwPscWHC7UtnO4BxGhrg0kDliwpc8Uf/bwfY" \
                                  "YbIZ3XQuYb7GeciBNi6sUKN3m7j9zWz51jmhlmU3sZk9FlAHWSzlQ/dA7PVU7q8tVFkyMuBT" \
                                  "FtqixwDVJffy0v2UhY7oMvZ2qhlfmoVuDVS+UhZKfoAU4vTXLLSBZ018oVEWqvnhYqPXSWzT" \
                                  "jYeSAzwtbUzKQjbdXChlIZd8BHDphy1lIYLCIQploRrFtVPrJLZrx0HKAY6OdhY5YNeOg0ze" \
                                  "gScMDDAxQXzA7QAAAABJRU5ErkJggg=="

// AutoConnect menu href
#define AUTOCONNECT_LINK(s)       "<a href=\"" AUTOCONNECT_URI "\"><img src=\"data:image/png;base64," AUTOCONNECT_GLYPH_ ##s "\" border=\"0\" title=\"AutoConnect menu\" alt=\"AutoConnect menu\"/></a>"

#endif  // _AutoConnectPage_H_
