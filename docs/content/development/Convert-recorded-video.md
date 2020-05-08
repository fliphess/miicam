## Convert recorded video

As the recorded video is at the moment not readable by VLC or other media players,
you need to convert the recorded `*.h264` files to something else using `ffmpeg` to play your recorded media files.


## Convert the video files

To do this, use the following command line string:

```bash
ffmpeg -vcodec h264 -i <video.h264> -vcodec copy -acodec copy <output>.m4v
```
