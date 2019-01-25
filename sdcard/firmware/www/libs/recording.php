<?php


// **************************************************************
// ** Video recording functions                                **
// **************************************************************

class VideoRecording
{
    protected static $recording_path = "/tmp/sd/RECORDED_VIDEOS";

    protected static $request_path   = "/dev/shm/rtspd_video";

    protected static $last_path      = "/dev/shm/rtspd_last_video_path";

    public function __construct() { }

    public static function GetLast() {
        $content = @file_get_contents(self::$last_path);
        if ($content == FALSE) {
            throw new \Exception(
                sprintf('No video found yet! Please create one first! (Error: %s not found)', self::$last_path)
            );
        }
        return $content;
    }

    public static function GetUrl($filename) {
        $replace = preg_quote(self::$recording_path, '/');
        return preg_replace("/$replace/", '/videos', $filename);
    }

    public static function GetLastUrl() {
        $last = self::GetLast();
        return self::GetUrl($last);
    }

    public static function Create() {
        if (@file_put_contents(self::$request_path, "gimmeh", LOCK_EX) == FALSE) {
            throw new \Exception(
                sprintf('Failed to request a recording! (Error: cannot create %s)', self::$request_path)
            );
        }

        $count = 0;
        while (file_exists(self::$request_path) and ($count < 15))  {
            // * KILL ALL PERFORMANCE WITH FIRE!
            sleep(1);
            $count++;
        }

        if (file_exists(self::$request_path)) {
            throw new \Exception(
                'Failed to request video: Is rtspd running?'
            );
        }

        return self::GetLast();
    }
}


// **************************************************************
// ** Snapshot functions                                       **
// **************************************************************

class Snapshot
{
    public static $snapshot_path = "/tmp/sd/RECORDED_IMAGES";

    public static $request_path  = "/dev/shm/rtspd_snapshot";

    public static $last_path     = "/dev/shm/rtspd_last_snapshot_path";

    public function __construct() { }

    public static function RequestImage() {
        // * Request image file
        if (@file_put_contents(self::$request_path, "gimmeh", LOCK_EX) == FALSE) {
            throw new \Exception(
                sprintf('Failed to request a snapshot! (Error: cannot create %s)', self::$request_path)
            );
        }
        return true;
    }

    public static function GetLast() {
        $content = @file_get_contents(self::$last_path);
        if ($content == FALSE) {
            throw new \Exception(
                sprintf('No snapshot found yet! Please create one first! (Error: %s not found)', self::$last_path)
            );
        }
        return $content;
    }

    public static function GetUrl($filename) {
        $replace = preg_quote(self::$snapshot_path, '/');
        return preg_replace("/$replace/", '/snapshots', $filename);
    }

    public static function GetLastUrl() {
        $last = self::GetLast();
        return self::GetUrl($last);
    }

    public static function GetImage() {
        $last_image_path = self::GetLast();
        $image_path      = self::GetLast();

        self::RequestImage();

        // * First loop: wait until image request file is removed
        $count = 0;
        while (file_exists(self::$request_path) and ($count < 150)) {
            // * No really this is slowing down a lot.... But we're already sooooo slow...
            sleep(1);
            $count++;
        }

        // * Second loop: wait until image path has changed
        $count = 0;
        while (($image_path == $last_image_path) and ($count < 150)) {
            $image_path = self::GetLast();
            sleep(1);
            $count++;
        }

        passthru(sprintf('cat %s', self::GetLast()));

        return;
    }

    public static function Create() {
        self::RequestImage();

        $count = 0;
        while (file_exists(self::$request_path) and ($count < 15)) {
            usleep( 1000 );
            $count++;
        }

        if (file_exists(self::$request_path)) {
            throw new \Exception('Failed to request snapshot: Is rtspd running?');
        }

        return self::GetLast();
    }
}

