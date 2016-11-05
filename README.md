# ASD
ASD is the multi-functional modular set of C++ libraries. The main purpose of this project is to make development on C++ more effective and clear. It is designed to be cross-platform in the nearest future!

ASD does not only extensively use almost every feature of C++ 11/14 (and 17 too?) standards, it also embodies some useful concepts, such as SFINAE, reference-based memory management, RTTI, static-function-based singletons, message (event) system, etc. The focus is made on compile-time programming.

Each library has its own power. There is no need to use all the libraries. Feel free to take only libraries which you need!

ASD uses CMake to build the libraries. There are some additional cmake tools that will help you to create new libraries, add sources and manage dependencies in very, VERY simple way. It is really something different.

## Possibilities
What can you do with this now? Do you want to make a small console application which will calculate something? Of course, The asd-Core and the asd-math modules will provide all necessary utilities to you. And if you need basic control on application lifetime out-of-box, the asd-application will help you with that...

But, maybe, you can wish something more complex. Maybe... A video game? Haha, game development is the REAL destination of ASD! The RST will try to provide as much help to you as it can without decreasing of development flexibility.

### Graphics
The basis of every video game is the rendering system. The asd-Graphics is the main part of this system in the RS-Toolkit. It provides the abstract classes which will allow you to draw different 2D and 3D objects. The asd-OpenGL and the asd-Direct3D modules contain the implementations for this abstract classes. And, of course you can use them not only for gamedev but in any project which requires visualisation.

### Virtual world
Rendering of graphic objects is not the only thing we need in a video game. We need a place for these objects too. The asd-Scene includes the useful classes to create a virtual world where these objects can be placed in and managed effectively.

### Physics
Usually, some objects should physically interact with each other in video games, and this interaction may be very complex sometimes. ASD contains the asd-Physics module to simplify control on all interactions in virtual worlds by providing the additional physical layer.

### User interface
We haven't forget about user interface too. The RST includes the asd-UI module which will help you to create widgets, windows and complex visual components. The main principles of the user interface in the RST are the freedom of choice and lightweight of all components. Widgets in the RST use message system provided by the asd-Core so you can easily control their behaviour.

### Interaction
The asd-Interaction library provides advanced interaction mechanisms. It is based on components which can be connected to elements of user interface and perform different actions over user input to generate special interaction behaviour and provide some processed data to another subsystems (mouse rotation, dragging and so on).
