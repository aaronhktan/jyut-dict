To capture a video demonstrating usage of the app (**macOS ONLY**):
  - Option 1: Use OBS, with two sources.
    - One "Window Capture", with the window set to Jyut Dictionary.
    - One "Display Capture", set to a monitor with a white background.
    - Under Settings > Video > Canvas Resolution, set the resolution to the same resolution as a screenshot with border shadowing. (In the current image, that would be 1824x1468.)
  - Option 2: Use Cleanshot X to capture a video of the Jyut Dictionary screen
  - Once the video has been captured, use the following command to generate a gif.
    - `ffmpeg -ss <start time, minutes:seconds> -t <duration, minutes:seconds> -i <generated video file> -vf "fps=30,scale=<width of video>:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" -loop 0 <output filename>`

To stitch together a GIF using multiple screenshots:
  - Run `./scripts/create_gif.sh ./<folder containing pngs labelled 1.png, 2.png, etc> <output_name>.mp4 <output_name>.gif