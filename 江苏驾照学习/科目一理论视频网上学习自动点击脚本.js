var __index=0;
var __intervalIndex=0;
var dot='.';
__intervalIndex = setInterval(function () {


var __btnNext = getElementsByClassName('dfss_down')[0];




if (__index>6) {
console.clear();
__index=0;
dot='';
}


if (__btnNext!=null&&__btnNext.childNodes[0]!=null) {
__btnNext.childNodes[0].click();
}else{
console.clear();
console.log('playing'+dot);
}
__index=__index+1;
dot+='.';
},1000);




function getElementsByClassName (className) {
var all = document.all ? document.all : document.getElementsByTagName('*');
var elements = new Array();
for ( var e = 0; e < all.length; e ++ ) 
{ 
if (all[e].className == className) {
elements[elements.length] = all[e]; 
break ; 
} 
} 
return elements;
}