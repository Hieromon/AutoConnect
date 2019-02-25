## What menus can be made using AutoConnect

AutoConnect generates a menu dynamically depending on the instantiated [AutoConnectAux](acintro.md#how-it-works) at the sketch executing time. Usually, it is a collection of [AutoConnectElement](acelements.md). In addition to this, you can generate a menu from only AutoConnectAux, without AutoConnectElements.<br>In other words, you can easily create a built-in menu featuring the WiFi connection facility embedding the legacy web pages.

## Basic mechanism of menu generation

The sketch can display the [AutoConnect menu](menu.md) by following three patterns depending on AutoConnect-API usage.

<i class="fa fa-desktop"></i>&ensp;**Basic menu**
:    It is the most basic menu for only connecting WiFi. Sketch can automatically display this menu with the basic call sequence of the AutoConnect API which invokes [AutoConnect::begin](api.md#begin) and [AutoConnect::handleClient](api.md#handleclient).

<i class="fa fa-desktop"></i>&ensp;**Extra menu with custom Web pages which is consisted by [AutoConnectElements](acelements.md)**
:    It is an extended menu that appears when the sketch consists of the custom Web pages with [AutoConnectAux](acintro.md#how-it-works) and AutoConnectElements. Refer to section [*Custom Web pages section*](acintro.md#custom-web-pages-in-autoconnect-menu).

<i class="fa fa-desktop"></i>&ensp;**Extra menu which contains legacy pages**
:    It is for the legacy sketches using the **on** handler of ESP8266WebServer/WebServer(for ESP32) class natively and looks the same as the extra menu as above.

The mechanism to generate the AutoConnect menu is simple. It will insert the item as `<li>` tag generated from the [**title**](apiaux.md#autoconnectaux) and [**uri**](apiaux.md#autoconnectaux) member variable of the AutoConnectAux object to the menu list of AutoConnect's built-in HTML. Therefore, the legacy sketches can invoke the web pages from the AutoConnect menu with just declaration the title and URI to AutoConnectAux.

## Place the item for the legacy sketches on the menu

To implement this with your sketch, use only the [AutoConnectAux](apiaux.md#autoconnectaux) constructed with the title and URI of that page. [AutoConnectElements](acelements.md) is not required.

The AutoConnect library package contains an example sketch for ESP8266WebServer known as FSBrowser. Its example is a sample implementation that supports AutoConnect without changing the structure of the original FSBrowser and has the menu item for **Edit** and **List**.

<span style="display:block;margin-left:auto;margin-right:auto;width:282px;height:492px;border:1px solid lightgrey;"><img data-gifffer="images/aux_fsbrowser.gif" data-gifffer-height="490" data-gifffer-width="280" /></span>

The changes I made to adapt the FSBrowser to the AutoConnect menu are slight as follows:

1. Add AutoConnect declaration.
2. Add the menu item named "**Edit**" and "**List**" of AutoConnectAux as each page.
3. Replace the instance of ESP8266WebServer to AutoConnect.
4. Change the menu title to FSBrowser using [AutoConnectConfig::title](apiconfig.md#title).
5. Join the legacy pages to AutoConnect declared at step #1 using [AutoConnect::join](api.md#join).<br>Joining multiple at one time with the [list initialization](https://en.cppreference.com/w/cpp/language/list_initialization) for [std::vector](https://ja.cppreference.com/w/cpp/container/vector/vector).
6. According to the basic procedure of AutoConnect.<br>Establish a connection with [AutoConnect::begin](api.md#begin) and perform [AutoConnect::handleClient](api.md#handleclient) in **loop()**.

<i class="fa fa-code"></i>&ensp;**Modification for FSBrowser** <small>(a part  of sketch code)</small>

<div style="overflow:auto"><img style="width:auto;max-width:none;height:840px" src="images/fsbrowser_ba.svg" /></div>

... and embeds a hyperlink with an [icon](apiextra.md#icons) in the bottom of the body section of *index.htm* contained in the data folder to jump to the AutoConnect menu.

```html
<p style="padding-top:15px;text-align:center">
  <a href="/_ac"><img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAC2klEQVRIS61VvWsUQRSfmU2pon9BUIkQUaKFaCBKgooSb2d3NSSFKbQR/KrEIiIKBiGF2CgRxEpjQNHs7mwOUcghwUQ7g58IsbGxEBWsb2f8zR177s3t3S2cA8ftzPu993vzvoaSnMu2vRKlaqgKp74Q/tE8qjQPyHGcrUrRjwlWShmDbFMURd/a6TcQwNiYUmpFCPElUebcuQ2vz6aNATMVReHEPwzfSSntDcNwNo2rI+DcvQzhpAbA40VKyV0p1Q9snzBG1qYVcYufXV1sREraDcxpyHdXgkfpRBj6Uwm2RsC5dxxmZ9pdOY9cKTISRcHTCmGiUCh4fYyplTwG2mAUbtMTBMHXOgK9QfyXEZr+TkgQ1oUwDA40hEgfIAfj+HuQRaBzAs9eKyUZ5Htx+T3ZODKG8DzOJMANhmGomJVMXPll+hx9UUAlzZrJJ4QNCDG3VEfguu7mcpmcB/gkBOtShhQhchAlu5jlLUgc9ENgyP5gf9+y6LTv+58p5zySkgwzLNOIGc8sEoT1Lc53NMlbCQQuvMxeCME1NNPVVkmH/i3IzzXDtCSA0qQQwZWOCJDY50jsQRjJmkslEOxvTcDRO6zPxOh5xZglKkYLhWM9jMVnkIsTyMT6NBj7IbOCEjm6HxNVVTo2WXqEWJZ1T8rytB6GxizyDkPhWVpBqfiXUtbo/HywYJSpA9kMamNNPZ71R9Hcm+TMHHZNGw3EuraXEUldbfvw25UdOjqOt+JhMwJd7+jSTpZaEiIcaCDwPK83jtWnTkwnunFMtxeL/ge9r4XItt1RNNaj/0GAcV2bR3U5sG3nEh6M61US+Qrfd9Bs31GGulI2GOS/8dgcQZV1w+ApjIxB7TDwF9GcNzJzoA+rD0/8HvPnXQJCt2qFCwbBTfRI7UyXumWVt+HJ9NO4XI++bdsb0YyrqXmlh+AWOLHaLqS5CLQR5EggR3YlcVS9gKeH2hnX8r8Kmi1CAsl36QAAAABJRU5ErkJggg==" border="0" title="AutoConnect menu" alt="AutoConnect menu"/></a>
</p>
```

<script>
  window.onload = function() {
    Gifffer();
  };
</script>
