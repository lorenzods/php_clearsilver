--TEST--
Test cs/hdf resource leak
--SKIPIF--
<?php if (!extension_loaded("clearsilver")) print "skip clearsilver extension not loaded"; ?>
--FILE--
<?php
chdir(dirname(__FILE__));
for ($i=0;$i<10;$i++) {
	$hdf = hdf_init();
	var_dump($hdf);
	hdf_set_value($hdf, 'message', 'Hello World!');
	$cs = cs_init( $hdf );
	var_dump($cs);
	cs_parse_file($cs, 'test.cs' );
	cs_render($cs);
	cs_destroy( $cs );
	var_dump($cs);
	hdf_destroy( $hdf );
	var_dump($hdf);
}
?>
--EXPECTF--
resource(%i) of type (ClearSilver HDF)
resource(%i) of type (ClearSilver CS)
resource(%i) of type (Unknown)
resource(%i) of type (Unknown)
resource(%i) of type (ClearSilver HDF)
resource(%i) of type (ClearSilver CS)
resource(%i) of type (Unknown)
resource(%i) of type (Unknown)
resource(%i) of type (ClearSilver HDF)
resource(%i) of type (ClearSilver CS)
resource(%i) of type (Unknown)
resource(%i) of type (Unknown)
resource(%i) of type (ClearSilver HDF)
resource(%i) of type (ClearSilver CS)
resource(%i) of type (Unknown)
resource(%i) of type (Unknown)
resource(%i) of type (ClearSilver HDF)
resource(%i) of type (ClearSilver CS)
resource(%i) of type (Unknown)
resource(%i) of type (Unknown)
resource(%i) of type (ClearSilver HDF)
resource(%i) of type (ClearSilver CS)
resource(%i) of type (Unknown)
resource(%i) of type (Unknown)
resource(%i) of type (ClearSilver HDF)
resource(%i) of type (ClearSilver CS)
resource(%i) of type (Unknown)
resource(%i) of type (Unknown)
resource(%i) of type (ClearSilver HDF)
resource(%i) of type (ClearSilver CS)
resource(%i) of type (Unknown)
resource(%i) of type (Unknown)
resource(%i) of type (ClearSilver HDF)
resource(%i) of type (ClearSilver CS)
resource(%i) of type (Unknown)
resource(%i) of type (Unknown)
resource(%i) of type (ClearSilver HDF)
resource(%i) of type (ClearSilver CS)
resource(%i) of type (Unknown)
resource(%i) of type (Unknown)
