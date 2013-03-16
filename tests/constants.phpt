--TEST--
Test constants
--SKIPIF--
<?php if (!extension_loaded("clearsilver")) print "skip clearsilver extension not loaded"; ?>
--FILE--
<?php
var_dump(is_numeric(@PHP_CLEARSILVER_VERSION));
var_dump((float) @PHP_CLEARSILVER_VERSION);
?>
--EXPECTF--
bool(true)
float(%f)
