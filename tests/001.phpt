--TEST--
utypes recv pass
--SKIPIF--
<?php if (!extension_loaded("utypes")) print "skip"; ?>
--FILE--
<?php 
utypes(function(string $type, $value){
	switch ($type) {
		case "fake":
			return true;
	}

	return false;
});

function test(fake $thing) {
	
}

test(10);
?>
OK
--EXPECT--
OK
