# bypass - luminosity

A 64kb intro for Deadline 2024.

rtz   - 3d scenes, direction and music

supah - real-time synth/tracker code and animations

warp  - shaders, rendering and intro code

Thanks to Erwin Orb for mastering and feedback.
(https://www.youtube.com/@erwinorb)

Please refer to the contained infofile (bypass-luminosity.txt) for more information.

## Run info

You will need a WebGPU-capable browser, like Chromium/Chrome or Edge, that supports indirect compute dispatches. Please use the `--allow-file-access-from-files` option for Chrome or start a local webserver (`python3 -m http.server`) in the intro directory to run. Linux users might try their luck with `google-chrome-stable --enable-unsafe-webgpu --enable-features=Vulkan`. (To make it run in Firefox Nightly, a couple of modifications to the shader code are required, which are not contained in this repository.)

## Build info

Use the provided Makefile to build the project. The source (main.js and main.c) contains a number of options to build the project in different flavors, e.g. capture mode, editor mode etc. You will need the following tools to package and compress the input files (JavaScript and shader).

https://github.com/mgnauck/wgslminify

https://github.com/mgnauck/js-payload-compress

https://github.com/terser/terser
