<?php

// **************************************************************
// ** Snapshot functions                                       **
// **************************************************************

class Snapshot
{
    protected static $ffmpeg = '/tmp/sd/firmware/bin/ffmpeg';

    protected static $snapshot_destination = "/tmp/sd/firmware/www/public/snapshot.jpg";

    protected static $command_string = "%s -loglevel fatal -i rtsp://127.0.0.1:554/live/ch00_0 -vframes 1 -r 1 -y %s 2>&1";

    public function __construct() { }

    protected function __GetDir() {
        return sprintf('/tmp/sd/RECORDED_IMAGES/%s', strftime("%Y/%m/%d"));
    }

    // * Determine file destination and create directory structure
    protected function __PrepareFile() {
        $directory = self::__GetDir();
        OS::CreateDir($directory);

        return $directory . "/" . strftime("%H%M%S.jpg");
    }

    // * Take a picture and save it to a file
    protected function __TakePicture($filename) {
        $command = sprintf(self::$command_string, self::$ffmpeg, $filename);
        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf("Error executing:\n%s\n%s", $command, implode(" ", $output))
            );
        }
        return $output;
    }

    // * Copy the current (last) snapshot to our images directory
    public function SaveCurrent() {
        $filename = self::__PrepareFile();

        if (!copy(self::$snapshot_destination, $filename)) {
            throw new \Exception(
                sprintf('Failed to copy %s to %s', self::$snapshot_destination, $filename)
            );
        } else {
            return $filename;
        }
        return false;
    }

    // * Save an image to our image directory
    public function SaveNew() {
        if ((!getenv('ENABLE_RTSP')) && (!Services::IsRunning('rtspd'))) {
            throw new \Exception(
                "Can't create snapshots when RTSP is not running!"
            );
        }

        $filename = self::__PrepareFile();
        $output   = self::__TakePicture($filename);

        return ($output) ? $filename : null;
    }

    public static function Create() {
        if ((!getenv('ENABLE_RTSP')) && (!Services::IsRunning('rtspd')))  {
            throw new \Exception(
                "Can't create snapshots when RTSP is not running!"
            );
        }
        return self::__TakePicture(self::$snapshot_destination);
    }
}
