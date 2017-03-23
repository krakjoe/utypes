--TEST--
utypes recv variadic
--SKIPIF--
<?php if (!extension_loaded("utypes")) print "skip"; ?>
--FILE--
<?php 
utypes\handler(function(string $type, $value){
	var_dump($type, $value);

	return true;
});

function test(some ... $vars) {
	
}

test(10, 40);
?>
--EXPECT--
string(4) "some"
array(2) {
  [0]=>
  int(10)
  [1]=>
  int(40)
}

