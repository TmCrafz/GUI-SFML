# GUI-SFML

![alt tag](/images/gui_sfml_example.jpg "Example Screenshot")

A small GUI System for SFML. Just for learning purpose and gather experience. It is useable but have some bugs and some strange behaviors at the moment. Scene can be created from xml files. There are some examples in the examples folder.

## Installtion and Libraries
To use GUI-SFML you need minimum SFML 2.4.0.
GUI-SFML has the tinyxml2 library included which is released under the Zlib licence (Details of licence are at the top of the source file).
To use GUI-SFML you have to add the path of the GUI-SFML project to your projects makefile and include GUI-SFML.hpp.
To test a example you have to navigate to one of the example folders and create the example with 'make' command.


## Available Widgets
- TextWidget
- TextInputWidget
- ButtonWidget
- TextButtonWidget
- CheckBoxWidget
- ListBoxWidget
- ComboBoxWidget
- ConsoleWidget
- ProgressWidget
- ScrollableWidget
- VerticalLayoutWidget
- HorizontalLayoutWidget
- WindowWidget
- Widget (can be used as a container for other widget)

## Note
The size of a widget which you specifiy by creating or with setWidth and setHeight methods are not always the real size of the widget. There are additional stuff which can affect the size of a widget like its outline thickness or scrollbars, topbars and stuff like that. You have to take careof it when you create your scenes. You can get the real size of a widget by calling getGlobalBounds().
