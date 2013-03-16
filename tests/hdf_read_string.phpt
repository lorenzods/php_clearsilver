--TEST--
Test hdf_read_string() behavior
--SKIPIF--
<?php if (!extension_loaded("clearsilver")) print "skip clearsilver extension not loaded"; ?>
--FILE--
<?php
$hdf = hdf_init();
var_dump(hdf_read_string($hdf, 'Page.Title = The Great Divide: Global Economic Disparity')); //dotted path scheme
var_dump(hdf_read_string($hdf, "Page {
Hypothesis = Humanity's greatest medium term challenge
}")); //nested element scheme
?>
--EXPECT--
bool(true)
bool(true)