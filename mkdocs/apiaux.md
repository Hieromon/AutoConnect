## <i class="fa fa-code"></i> Constructor

### AutoConnectAux

```cpp
AutoConnectAux(const String& uri = String(""), const String& title = String(""), const bool menu = true, const AutoConnectElementVT addons = AutoConnectElementVT())
```
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">uri</span>URI of this custom Web Page.</dd>
    <dd><span class="apidef">title</span>Page title of this custom web page. It will appear on the auto connection menu and at the top of that page.</dd>
    <dd><span class="apidef">addons</span>Reference to AutoConnectElement collection.</dt>
</dl>

## <i class="fa fa-code"></i> Public member functions

### add

```cpp
void add(AutoConnectElement& addon)
```
```cpp
void add(AutoConnectElementVT addons)
```
Add an element to the AutoConnectAux. An added element is displayed on the custom Web page invoked from the AutoConnect menu.


  template<typename T>
  T&    getElement(const String& name);
  AutoConnectElement* getElement(const String& name);                   /**< Get registered AutoConnectElement as specified name */
  void  menu(const bool post) { _menu = post; }                         /**< Set or reset the display as menu item for this aux */
  bool  release(const String& name);                                    /**< Release an AutoConnectElement */
  bool  setElementValue(const String& name, const String value);        /**< Set value to specified element */
  bool  setElementValue(const String& name, std::vector<String> const& values);  /**< Set values collection to specified element */
  void  setTitle(const String title) { _title = title; }                /**< Set a title of the auxiliary page */
  void  on(const AuxHandlerFunctionT handler, const AutoConnectExitOrder_t order = AC_EXIT_AHEAD) { _handler = handler; _order = order; }   /**< Set user handler */
  bool load(const String& in);                                          /**< Load whole elements to AutoConnectAux Page */
  bool load(const __FlashStringHelper* in);                             /**< Load whole elements to AutoConnectAux Page */
  bool load(Stream& in);                                                /**< Load whole elements to AutoConnectAux Page */
  bool loadElement(const String& in, const String& name = String(""));              /**< Load specified element */
  bool loadElement(const __FlashStringHelper* in, const String& name = String("")); /**< Load specified element */
  bool loadElement(Stream& in, const String& name = String(""));       /**< Load specified element */
  size_t  saveElement(Stream& out, std::vector<String> const& names = {});    /**< Write elements of AutoConnectAux to the stream */
