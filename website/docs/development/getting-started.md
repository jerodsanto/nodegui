## Getting started

This library aims to be a nodejs addon which can export Qt Widgets to the Javascript world. By doing so one can develop fully fledged cross platform native GUI applications using only Javascript.

The library depends on `qode` which is a lightly modified version of NodeJS. The slight modification was needed to make it work with this addon. In essense, we will do `qode your_file.js` instead of `node your_file.js`.

Qode is inspired by this post by [Cheng Zhao](https://github.com/zcbenz): https://electronjs.org/blog/electron-internals-node-integration

This library does not modify Qt in any way and only use it as it is. This library also dynamically links to Qt. So it needs Qt libs to be installed in your system to work (This is done to keep in compliance with open source LGPL license of Qt). We can think of exporting the required libs later.

## Code Structure

```
.
├── binding.gyp
├── config
├── demo.ts
├── package.json
├── src
│   ├── cpp    <-- C++ source code
│   └── lib    <-- Typescript source code
├── tsconfig.json
└── yarn.lock
```

The main folder is `src`. It contains

- `cpp` : This folder contains all the C++ source code. Basically all the wrapper code using NAPI to export Qt Widgets and other helper functions to Javascript.
- `lib` : This folder contains all the Typescript code of the library. This is used to add additonal helper methods and types to exported addon.

**Detailed version:**

```
.
├── binding.gyp
├── config
│   ├── application.gypi
│   ├── common.gypi
│   └── yoga.gypi
├── demo.ts
├── package.json
├── src
│   ├── cpp
│   │   ├── Extras
│   │   ├── QtGui         <------ All exported classes found inside Qts Gui dynamic library
│   │   ├── QtWidgets     <------ All exported classes found inside Qts Widgets dynamic library
│   │   ├── core
│   │   └── main.cpp
│   └── lib
│       ├── QtGui
│       ├── QtWidgets
│       └── core
├── tsconfig.json
└── yarn.lock

```

First step to seeing how everything works is to take a look at `demo.ts` file. This file is basically like a Kitchen application showcasing all the exported widgets currently with the library.

Make sure you have read how to write native NodeJS Addons blog first. https://medium.com/@atulanand94/beginners-guide-to-writing-nodejs-addons-using-c-and-n-api-node-addon-api-9b3b718a9a7f

Once you have done that check out `src/cpp/main.cpp` and `config/application.gypi` to see the list of exported C++ classes.

Then maybe you can take a look at `src/cpp/QtWidgets/QLabel/qlabel_wrap.h`. This will show you how to wrap a simple Qt Widget.
Check the corresponding JS file for the addon here `src/lib/QtWidgets/QLabel/index.ts`.

## Wrapping a widget

Create wrappers for each and every Qt class that you will use with N-API (using node-addon-api since it is c++) and export it onto JS side.

Taking the example of QLabel, if you look inside the directory `src/cpp/QtWidgets/QLabel`, you should see:

```
├── QLabel
│   ├── nlabel.cpp
│   ├── nlabel.h     <---- Extended QLabel
│   ├── nlabel_moc.cpp <--- Autogenerated file by qt moc.
│   ├── qlabel_wrap.cpp
│   └── qlabel_wrap.h <--- Wrapper file
```

The idea is :

1. We will first extend QLabel class to form NLabel. NLabel is basically QLabel with some extra methods and variables. More on it below.
2. Then we will use NLabel and wrap it using NAPI and export it to JS side. This is what qlabel_wrap does.

**NLabel**: Since NLabel has inherited from QLabel we can treat is as QLabel with extra methods and properties. Primary reason to extend QLabel to create NLabel is to add support for Event listeners and CSS styling using Flex.
So if you take a look at NLabel you will see, it inherits from QLabel and NodeWidget. NodeWidget inturn inherits from YogaWidget and EventWidget. Event widget adds event handling support. YogaWidget is a class that contains the magic that enables a regular Qt Widget to have Yoga node. A Yoga node is an instance used by yoga library to calculate a widgets position on the screen. Yoga is a library that will layout the widget on the screen. To do so we will specify the flex properties like alignitems, justify content, margin, paddings etc on the Yoga node of the widget. Apart from adding yoga node, YogaWidget adds support for specifying those yoga properties via Qt's stylesheet. (This is done by using Q_PROPERTY). To make this work we need to use something called as Q_OBJECT inside the class which is a C++ macro. Q_OBJECT will be expanded to relevant code by the compiler. In Qt whenever we add Q_OBJECT to a header file, we need to use a pre compiler called Qt MOC (Meta Object Compiler). The way we use it is

```
moc headername.h -o headername_moc.cpp --include <any_include_file_which_contains_macro>  // example : ../../core/YogaWidget/yogawidget.h
```

So for nlabel I would run it as:

```
moc nlabel.h -o nlabel_moc.cpp --include ../../core/YogaWidget/yogawidget.h
```

This will run moc on `headername.h` and generate `headername_moc.cpp`. We will include `headername_moc.cpp` in `config/moc.gypi`. If you dont do this. Then it will give a symbol not found error.

I hope QLabel's example is enough for now. For more examples and inspirations we can take a look at other wrapped widgets.

## Learning Materials

1. Beginners guide to NodeJS Addon - https://medium.com/@atulanand94/beginners-guide-to-writing-nodejs-addons-using-c-and-n-api-node-addon-api-9b3b718a9a7f
2. First read this: N-API in nodejs docs
3. https://www.youtube.com/watch?v=-Oniup60Afs&feature=youtu.be
4. See samples at https://github.com/nodejs/abi-stable-node-addon-examples/
   4.1. You can see the readme of https://github.com/nodejs/node-addon-api.git/
5. See node-qt implementation. It is implemented in Nan (explained in video).
6. Now try to match the implementation in node-qt and convert to N-API using examples from samples.
7. Implementations not in node-qt need to be done with effort.
