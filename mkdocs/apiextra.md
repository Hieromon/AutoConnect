## <i class="fa fa-gift"></i> Icons

The library presents two PNG icons which can be used to embed a hyperlink to the AutoConnect menu.

- Bar type <img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAApklEQVRYR2NkGGDAOMD2M4w6YDQEkEMgEJggZwCxGI0T5mug+alAvBFkD7IDXtLBcpjfXgEZ4ugOeAETpHEIgIwHeVYC3QH+0CgAS9AQgCwHRcFmdAfQ0E7cRo9mw0EVAqPlAKhwEKVTVsBZDsyiQ2k4Wg6gxPKgyoZ0Sn+o1iCHQBBQaiYQi9DYJTjbAyAJWluOtz0wWg7QOOqxGz+aDUdDYMBDAACA0x4hs/MPrwAAAABJRU5ErkJggg==" title="AutoConnect menu" alt="AutoConnect menu" />
- Cog type <img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAC2klEQVRIS61VvWsUQRSfmU2pon9BUIkQUaKFaCBKgooSb2d3NSSFKbQR/KrEIiIKBiGF2CgRxEpjQNHs7mwOUcghwUQ7g58IsbGxEBWsb2f8zR177s3t3S2cA8ftzPu993vzvoaSnMu2vRKlaqgKp74Q/tE8qjQPyHGcrUrRjwlWShmDbFMURd/a6TcQwNiYUmpFCPElUebcuQ2vz6aNATMVReHEPwzfSSntDcNwNo2rI+DcvQzhpAbA40VKyV0p1Q9snzBG1qYVcYufXV1sREraDcxpyHdXgkfpRBj6Uwm2RsC5dxxmZ9pdOY9cKTISRcHTCmGiUCh4fYyplTwG2mAUbtMTBMHXOgK9QfyXEZr+TkgQ1oUwDA40hEgfIAfj+HuQRaBzAs9eKyUZ5Htx+T3ZODKG8DzOJMANhmGomJVMXPll+hx9UUAlzZrJJ4QNCDG3VEfguu7mcpmcB/gkBOtShhQhchAlu5jlLUgc9ENgyP5gf9+y6LTv+58p5zySkgwzLNOIGc8sEoT1Lc53NMlbCQQuvMxeCME1NNPVVkmH/i3IzzXDtCSA0qQQwZWOCJDY50jsQRjJmkslEOxvTcDRO6zPxOh5xZglKkYLhWM9jMVnkIsTyMT6NBj7IbOCEjm6HxNVVTo2WXqEWJZ1T8rytB6GxizyDkPhWVpBqfiXUtbo/HywYJSpA9kMamNNPZ71R9Hcm+TMHHZNGw3EuraXEUldbfvw25UdOjqOt+JhMwJd7+jSTpZaEiIcaCDwPK83jtWnTkwnunFMtxeL/ge9r4XItt1RNNaj/0GAcV2bR3U5sG3nEh6M61US+Qrfd9Bs31GGulI2GOS/8dgcQZV1w+ApjIxB7TDwF9GcNzJzoA+rD0/8HvPnXQJCt2qFCwbBTfRI7UyXumWVt+HJ9NO4XI++bdsb0YyrqXmlh+AWOLHaLqS5CLQR5EggR3YlcVS9gKeH2hnX8r8Kmi1CAsl36QAAAABJRU5ErkJggg==" title="AutoConnect menu" alt="AutoConnect menu" />

To reference the icon, use the **AUTOCONNECT_LINK** macro in the sketch. It expands into the string literal as an HTML ```<a></a>``` tag with PNG embedded of the AutoConnect menu hyperlinks. Icon type is specified by the parameter of the macro.

<dl class="apidl">
    <dd><span class="apidef">BAR_24</span>Bars icon, 24x24.</dd>
    <dd><span class="apidef">BAR_32</span>Bars icon, 32x32.</dd>
    <dd><span class="apidef">BAR_48</span>Bars icon, 48x48.</dd>
    <dd><span class="apidef">COG_24</span>Cog icon, 24x24.</dd>
    <dd><span class="apidef">COG_32</span>Cog icon, 32x32.</dd>
</dl>

!!! note "Usage"
    ```cpp
    String html = "<html>";
    html += AUTOCONNECT_LINK(BAR_32);
    html += "</html>";
    server.send(200, "text/html", html);
    ```
