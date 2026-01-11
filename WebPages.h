#ifndef WEB_PAGES_H
#define WEB_PAGES_H

#include <Arduino.h>

class WebPages {
public:
  // Get HTML page content
  static String getIndexHTML();
  
  // Get inline CSS
  static String getInlineCSS();
  
  // Get inline JavaScript
  static String getInlineJavaScript();
};

#endif
