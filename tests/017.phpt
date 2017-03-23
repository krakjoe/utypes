--TEST--
utypes verify complex unloaded
--SKIPIF--
<?php if (!extension_loaded("utypes")) print "skip"; ?>
--FILE--
<?php 
interface IFunction
{
	public function IFunction(Some $other);
}

utypes\handler(function(string $type, $value){
	switch ($type) {
		case "IFunction":
			return utypes\verify($value, IFunction::class);
	}

	return false;
});

function test(IFunction $num) {
	return $num;
}

var_dump(test(function (Some $other) {

}));
?>
--EXPECT--
object(Closure)#2 (1) {
  ["parameter"]=>
  array(1) {
    ["$other"]=>
    string(10) "<required>"
  }
}





