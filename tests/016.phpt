--TEST--
utypes verify variadic last
--SKIPIF--
<?php if (!extension_loaded("utypes")) print "skip"; ?>
--FILE--
<?php 
interface IFunction
{
	public function IFunction(int $int, ... $thing);
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

var_dump(test(function (int $int, ... $thing) {

}));
?>
--EXPECT--
object(Closure)#2 (1) {
  ["parameter"]=>
  array(2) {
    ["$int"]=>
    string(10) "<required>"
    ["$thing"]=>
    string(10) "<optional>"
  }
}




