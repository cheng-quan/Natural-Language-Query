grammar Test;

cmd
: request (exist_report | (cc? object)+) EOF;
 
request 
: wh? vb? prp? dt?; // "Where is my", "Give me the"

object options {backtrack=true;}
: part1 | part2;

part1 options {backtrack=true;}
: subpart1 | subpart2;

subpart1 options {backtrack=true;}
: exist_filter_object
| attribute_filter_1
| attribute_filter_2
| attribute_filter_3;

subpart2 options {backtrack=true;}
: object_with_prefix_1
| object_with_prefix_2;

part2 options {backtrack=true;}
: subpart3 | subpart4;

subpart3
: metric_filter;

subpart4
: non_filter_object 
| object_no_prefix;

object_with_prefix_1
: prps? nn cd attribute ((in? exist_filter)=>exist_filter_object | (in dt? attribute in? element)=>attribute_filter_1 | (in element)=>attribute_filter_2)? in metric; // "top 3 call centers in sales"

object_with_prefix_2
: prps? jjs cd? attribute ((in? exist_filter)=>exist_filter_object | (in dt? attribute in? element)=>attribute_filter_1 | (in element)=>attribute_filter_2)? in metric; // "best call center by sales"

object_no_prefix 
: (attribute | metric);

non_filter_object 
: in dt? object_no_prefix; // "in sales", "by sales", "of call center"

exist_filter_object 
: in? exist_filter; // "in my region"

attribute_filter_1 // "in the categolry of books and movies"
: in? dt? attribute in? (cc? element)+;

attribute_filter_2 // "in NY", "in NY and DC"
: in (cc? element)+;

attribute_filter_3 // "last month"
: (dt | jj) attribute;

metric_filter // "with revenue greater than 1000000 dollars"
: in? metric op in? cd np?;

attribute : {input.LT(1).getText().equals("call") || input.LT(1).getText().equals("month") || input.LT(1).getText().equals("year") || input.LT(1).getText().equals("store")}? metadata;
element : {input.LT(1).getText().equals("2014") || input.LT(1).getText().equals("2010") || input.LT(1).getText().equals("2012") || input.LT(1).getText().equals("NY") || input.LT(1).getText().equals("DC")}? metadata;
metric : {input.LT(1).getText().equals("sales") || input.LT(1).getText().equals("revenue")}? metadata;
metadata : np (vb | np*) | jj | cd | nnp+;

exist_report : {input.LT(1).getText().equals("existingReport")}? exist_report_name; 
exist_report_name : TERM+;

exist_filter : {input.LT(1).getText().equals("existingFilter")}? exist_filter_name; 
exist_filter_name : prps np+ | jj np+ | dt np+; // "my region"

nn : {input.LT(1).getText().equals("top")}? TERM;
np : {input.LT(1).getText().equals("call") || input.LT(1).getText().equals("center") || input.LT(1).getText().equals("month") || input.LT(1).getText().equals("year") || input.LT(1).getText().equals("revenue") 
	|| input.LT(1).getText().equals("store") || input.LT(1).getText().equals("sales") || input.LT(1).getText().equals("centers") || input.LT(1).getText().equals("dollars")}? TERM; 
nnp : {input.LT(1).getText().equals("NY") || input.LT(1).getText().equals("DC")}? TERM; 
jj : {input.LT(1).getText().equals("good") || input.LT(1).getText().equals("last")}? TERM;
jjs : {input.LT(1).getText().equals("best")}? TERM;
cd : {input.LT(1).getText().equals("3") || input.LT(1).getText().equals("1000000") || input.LT(1).getText().equals("2014") || input.LT(1).getText().equals("2010") || input.LT(1).getText().equals("2012")}? NUMBER;
in : {input.LT(1).getText().equals("in") || input.LT(1).getText().equals("by") || input.LT(1).getText().equals("with") || input.LT(1).getText().equals("of") || input.LT(1).getText().equals("than")}? TERM;
cc : {input.LT(1).getText().equals("and")}? TERM;
prps : {input.LT(1).getText().equals("my")}? TERM;
prp : {input.LT(1).getText().equals("me")}? TERM;
vb : {input.LT(1).getText().equals("give") || input.LT(1).getText().equals("show") || input.LT(1).getText().equals("is")}? TERM;
dt : {input.LT(1).getText().equals("the") || input.LT(1).getText().equals("this")}? TERM; 
wh : {input.LT(1).getText().equals("where")}? TERM;
op : {input.LT(1).getText().equals("greater") || input.LT(1).getText().equals("than") || input.LT(1).getText().equals("of")}? TERM;

TERM: ('a'..'z'|'A'..'Z')+;
NUMBER: '0'..'9'+;
WHITESPACE: ' ' {skip();};
COMMA: ',' {skip();};
