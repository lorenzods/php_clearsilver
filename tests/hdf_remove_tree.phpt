--TEST--
Test hdf_remove_tree() behavior
--SKIPIF--
<?php if (!extension_loaded("clearsilver")) print "skip clearsilver extension not loaded"; ?>
--FILE--
<?php
$hdf = hdf_init();
hdf_set_value($hdf, 'Page.Menu.0', 'My Menu');
var_dump(hdf_remove_tree($hdf, 'Page.Menu'));
?>
--EXPECT--
bool(true)