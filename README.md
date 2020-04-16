# ndi test

To be run on macos

## Install opencv

```
brew install opencv
```

## Install NDI Tools

[NDI Tools for macos](http://mkto-q0143.com/v0rP0abn0MfCQirqV03k000)

## Compile this sample project

```
g++ $(pkg-config --cflags --libs opencv4) -std=c++11 main.cpp $(pwd)/lib/osx/x64/libndi.4.dylib
```

## Run it

```
./a.out
```

## Open NewTek NDI Video Monitor app

Then go to `File>YOUR_COMPUTER>NDI-Test`

## Expected result

You should receive the NDI stream on the NewTek NDI Video Monitor app
