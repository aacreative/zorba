import module namespace j = "http://jsoniq.org/functions";

variable $o := { "foo" : "bar" };

(
insert json { "bar" : "foo" } into $o, 
insert json { "baz" : "foo", "bar": "so" } into $o
);

$o
