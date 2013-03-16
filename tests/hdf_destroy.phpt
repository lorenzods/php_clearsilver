--TEST--
Test hdf_destroy() behavior
--SKIPIF--
<?php if (!extension_loaded("clearsilver")) print "skip clearsilver extension not loaded"; ?>
--FILE--
<?php
$hdf = hdf_init();
var_dump($hdf);
$node = hdf_get_obj($hdf, 'Page.Menu.0');
var_dump(@hdf_destroy($node)); //suppress error message due to invalid HDF parameter
$node = hdf_get_node($hdf, 'Page.Menu.0');
var_dump(@hdf_destroy($node)); //suppress error message due to HDF not being root node
var_dump(hdf_destroy($hdf));
var_dump($hdf);
?>
--EXPECTF--
resource(%i) of type (ClearSilver HDF)
bool(false)
bool(false)
bool(true)
resource(%i) of type (Unknown)