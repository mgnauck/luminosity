# bypass - luminosity

A 64kb intro for Deadline 2024.

rtz   - 3d scenes, direction and music

supah - real-time synth/tracker code and animations

warp  - shaders, rendering and intro code

Thanks to Erwin Orb for mastering and feedback.
(https://www.youtube.com/@erwinorb)

## Run info

You will need a WebGPU-capable browser, like Chromium/Chrome or Edge, that supports indirect compute dispatches (Firefox currently does not). Please use the `--allow-file-access-from-files` option for Chrome or start a local webserver (`python3 -m http.server`) in the intro directory to run. Linux users might try their luck with `google-chrome-stable --enable-unsafe-webgpu --enable-features=Vulkan`.

## Build info

Use the provided Makefile to build the project. The source (main.js and main.c) contains a number of options to build the project in different flavors, e.g. capture mode, editor mode etc. You will need the following tools to package and compress the input files (JavaScript and shader).

https://github.com/mgnauck/wgslminify

https://github.com/mgnauck/js-payload-compress

https://github.com/terser/terser
