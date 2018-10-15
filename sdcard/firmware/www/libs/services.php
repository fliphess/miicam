<?php

// **************************************************************
// ** GPIO Functions                                           **
// **************************************************************

class Services
{
    public function __construct() {}

    public static function IsRunning($name) {
        if (is_executable('/usr/bin/pgrep')) {
            $command = sprintf("/usr/bin/pgrep %s 2>&1", escapeshellarg($name));
            exec($command, $output, $return);

            if (!empty($output) && ($return == 0)) {
                return true;
            }
            return false;
        }
    }







}

?>
