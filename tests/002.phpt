--TEST--
utypes recv fail
--SKIPIF--
<?php if (!extension_loaded("utypes")) print "skip"; ?>
--FILE--
<?php 
utypes(function(string $type, $value){
	switch ($type) {
		case "fake":
			return false;
	}

	return false;
});

function test(fake $thing) {
	
}

test(10);
?>
OK
--EXPECTF--
Fatal error: Uncaught TypeError: Argument 1 passed to test() must be an instance of fake, integer given, called in %s on line 15 and defined in %s:11
Stack trace:
#0 %s(15): test(10)
#1 {main}
  thrown in %s on line 11

